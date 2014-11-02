#ifndef MYSERVER_H
#define MYSERVER_H

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


class MyServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit MyServer(QMainWindow*, QObject *parent = 0);

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

    bool registerUser(QByteArray, QByteArray, quint32 host);
    bool login(QByteArray, quint32 host, QSocket *socket);
    bool logout(QByteArray);
    bool getUserList(QSocket*);

#ifdef ENCRYPTED
    int setSsl(Socket*);
#endif
};

#endif // MYSERVER_H
