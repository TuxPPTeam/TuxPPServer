#include "dbmanager.h"

DBmanager::DBmanager(QObject *parent) :
    QObject(parent)
{
}

bool DBmanager::establishConnection() {
    db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName(dsn);
    if (db.open()) {
        qDebug() << "DB connection opened";
        qDebug() << "Has LastInsertID feature ? " << db.driver()->hasFeature(QSqlDriver::LastInsertId);
        return true;
    }
    else {
        qDebug() << "Error: " << db.lastError().text();
        return false;
    }
}

void DBmanager::closeConnection() {
    qDebug() << "DB connection closed";
    db.close();
}

bool DBmanager::insertUser(User *user) {
    QSqlQuery query;
    query.prepare("INSERT INTO users (username, ip_address, pub_key) OUTPUT INSERTED.ID "
                  "VALUES (?, ?, ?)");
    query.addBindValue(user->getUsername());
    query.addBindValue(user->getHost());
    query.addBindValue(user->getPubKey());
    query.exec();
    query.next();
    qDebug() << "Inserted user ID: " << query.value(0).toLongLong();
    return true;
}
