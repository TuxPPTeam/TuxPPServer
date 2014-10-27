#include "myserver.h"
#include "worker.h"

#define PORTNO 1234

MyServer::MyServer(QMainWindow *w, QObject *parent) :
    QTcpServer(parent),
    window(w)
{

}

void MyServer::start()
{
    if(!this->listen(QHostAddress::Any, PORTNO))
    {
        qDebug() << "Could not start server";
    }
    else
    {
        qDebug() << "Listening to port " << PORTNO << "...";
    }
}

void MyServer::shutdown()
{      
    this->close();

    qDebug() << "Server is shut down";
}

// This function is called by QTcpServer when a new connection is available.
void MyServer::incomingConnection(qintptr socketDescriptor)
{
    // We have a new connection
    qDebug() << socketDescriptor << " Connecting...";

    // Every new connection will be run in a newly created thread
    //MyThread *thread = new MyThread(socketDescriptor, this);

    Worker *worker = new Worker(socketDescriptor, this);

    // connect signal/slot
    // once a thread is not needed, it will be beleted later
    connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
    connect(worker, SIGNAL(dataRecieved(QByteArray)), window, SLOT(writeData(QByteArray)));
    connect(worker, SIGNAL(remove()), this, SLOT(remove()));

    QThreadPool::globalInstance()->start(worker);
}


