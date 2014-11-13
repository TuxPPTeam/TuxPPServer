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
    query.prepare("INSERT INTO " + dbName + " (username, ip_address, pub_key) OUTPUT INSERTED.ID "
                  "VALUES (?, ?, ?)");
    //query.addBindValue(dbName);
    query.addBindValue(user->getUsername());
    if (user->getSocket() != NULL)
        query.addBindValue(user->getSocket()->peerAddress().toString());
    else
        query.addBindValue(QVariant::String);
    query.addBindValue(user->getPubKey());
    bool res = query.exec();
    query.next();
    qDebug() << "Inserted user ID: " << query.value(0).toLongLong();

    return res;
}

bool DBmanager::updateUser(User *user) {
    QSqlQuery query;
    query.prepare("UPDATE " + dbName + " SET username = ?, ip_address = ?,pub_key = ? WHERE id = ?");
    //query.addBindValue(dbName);
    query.addBindValue(user->getUsername());
    if (user->getSocket() != NULL)
        query.addBindValue(user->getSocket()->peerAddress().toString());
    else
        query.addBindValue(QVariant::String);
    query.addBindValue(user->getPubKey());
    query.addBindValue(user->getID());
    return query.exec();
}

bool DBmanager::deleteUser(User *user) {
    QSqlQuery query;
    query.prepare("DELETE FROM " + dbName + " WHERE id = ?");
    query.addBindValue(user->getID());
    return query.exec();
}

User* DBmanager::getUserByID(quint64 ID) {
    QSqlQuery query;
    query.prepare("SELECT id, username, ip_address, pub_key FROM " + dbName + " WHERE id = ?");
    query.addBindValue(ID);
    query.exec();
    if (!query.next()) {
        return NULL;
    }
    return new User(0,
                    query.value(0).toLongLong(),
                    query.value(1).toString(),
                    query.value(3).toByteArray());
}

QList<User*> DBmanager::getUsersByName(QString name) {
    QList<User*> users;
    QSqlQuery query;
    query.prepare("SELECT id, username, ip_address, pub_key FROM " + dbName + " WHERE username = ?");
    query.addBindValue(name);
    query.exec();

    while (query.next()) {
        users.append(new User(0,
                              query.value(0).toLongLong(),
                              query.value(1).toString(),
                              query.value(3).toByteArray()));
    }
    return users;
}

User* DBmanager::getUserByNameAndKey(QString name, QByteArray key) {
    QSqlQuery query;
    query.prepare("SELECT id, username, ip_address, pub_key FROM " + dbName + " WHERE username = ? AND pub_key = ?");
    query.addBindValue(name);
    query.addBindValue(key);
    query.exec();

    if (!query.next())
        return NULL;

    return new User(0,
                    query.value(0).toLongLong(),
                    query.value(1).toString(),
                    query.value(3).toByteArray());
}

QList<User*> DBmanager::listAllUsers() {
    QList<User*> users;
    QSqlQuery query;
    query.prepare("SELECT id, username, ip_address, pub_key FROM " + dbName);
    query.exec();

    while (query.next()) {
        users.append(new User(0,
                              query.value(0).toLongLong(),
                              query.value(1).toString(),
                              query.value(3).toByteArray()));
    }
    return users;
}
