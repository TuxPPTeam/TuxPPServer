#ifndef USER_H
#define USER_H

#include <QObject>

class User : public QObject
{
    Q_OBJECT
public:
    explicit User(QObject *parent = 0);
    explicit User(QObject *parent = 0, QString username = "", QByteArray host = "", QByteArray pubKey = "");
    static const unsigned int MAX_USERNAME_LENGTH = 32;

    QString getUsername();
    QByteArray getHost();
    QByteArray getPubKey();

signals:
    void propertyChanged();

public slots:
    int setUsername(const QString newUsername);
    void setHost(const QByteArray newHost);
    void setPubKey(const QByteArray newPubKey);

private:
    unsigned int ID;
    QString username;
    QByteArray host;
    QByteArray pubKey;
};

#endif // USER_H
