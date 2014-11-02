#ifndef USER_H
#define USER_H

#include <QObject>

class User : public QObject
{
    Q_OBJECT
public:
    explicit User(QObject *parent = 0);
    explicit User(QObject *parent = 0, QString userName = "", qint32 host = 0, QByteArray pubKey = "");
    explicit User(QObject *parent = 0, qint64 = 0, QString userName = "", qint32 host = 0, QByteArray pubKey = "");
    static const unsigned int MAX_USERNAME_LENGTH = 32;

    qint64 getID() const;
    QString getUsername() const;
    qint32 getHost() const;
    QByteArray getPubKey() const;

    bool operator==(const User &) const;

signals:
    void propertyChanged();

public slots:
    bool setID(const qint64);
    bool setUsername(const QString);
    void setHost(const qint32);
    void setPubKey(const QByteArray);

private:
    qint64 ID;
    QString username;
    qint32 host;
    QByteArray pubKey;
};

#endif // USER_H
