#include "usertablemodel.h"

UserTableModel::UserTableModel(QObject *parent) :
    QAbstractTableModel(parent),
    users(new QList<User*>())
{
}

int UserTableModel::rowCount(const QModelIndex &) const {
    return users->count();
}

int UserTableModel::columnCount(const QModelIndex &) const {
    return 4;
}

QVariant UserTableModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) {
         return QVariant();
    }

     if (index.row() >= users->count() || index.column() >= 4) {
         return QVariant();
     }

     if (!users->at(index.row())->getSocket()->isOpen()) {
         return QVariant();
     }

     if (role == Qt::DisplayRole) {
         switch (index.column()) {
            case 0:     return users->at(index.row())->getID();
            case 1:     return users->at(index.row())->getUsername();
            case 2:     return users->at(index.row())->getPubKey().toPem();
            case 3:     return users->at(index.row())->getSocket()->peerAddress().toString();
            default:    return QVariant();
         }
     }
     else {
         return QVariant();
     }
}

QVariant UserTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    if (orientation == Qt::Horizontal) {
        switch (section) {
            case 0:     return QString("ID");
            case 1:     return QString("Username");
            case 2:     return QString("Public key");
            case 3:     return QString("IP address");
            default:    return QVariant();
        }
    }
    else {
        return QVariant();
    }
}

void UserTableModel::setUsers(QList<User*> *newUsers) {
    beginResetModel();
    users = newUsers;
    endResetModel();
}

/*void UserTableModel::addUser(User *user) {
    users.append(user);
}

void UserTableModel::removeUser(User *user) {
    users.removeAll(user);
}*/
