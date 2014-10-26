#include "mythread.h"
#include "myserver.h"

MyThread::MyThread(qintptr ID, QObject *parent) :
    QThread(parent),
    socketDescriptor(ID),
    parent(parent)
{
    //this->socketDescriptor = ID;
}

void MyThread::run()
{
    // thread starts here
    qDebug() << " Thread started";
    socket = new QTcpSocket();

    // set the ID
    if(!socket->setSocketDescriptor(this->socketDescriptor))
    {
        // something's wrong, we just emit a signal
        emit error(socket->error());
        return;
    }

    // connect socket and signal
    // note - Qt::DirectConnection is used because it's multithreaded
    //        This makes the slot to be invoked immediately, when the signal is emitted.

    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::DirectConnection);
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));

    // We'll have multiple clients, we want to know which is which
    qDebug() << socketDescriptor << " Client connected";

    exec();
}

void MyThread::readyRead()
{
    // get the information
    QByteArray Data = socket->readAll();

    // will write on server side window
    qDebug() << socketDescriptor << " Data in: " << Data;

    socket->write(Data);

    emit dataRecieved(Data);
}

void MyThread::disconnected()
{
    if (QObject::sender() != parent) {
        qDebug() << socketDescriptor << " Disconnected";
        socket->deleteLater();
        emit remove();
        exit(0);
    }
}
