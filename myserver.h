#ifndef MYSERVER_H
#define MYSERVER_H

#include <QTcpServer>
#include <QSslSocket>
#include <QMainWindow>
#include <QList>

#ifdef ENCRYPTED
    typedef QSslSocket Socket;
#else
    typedef QTcpSocket Socket;
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
    QList<Socket*> sockets;

#ifdef ENCRYPTED
    int setSsl(Socket*);
#endif
};

#endif // MYSERVER_H
