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

bool DBmanager::isConnected() {
    return db.isOpen();
}

bool DBmanager::insertUser(User *user) {
    QSqlQuery query;
    query.prepare("INSERT INTO users (username, ip_address, pub_key) OUTPUT INSERTED.ID "
                  "VALUES (?, ?, ?)");
    query.addBindValue(user->getUsername());
    query.addBindValue(QVariant::Int);
    query.addBindValue(user->getPubKey());
    query.exec();
    query.next();
    qDebug() << "Inserted user ID: " << query.value(0).toLongLong();
    return true;
}

User* DBmanager::getUserByID(quint64 ID) {
    QSqlQuery query;
    query.prepare("SELECT id, username, ip_address, pub_key FROM users WHERE id == ?");
    query.addBindValue(ID);
    query.exec();
    if (!query.next()) {
        return NULL;
    }
    return new User(0,
                    query.value(0).toLongLong(),
                    query.value(1).toString(),
                    query.value(2).toInt(),
                    query.value(3).toByteArray());
}

QList<User*> DBmanager::getUsersByName(QString name) {
    QList<User*> users;
    QSqlQuery query;
    query.prepare("SELECT id, username, ip_address, pub_key FROM users WHERE username = ?");
    query.addBindValue(name);
    query.exec();

    while (query.next()) {
        users.append(new User(0,
                              query.value(0).toLongLong(),
                              query.value(1).toString(),
                              query.value(2).toInt(),
                              query.value(3).toByteArray()));
    }
    return users;
}

QList<User*> DBmanager::listAllUsers() {
    QList<User*> users;
    QSqlQuery query;
    query.prepare("SELECT id, username, ip_address, pub_key FROM users");
    query.exec();

    while (query.next()) {
        users.append(new User(0,
                              query.value(0).toLongLong(),
                              query.value(1).toString(),
                              query.value(2).toInt(),
                              query.value(3).toByteArray()));
    }
    return users;
}
