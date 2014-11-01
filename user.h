#ifndef USER_H
#define USER_H

#include <QObject>

class User : public QObject
{
    Q_OBJECT
public:
    explicit User(QObject *parent = 0);
    static const unsigned int MAX_USERNAME_LENGTH = 32;

signals:
    void propertyChanged();

public slots:
    int setUsername(const QString newUsername);
    void setHost(const QString newHost);
    void setPubKey(const QString newPubKey);

private:
    QString username;
    QString host;
    QString pubKey;
};

#endif // USER_H
