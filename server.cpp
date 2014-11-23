#include "server.h"

Server::Server(QObject *parent) :
    QTcpServer(parent)/*,
    window((QMainWindow*)parent)*/
{
    manager.establishConnection();
    //connect(&onlineUsers, SIGNAL(changed(QList<User*>)), window, SLOT(updateUI(QList<User*>)));

}

void Server::start() {
    if(!this->listen(QHostAddress::Any, PORTNO)) {
        qDebug() << "Could not start server";
    }
    else {
        qDebug() << "Listening to port " << PORTNO << "...";
    }
}

void Server::shutdown() {
    // Stop listening
    this->close();
    qDebug() << "Server stopped listening.";

    // Close each connection and delete sockets
    foreach (QSocket *socket, sockets) {
        socket->close();
        socket->deleteLater();
    }

    // Delete all online users
    onlineUsers.clear();

    manager.closeConnection();
    qDebug() << "Server is shut down";
}

void Server::disconnected() {
    // Get calling socket
    QSocket *socket = qobject_cast<QSocket*>(sender());
    qDebug() << socket->peerAddress().toString() << " disconnected.";
    // Remove it from active sockets and delete
    sockets.removeAll(socket);
    socket->deleteLater();
}

// This function is called by QTcpServer when a new connection is available.
void Server::incomingConnection(qintptr socketDescriptor) {
    // We have a new connection
    qDebug() << " Connecting " << socketDescriptor;

    QSocket *socket = new QSocket(this);

    // Set ID and connect signals, if failed, delete socket
    if(socket->setSocketDescriptor(socketDescriptor)) {
        connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));

#ifdef ENCRYPTED
        connect(socket, SIGNAL(encrypted()), this, SLOT(socketReady()));
        connect(socket, SIGNAL(sslErrors(const QList<QSslError>)), this, SLOT(errorOccured(QList<QSslError>)));
        // Set needed encryption infiormation and start connection encryption
        setSsl(socket);
        socket->startServerEncryption();
#else
        qDebug() << socket->peerAddress().toString() << " connected.";
        connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
        connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(errorOccured(QAbstractSocket::SocketError)));
#endif

        sockets.append(socket);
    }
    else {
        delete socket;
    }
}


void Server::readyRead() {
    // Get calling socket
    QSocket *socket = qobject_cast<QSocket*>(sender());
#ifdef ENCRYPTED
    if (socket->isEncrypted()) {
        //QSslConfiguration config = socket->sslConfiguration();
#else
    if (socket->isReadable()) {
#endif
        // Read all data
        QByteArray data = socket->readAll();
        qDebug() << socket->peerAddress().toString() << ": " << data;

        switch (data.at(0))
        {
            case ECHO:      echo(data, socket); break;
            case LOGIN:     login(data.mid(1), socket); break;
            case LOGOUT:    logout(data.mid(1), socket); break;
            case REGISTER:  registerUser(data.mid(1), socket); break;
            case GETUSERS:  getUserList(socket); break;
            default:        qDebug() << "Unknown command = " << data.at(0);
        }
    }
    else {
        qDebug() << "Cannot read from socket, either due to security problem (if enabled encryption), or it's not readable.";
    }
}

#ifdef ENCRYPTED
int Server::setSsl(Socket *socket) {
    // Set all needed SSL information, such as ciphers, certificates, mode, ...

    return 0;
}

void Server::errorOccured(QList<QSslError> errors) {
    foreach (QSslError error, errors) {
        qDebug() << "SSL error during hadshake: " << error.errorString();
    }
}

void Server::socketReady() {
    // Get calling socket
    QSocket *socket = qobject_cast<QSocket*>(sender());
    qDebug() << socket->peerAddress() << " connected.";

    // Start recieving data
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
}
#else
void Server::errorOccured(QAbstractSocket::SocketError error) {
    qDebug() << error;
}
#endif

void Server::echo(QByteArray data, QSocket *socket) {
    int n;
    if ((n = socket->write(data)) > 0) {
        qDebug() << n << " bytes written to " << socket->peerAddress().toString();
    }
}

bool Server::registerUser(QByteArray data, QSocket *socket) {
    if (!manager.isConnected()) {
        qDebug() << "DB is not connected!";
        return false;
    }

    int n = data.indexOf(commandDelimiter);
    QByteArray response;
    response.append((char)REGISTER);

    if (n < 0) {
        socket->write(response.append('\0').append("Wrong format of data."));
        return false;
    }

    QByteArray name = data.left(n);
    QByteArray key = data.mid(n+1);
    User *user = new User(this, name, key, socket);
    bool result = manager.insertUser(user);
    if (result) {
        response.append('\1')
                .append("Registered as \"" + user->getUsername().toLocal8Bit() + "\" with ID: ")
                .append('0' + user->getID())
                .append(". Now you can login.");
    }
    else {
        response.append('\0');
        response.append("Could not register. Username already in use.");
    }
    socket->write(response);
    user->deleteLater();
    return result;
}

bool Server::login(QByteArray userName, QSocket* socket) {
    if (!manager.isConnected()) {
        qDebug() << "DB is not connected!";
        return false;
    }

    User* u;
    QByteArray response;
    response.append((char)LOGIN);
#ifdef ENCRYPTED
    u = manager.getUserByNameAndKey(userName, );
#else
    QList<User*> users = manager.getUsersByName(userName);
    if (users.isEmpty()) {
        response.append('\0');
        response.append("Cannot login, unregistered user or wrong username.");
        socket->write(response);
        qDebug() << "Cannot login, unregistered user or wrong username.";
        return false;
    }
    u = users.first();
#endif
    if (u == NULL) {
        response.append('\0');
        socket->write(response);
        return false;
    }

    u->setSocket(socket);

    onlineUsers.append(u);
    emit usersChanged(&onlineUsers);

    manager.updateUser(u);

    response.append('\1');
    //response.append("Logged as \"" + u->getUsername().toLocal8Bit() + "\"\nYour ID: ").append('0' + u->getID());
    socket->write(response);

    return true;
}

bool Server::logout(QByteArray userName, QSocket *socket) {
    foreach(User* u, onlineUsers) {
        if (u->getUsername() == QString(userName)) {
            onlineUsers.removeAll(u);
            QByteArray response;
            response.append((char)LOGOUT);
            socket->write(response);
            emit usersChanged(&onlineUsers);
            return true;
        }
    }

    return false;
}

bool Server::getUserList(QSocket *socket) {
    QByteArray userList;
    userList.append((char)GETUSERS);

    foreach (User* u, onlineUsers) {
        userList.append(u->getUsername())
                .append(commandDelimiter)
                .append(u->getPubKey())
                .append(commandDelimiter)
                .append(u->getSocket()->peerAddress().toString())
                .append(commandDelimiter);
    }

    return socket->write(userList) > 0;
}

QList<User*>* Server::getUsers() {
    return &onlineUsers;
}

bool Server::isUserAlive(User *u) {
    return false;
}

void Server::refresh() {

}
