#include "user.h"

User::User(QObject *parent) :
    QObject(parent)
{

}

User::User(QObject *parent, QString userName, qint32 host, QByteArray pubKey) :
    QObject(parent)
{
    ID = -1;
    setUsername(userName);
    setHost(host);
    setPubKey(pubKey);
}

User::User(QObject *parent, qint64 ID, QString userName, qint32 host, QByteArray pubKey) :
    QObject(parent)
{
    setID(ID);
    setUsername(userName);
    setHost(host);
    setPubKey(pubKey);
}

bool User::setID(const qint64 newID) {
    if (ID != -1) {
        ID = newID;
        return true;
    }
    return false;
}

bool User::setUsername(const QString newUsername) {
    if ((unsigned int)newUsername.length() > MAX_USERNAME_LENGTH) {
        return false;
    }
    else {
        username = newUsername;
        return true;
    }
}

void User::setHost(const qint32 newHost) {
        host = newHost;
}

void User::setPubKey(const QByteArray newPubKey) {
        pubKey = newPubKey;
}

qint64 User::getID() const {
    return ID;
}

QString User::getUsername() const {
    return username;
}

qint32 User::getHost() const {
    return host;
}

QByteArray User::getPubKey() const {
    return pubKey;
}

bool User::operator ==(const User &user) const {
    return  (user.getID() == ID) &&
            (user.getUsername() == username) &&
            (user.getPubKey() == pubKey);
}
