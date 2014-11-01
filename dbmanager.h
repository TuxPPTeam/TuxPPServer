#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QObject>
#include <QtSql>
#include "user.h"

// DB connection constants
static const QString serverName = "MAJO\\SQLEXPRESS";
static const QString dbName = "users";
static const QString dsn = //QString("Driver={SQL Server Native Client 11.0};Server=%1;Database=%2;Uid=%3;Pwd=%4;")
                           QString("Driver={SQL Server Native Client 11.0};Server=%1;Database=%2;Trusted_Connection=Yes;")
                    .arg(serverName)
                    .arg(dbName);


class DBmanager : public QObject
{
    Q_OBJECT
public:
    explicit DBmanager(QObject *parent = 0);

    bool establishConnection();
    void closeConnection();
    bool insertUser(User*);
    int updateUser(User*);
    int deleteUser(User*);
    int retrieveUserByID(int ID);
    int listAllUsers();

signals:

public slots:

private:
    QSqlDatabase db;

};

#endif // DBMANAGER_H
