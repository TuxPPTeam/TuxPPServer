#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QObject>
#include <QtSql>
#include <QHostAddress>
#include <QtGlobal>
#include "user.h"

// DB connection constants
#ifdef Q_OS_WIN
static const QString serverName = "MAJO\\SQLEXPRESS";
#endif
static const QString dbName = "users";
static const QString tableName = "users";
#ifdef Q_OS_WIN
static const QString dsn =  QString("Driver={SQL Server Native Client 11.0};Server=%1;Database=%2;Trusted_Connection=Yes;")
                            .arg(serverName)
                            .arg(dbName);
#else
static const QString dsn =  QString("./" + dbName);
#endif


class DBmanager : public QObject
{
    Q_OBJECT
public:
    explicit DBmanager(QObject *parent = 0);

    bool establishConnection();
    void closeConnection();
    bool isConnected();

    bool insertUser(User*);
    bool updateUser(User*);
    bool deleteUser(User*);

    User* getUserByID(quint64 ID);
    QList<User*> getUsersByName(QString);
    User* getUserByNameAndKey(QString, QByteArray);
    QList<User*> listAllUsers();

signals:

public slots:

private:
    bool validateUser(User*);

    QSqlDatabase db;

};

#endif // DBMANAGER_H
