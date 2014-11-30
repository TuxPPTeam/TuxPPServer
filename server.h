#ifndef SERVER_H
#define SERVER_H

#define PORTNO 1234
#define ENCRYPTED

#include <QTcpServer>
#include <QList>
#include <QMap>
#include <QPlainTextEdit>
#include "dbmanager.h"
//#include "mainwindow.h"
//#include "map.h"

static const char commandDelimiter = '\31';

class Server : public QTcpServer
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = 0);
    QList<User*>* getUsers();

signals:
    void dataReady(QByteArray);
    void usersChanged(QList<User*>*);

public slots:
    void start();
    void shutdown();

private slots:
    void disconnected();
    void readyRead();
    void refresh();

#ifdef ENCRYPTED
    void socketReady();
    void errorOccured(QList<QSslError>);
#else
    void errorOccured(QAbstractSocket::SocketError);
#endif

protected:
    void incomingConnection(qintptr);

private:
    QList<QSocket*> sockets;
    QList<User*> onlineUsers;
    DBmanager manager;

    void echo(QByteArray, QSocket*);
    bool registerUser(QByteArray, QSocket*);
    bool login(QByteArray, QSslSocket *socket);
    bool logout(QByteArray, QSocket *socket);
    bool getUserList(QSocket*);
    bool isUserAlive(User*);
    void generateKey(QByteArray);

    enum Command { ECHO, LOGIN, LOGOUT, REGISTER, GETUSERS, GENKEY };

#ifdef ENCRYPTED
    bool setSsl(QSocket*);
#endif
};

#endif // SERVER_H
