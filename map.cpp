#include "map.h"

Map::Map(QObject *parent) :
    QObject(parent)
{
}

Map::~Map() {
    removeAll();
}

void Map::add(User *u) {
    users[u->getID()] = u;
    emit changed(users.values());
}

int Map::remove(User *u) {
    int res = users.remove(u->getID());
    u->deleteLater();
    emit changed(users.values());
    return res;
}

void Map::removeAll() {
    foreach (User *u, users) {
        u->deleteLater();
    }
    users.clear();
}

bool Map::contains(quint64 ID) {
    return users.contains(ID);
}

User* Map::get(quint64 ID) {
    if (users.contains(ID))
        return users.value(ID);
    return NULL;
}

QList<User*> Map::getAll() {
    return users.values();
}
