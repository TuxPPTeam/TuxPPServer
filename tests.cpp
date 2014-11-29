#include "tests.h"

void Tests::testAddUser()
{
    DBmanager* db = new DBmanager(this);
    db->establishConnection();
    QString name = "marek";
    QByteArray key = "keyvnpreisaovpnudsio";
    User* user = new User(this, name, key);
    db->insertUser(user);
    QList<User*> dbUsers = db->getUsersByName(name);
    //QVERIFY(dbUsers.length() == 1);
    //QVERIFY(key == dbUsers.at(0)->getPubKey());
}
