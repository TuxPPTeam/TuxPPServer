#ifndef SERVER_H
#define SERVER_H

#define PORTNO 1234
//#define ENCRYPTED

#include <QTcpServer>
#include <QMainWindow>
#include <QList>
#include <QMap>
#include "dbmanager.h"
#include "map.h"

static const char commandDelimiter = '\n';

class Server : public QTcpServer
{
    Q_OBJECT
public:
    explicit Server(QMainWindow*, QObject *parent = 0);

signals:
    void dataReady(QByteArray);

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
    QMainWindow *window;
    QList<QSocket*> sockets;
    //QMap<quint64, User*> onlineUsers;
    Map onlineUsers;
    DBmanager manager;

    void echo(QByteArray, QSocket*);
    bool registerUser(QByteArray, QSocket*);
    bool login(QByteArray, QSocket*);
    bool logout(QByteArray);
    bool getUserList(QSocket*);
    bool isUserAlive(User*);

    enum Command { ECHO, LOGIN, LOGOUT, REGISTER, GETUSERS};

#ifdef ENCRYPTED
    int setSsl(Socket*);
#endif
};

#endif // SERVER_H
