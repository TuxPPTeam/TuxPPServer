#include "dbmanager.h"

DBmanager::DBmanager(QObject *parent) :
    QObject(parent)
{
}

bool DBmanager::establishConnection() {
#ifdef Q_OS_WIN
    db = QSqlDatabase::addDatabase("QODBC");
#else
    db = QSqlDatabase::addDatabase("QSQLITE");

#endif
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
    if (!validateUser(user))
        return false;

    QSqlQuery query;
    query.prepare("INSERT INTO " + tableName + " (username, ip_address, pub_key) "
              #ifdef Q_OS_WIN
                  + "OUTPUT INSERTED.ID "
              #endif
                  "VALUES (?, ?, ?)");
    query.addBindValue(user->getUsername());
    if (user->getSocket() != NULL)
        query.addBindValue(user->getSocket()->peerAddress().toString());
    else
        query.addBindValue(QVariant::String);
    query.addBindValue(user->getPubKey().toPem());
    bool res = query.exec();

#ifdef Q_OS_WIN
    query.next();
    qDebug() << "Inserted user ID: " << query.value(0).toLongLong();
#endif

    return res;
}

bool DBmanager::updateUser(User *user) {
    if (!validateUser(user))
        return false;

    QSqlQuery query;
    query.prepare("UPDATE " + tableName + " SET ip_address = ? WHERE id = ?");

    if (user->getSocket() != NULL)
        query.addBindValue(user->getSocket()->peerAddress().toString());
    else
        query.addBindValue(QVariant::String);
    query.addBindValue(user->getID());
    return query.exec();
}

bool DBmanager::deleteUser(User *user) {
    QSqlQuery query;
    query.prepare("DELETE FROM " + tableName + " WHERE id = ?");
    query.addBindValue(user->getID());
    return query.exec();
}

User* DBmanager::getUserByID(quint64 ID) {
    QSqlQuery query;
    query.prepare("SELECT id, username, ip_address, pub_key FROM " + tableName + " WHERE id = ?");
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
    query.prepare("SELECT id, username, ip_address, pub_key FROM " + tableName + " WHERE username = ?");
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
    query.prepare("SELECT id, username, ip_address, pub_key FROM " + tableName + " WHERE username = ? AND pub_key = ?");
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
    query.prepare("SELECT id, username, ip_address, pub_key FROM " + tableName);
    query.exec();

    while (query.next()) {
        users.append(new User(0,
                              query.value(0).toLongLong(),
                              query.value(1).toString(),
                              query.value(3).toByteArray()));
    }
    return users;
}

bool DBmanager::validateUser(User *u) {
    return true;
}
