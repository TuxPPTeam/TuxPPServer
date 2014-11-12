#ifndef MAP_H
#define MAP_H

#include <QObject>
#include "user.h"

class Map : public QObject
{
    Q_OBJECT
public:
    explicit Map(QObject *parent = 0);
    ~Map();

signals:
    void changed(QList<User*>);

public slots:
    void add(User*);
    int remove(User*);
    void removeAll();
    bool contains(quint64);

    User* get(quint64);
    QList<User*> getAll();

private:
    QMap<quint64, User*> users;
};

#endif // MAP_H
