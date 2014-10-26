#ifndef WORKER_H
#define WORKER_H

#include <QRunnable>
#include <QSslSocket>
#include <QEventLoop>

class Worker : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit Worker(qintptr ID, QObject *parent = 0);
    void run();

signals:
    void error(QTcpSocket::SocketError socketerror);
    void dataRecieved(QByteArray data);
    void remove();
    void finished();

public slots:
    void readyRead();
    void disconnected();

private:
    QTcpSocket *socket;
    qintptr socketDescriptor;
    QObject* parent;
    QEventLoop* loop;
};

#endif // WORKER_H
