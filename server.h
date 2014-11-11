#ifndef SERVER_H
#define SERVER_H

#define PORTNO 1234
//#define ENCRYPTED

#include <QTcpServer>
#include <QSslSocket>
#include <QMainWindow>
#include <QList>
#include <QMap>
#include "dbmanager.h"

#ifdef ENCRYPTED
    typedef QSslSocket QSocket;
#else
    typedef QTcpSocket QSocket;
#endif




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
    void socketReady();
    void errorOccured(QList<QSslError>);
    void disconnected();
    void readyRead();

protected:
    void incomingConnection(qintptr);

private:
    QMainWindow *window;
    QList<QSocket*> sockets;
    QMap<quint64, User*> onlineUsers;
    DBmanager manager;

    void echo(QByteArray, QSocket *socket);
    bool registerUser(QByteArray, quint32 host);
    bool login(QByteArray, quint32 host, QSocket *socket);
    bool logout(QByteArray);
    bool getUserList(QSocket*);

    enum command { ECHO, LOGIN, LOGOUT, REGISTER, GETUSERS};

#ifdef ENCRYPTED
    int setSsl(Socket*);
#endif
};

#endif // SERVER_H
