#include "myserver.h"

MyServer::MyServer(QMainWindow *w, QObject *parent) :
    QTcpServer(parent),
    window(w)
{
    manager.establishConnection();
    User u(this, "Zdeno", QString("255.255.255.255").toLocal8Bit(), QString("aaa").toLocal8Bit());
    manager.insertUser(&u);
}

void MyServer::start() {
    if(!this->listen(QHostAddress::Any, PORTNO)) {
        qDebug() << "Could not start server";
    }
    else {
        qDebug() << "Listening to port " << PORTNO << "...";
    }
}

void MyServer::shutdown() {
    // Stop listening
    this->close();

    // Close each connection and delete sockets
    foreach (Socket *socket, sockets) {
        socket->close();
        socket->deleteLater();
    }

    manager.closeConnection();
    qDebug() << "Server is shut down";
}

void MyServer::disconnected() {
    // Get calling socket
    Socket *socket = qobject_cast<Socket*>(sender());
    qDebug() << socket->peerAddress().toString() << " disconnected.";
    // Remove it from active sockets and delete
    sockets.removeAll(socket);
    socket->deleteLater();
}

// This function is called by QTcpServer when a new connection is available.
void MyServer::incomingConnection(qintptr socketDescriptor) {
    // We have a new connection
    qDebug() << socketDescriptor << " Connecting...";

    Socket *socket = new Socket(this);
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

void MyServer::socketReady() {
    // Get calling socket
    Socket *socket = qobject_cast<Socket*>(sender());
    qDebug() << socket->peerAddress().toString() << " connected.";

    // Start recieving data
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
}

void MyServer::errorOccured(QList<QSslError> errors) {
    foreach (QSslError error, errors) {
        qDebug() << "SSL error during hadshake: " << error.errorString();
    }
}

void MyServer::readyRead() {
    // Get calling socket
    Socket *socket = qobject_cast<Socket*>(sender());
#ifdef ENCRYPTED
    if (socket->isEncrypted()) {
#else
    if (socket->isReadable()) {
#endif
        // Read all data
        QByteArray data = socket->readAll();

        qDebug() << data;
        emit dataReady(data);
        socket->write("Reading done.");
    }
}

#ifdef ENCRYPTED
int MyServer::setSsl(Socket *socket) {
    // Set all needed SSL information, such as ciphers, certificates, mode, ...

    return 0;
}
#endif
