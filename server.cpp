#include "server.h"
#include <QSslConfiguration>

Server::Server(QObject *parent) :
    QTcpServer(parent)/*,
    window((QMainWindow*)parent)*/
{
    manager.establishConnection();
    //connect(&onlineUsers, SIGNAL(changed(QList<User*>)), window, SLOT(updateUI(QList<User*>)));
    
}

void Server::start() {
    if (!manager.isConnected()) {
        manager.establishConnection();
    }
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
    qDebug() << "Error:" << socket->errorString();
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
        setSsl(socket);
        connect(socket, SIGNAL(encrypted()), this, SLOT(socketReady()));
        connect(socket, SIGNAL(sslErrors(const QList<QSslError>)), this, SLOT(errorOccured(QList<QSslError>)));
        // Set needed encryption infiormation and start connection encryption
        qDebug() << socket->state();
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
            case GENKEY:    generateKey(data.mid(1)); break;
            case CONREQ:
            case CONRESP:   forward(data, socket); break;
            default:        qDebug() << "Unknown command = " << data.at(0);
        }
    }
    else {
        qDebug() << "Cannot read from socket, either due to security problem (if enabled encryption), or it's not readable.";
    }
}

#ifdef ENCRYPTED
bool Server::setSsl(QSocket *socket) {
    // Set all needed SSL information, such as ciphers, certificates, mode, ...
    QSslCertificate ca = QSslCertificate::fromPath("../certs/labak/ca.crt").first();
    socket->addCaCertificate(ca);
    QSslCertificate server = QSslCertificate::fromPath("../certs/labak/server.crt").first();
    socket->setLocalCertificate(server);
    socket->setPrivateKey("../certs/labak/server.key");

    return true;
}

void Server::errorOccured(QList<QSslError> errors) {
    QSocket *socket = qobject_cast<QSocket*>(sender());
    foreach (QSslError error, errors) {
        qDebug() << "SSL error during hadshake: " << error.errorString();
    }
    socket->ignoreSslErrors(errors);
}

void Server::socketReady() {
    // Get calling socket
    QSocket *socket = qobject_cast<QSocket*>(sender());
    qDebug() << socket->peerAddress() << " connected.";

    // Start receiving data
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


    //int n = data.indexOf(commandDelimiter);
    QByteArray response;
    response.append((char)REGISTER);

    /*if (n < 0) {
        socket->write(response.append('\0').append("Wrong format of data."));
        return false;
    }*/

    //QByteArray name = data.left(n);
    //QByteArray key = data.mid(n+1);
    User *user = new User(this, data, socket->peerCertificate().publicKey(), socket);
    qDebug() << user->getPubKey().toPem().size();
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
    u = manager.getUserByNameAndKey(userName, socket->peerCertificate().publicKey().toPem());
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

    QByteArray byteID;
    byteID.setNum(u->getID());
    response.append('\1')
            .append(byteID);
    //response.append("Logged as \"" + u->getUsername().toLocal8Bit() + "\"\nYour ID: ").append('0' + u->getID());
    socket->write(response);

    return true;
}

bool Server::logout(QByteArray userName, QSocket *socket) {
    foreach(User* u, onlineUsers) {
        if (u->getUsername() == QString(userName) &&
            u->getPubKey() == socket->peerCertificate().publicKey()) {
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
        QByteArray userIdByte;
        userList.append(userIdByte.setNum(u->getID()))
                .append(commandDelimiter)
                .append(u->getUsername())
                .append(commandDelimiter)
                .append(u->getPubKey().toPem())
                .append(commandDelimiter)
                .append(u->getPubKey().algorithm())
                .append(commandDelimiter)
                .append(u->getSocket()->peerAddress().toString())
                .append(commandDelimiter);
    }

    return socket->write(userList) > 0;
}

void Server::generateKey(QByteArray data) {
    QList<QByteArray> tokens = data.split(commandDelimiter);
    long sourceUserID = tokens[0].toLong();
    long destUserID = tokens[1].toLong();
    QByteArray halfKey = tokens[2];
    foreach(User *u, onlineUsers) {
        if (u->getID() == destUserID) {
            QByteArray response;
            response.append((char)GENKEY)
                    .append(tokens[0])
                    .append(commandDelimiter)
                    .append(halfKey);
            u->getSocket()->write(response);
        }
    }
}

void Server::forward(QByteArray data, QSocket *socket) {
    //TODO
    //QByteArray response(data[0]);
    QList<QByteArray> tokens = data.mid(1).split(commandDelimiter);
    //long sourceUserID = tokens[0].toLong();
    long destUserID = tokens[1].toLong();
    //QByteArray data = tokens[2];

    foreach(User *u, onlineUsers) {
        if (u->getID() == tokens[1].toLong()) {
            /*response.append(tokens[0])
                    .append(commandDelimiter)
                    .append(tokens[1])
                    .append(commandDelimiter)
                    .append(tokens[2]);*/
            u->getSocket()->write(data);
        }
        else {
            socket->write(QByteArray().append((char)CONERR));
        }
    }
}

QList<User*>* Server::getUsers() {
    return &onlineUsers;
}

bool Server::isUserAlive(User *u) {
    return false;
}

void Server::refresh() {

}
