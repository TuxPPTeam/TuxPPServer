#ifndef USERTABLEMODEL_H
#define USERTABLEMODEL_H

#include <QAbstractTableModel>
#include <QHostAddress>
#include "user.h"

class UserTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit UserTableModel(QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                         int role = Qt::DisplayRole) const;

public slots:
    void setUsers(QList<User*> *);

private:
    QList<User*> *users;

};

#endif // USERTABLEMODEL_H
