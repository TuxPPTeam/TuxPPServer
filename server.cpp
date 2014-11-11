#include "server.h"

Server::Server(QMainWindow *w, QObject *parent) :
    QTcpServer(parent),
    window(w)
{
    manager.establishConnection();
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

    // Close each connection and delete sockets
    foreach (QSocket *socket, sockets) {
        socket->close();
        socket->deleteLater();
    }

    // Delete all online users
    foreach (User* u, onlineUsers) {
        u->deleteLater();
    }

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
    qDebug() << socketDescriptor << " Connecting...";

    QSocket *socket = new QSocket(this);

    // Set ID and connect signals, if failed, delete socket
    if(socket->setSocketDescriptor(socketDescriptor)) {

#ifdef ENCRYPTED
        connect(socket, SIGNAL(encrypted()), this, SLOT(socketReady()));
        connect(socket, SIGNAL(sslErrors(const QList<QSslError>)), this, SLOT(errorOccured(QList<QSslError>)));
#else
        connect(socket, SIGNAL(connected()), this, SLOT(socketReady()));
#endif
        connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
        connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
        connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT());

#ifdef ENCRYPTED
        // Set needed encryption infiormation and start connection encryption
        setSsl(socket);
        socket->startServerEncryption();
#endif
        sockets.append(socket);
    }
    else {
        delete socket;
    }
}

void Server::socketReady() {
    // Get calling socket
    QSocket *socket = qobject_cast<QSocket*>(sender());
    qDebug() << socket->peerAddress().toString() << " connected.";

    // Start recieving data
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
}

void Server::errorOccured(QList<QSslError> errors) {
    foreach (QSslError error, errors) {
        qDebug() << "SSL error during hadshake: " << error.errorString();
    }
}

void Server::readyRead() {
    // Get calling socket
    QSocket *socket = qobject_cast<QSocket*>(sender());
#ifdef ENCRYPTED
    if (socket->isEncrypted()) {
        QSslConfiguration config = socket->sslConfiguration();
#else
    if (socket->isReadable()) {
#endif
        // Read all data
        QByteArray data = socket->readAll();

        switch (data.at(0))
        {
            case ECHO: echo(data.mid(sizeof(command)), socket); break;
            case LOGIN: login(data.mid(sizeof(command)), socket->peerAddress().toIPv4Address(), socket); break;
            case LOGOUT: logout(data.mid(sizeof(command))); break;
            //case REGISTER: registerUser(sizeof(command), socket->peerAddress().toIPv4Address()); break;
            case GETUSERS: getUserList(socket); break;
            default: qDebug() << "Unknown command = " + data.at(0);
        }

//################################################################################################
//######### REMOVE WHEN CONNECTING WITH CLIENT, NOT TERMINAL #####################################
//################################################################################################

        // Remove last 2 bytes - \n\r
        //data.chop(2);

//################################################################################################
//################################################################################################
//################################################################################################
        /*QList<QByteArray> tokens = data.split('|');

        // Register client
        if (tokens.at(0) == QByteArray("register")) {
            registerUser(tokens.at(1), tokens.at(2), socket->peerAddress().toIPv4Address());

        // Login client
        } else if (tokens.at(0) == QByteArray("login")) {
            login(tokens.at(1), socket->peerAddress().toIPv4Address(), socket);

        // Logout client
        } else if (tokens.at(0) == QByteArray("logout")) {
            logout(tokens.at(1));

        // Send client online users
        } else if (tokens.at(0) == QByteArray("getusers")) {
            getUserList(socket);

        // Unknown command
        } else {
            qDebug() << "Unknown command: " << tokens.at(0);
        }*/
    }
}

#ifdef ENCRYPTED
int Server::setSsl(Socket *socket) {
    // Set all needed SSL information, such as ciphers, certificates, mode, ...

    return 0;
}
#endif

void Server::echo(QByteArray data, QSocket *socket) {
    int n;
    if ((n = socket->write(data)) > 0) {
        qDebug() << n + " bytes written to " + socket->peerAddress().toString();
    }
}

bool Server::registerUser(QByteArray userName, quint32 host) {
    /*if (!manager.isConnected()) {
        qDebug() << "DB is not connected!";
        return false;
    }
    //User *user = new User(this, userName, host, pubKey);
    //bool result = manager.insertUser(user);
    //user->deleteLater();
    return result;*/
    return false;
}

bool Server::login(QByteArray userName, quint32 host, QSocket *socket) {
    if (!manager.isConnected()) {
        qDebug() << "DB is not connected!";
        return false;
    }
    QList<User*> users = manager.getUsersByName(userName);
    if (users.isEmpty()) {
        qDebug() << "Cannot login, unregistered user or wrong username";
        return false;
    }
    User *u = manager.getUsersByName(userName).first();
    if (u == NULL) {
        return false;
    }
    u->setHost(host);
    onlineUsers[u->getID()] = u;
    socket->write("Logged\n");
    emit dataReady(userName);
    socket->disconnect();
    return true;
}

bool Server::logout(QByteArray userName) {
    if (!manager.isConnected()) {
        qDebug() << "DB is not connected!";
        return false;
    }
    User *u = manager.getUsersByName(userName).first();
    if (u == NULL) {
        return false;
    }
    return !onlineUsers.remove(u->getID());
}

bool Server::getUserList(QSocket *socket) {
    QByteArray userList;
    foreach (User* u, onlineUsers) {
        userList.append(u->getUsername()).append(", ");
    }
    userList.chop(2);

    return socket->write(userList) > 0;
}
