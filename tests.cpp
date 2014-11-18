#include "tests.h"

void Tests::testAddUser()
{
    DBmanager* db = new DBmanager(this);
    db->establishConnection();
    QString name = "marek";
    QString key = "keyvnpreisaovpnudsio";
    QSocket* socket = new QSocket();
    /*User* user = new User(this/*, name, key, socket);
    db->insertUser(user);
    User* dbUser = db->getUsersByName(name);*/
    QVERIFY(false/*key == dbUser->getPubKey()*/);
}
