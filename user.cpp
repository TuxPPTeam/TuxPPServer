#include "user.h"

User::User(QObject *parent) :
    QObject(parent)
{
}

int User::setUsername(const QString newUsername)
{
    if (newUsername.length() > 32)
    {
        return -1;
    }
    else
    {
        bool set;
        if (username.isEmpty() || username.isNull())
            set = false;
        if (username != newUsername) {
            username = newUsername;
        }
        if (set)
            emit propertyChanged();
        return 0;
    }
}

void User::setHost(const QString newHost)
{
    bool set;
    if (host.isEmpty() || host.isNull())
        set = false;
    if (host != newHost) {
        host = newHost;
    }
    if (set)
        emit propertyChanged();
}

void User::setPubKey(const QString newPubKey)
{
    bool set;
    if (pubKey.isEmpty() || host.isNull())
        set = false;
    if (pubKey != newPubKey) {
        pubKey = newPubKey;
    }
    if (set)
        emit propertyChanged();
}
