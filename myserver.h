#ifndef MYSERVER_H
#define MYSERVER_H

#include <QTcpServer>
#include <QThreadPool>
#include <QMainWindow>
#include "mythread.h"
#include "worker.h"

class MyServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit MyServer(QMainWindow *w, QObject *parent = 0);

signals:

public slots:
    void start();
    void shutdown();

private slots:
    void remove();

protected:
    void incomingConnection(qintptr socketDescriptor);

private:
    QMainWindow *window;
    QList<Worker*> workers;
    QThreadPool *pool;
};

#endif // MYSERVER_H
