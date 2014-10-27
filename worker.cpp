#include "worker.h"
#include "myserver.h"

Worker::Worker(qintptr ID, QObject *parent) :
    //QRunnable(),
    QObject(parent),
    socketDescriptor(ID),
    parent(parent),
    loop(new QEventLoop(this))
{

}

Worker::~Worker()
{
    delete loop;
    delete socket;
}

void Worker::run()
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

    loop->exec();
    // We'll have multiple clients, we want to know which is which
    qDebug() << socketDescriptor << " Client connected";

    qDebug() << "Read succesfull? " << socket->waitForReadyRead();
}

void Worker::readyRead()
{
    // get the information
    QByteArray Data = socket->readAll();

    // will write on server side window
    qDebug() << socketDescriptor << " Data in: " << Data;

    socket->write(Data);

    emit dataRecieved(Data);
}

void Worker::disconnected()
{
    if (QObject::sender() != parent) {
        qDebug() << socketDescriptor << " Disconnected";
        socket->deleteLater();
        //loop->exit();
        emit remove();
        emit finished();
        exit(0);
    }
}
