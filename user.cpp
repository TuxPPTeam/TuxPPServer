#include "user.h"

User::User(QObject *parent) :
    QObject(parent)
{

}

User::User(QObject *parent, QString username, QByteArray host, QByteArray pubKey) :
    QObject(parent)
{
    setUsername(username);
    setHost(host);
    setPubKey(pubKey);
}

int User::setUsername(const QString newUsername) {
    if ((unsigned int)newUsername.length() > MAX_USERNAME_LENGTH) {
        return -1;
    }
    else {
        bool set;
        if (username.isEmpty() || username.isNull()) {
            set = false;
        }
        if (username != newUsername) {
            username = newUsername;
        }
        if (set) {
            emit propertyChanged();
        }
        return 0;
    }
}

void User::setHost(const QByteArray newHost) {
    bool set;
    if (host.isEmpty() || host.isNull()) {
        set = false;
    }
    if (host != newHost) {
        host = newHost;
    }
    if (set) {
        emit propertyChanged();
    }
}

void User::setPubKey(const QByteArray newPubKey) {
    bool set;
    if (pubKey.isEmpty() || host.isNull()) {
        set = false;
    }
    if (pubKey != newPubKey) {
        pubKey = newPubKey;
    }
    if (set) {
        emit propertyChanged();
    }
}

QString User::getUsername() {
    return username;
}

QByteArray User::getHost() {
    return host;
}

QByteArray User::getPubKey() {
    return pubKey;
}
