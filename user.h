#ifndef USER_H
#define USER_H

#include <QObject>
#include <QSslSocket>

#ifdef ENCRYPTED
    typedef QSslSocket QSocket;
#else
    typedef QTcpSocket QSocket;
#endif

class User : public QObject
{
    Q_OBJECT
public:
    explicit User(QObject *parent = 0);
    explicit User(QObject *parent = 0, QString userName = "", /*qint32 host = 0,*/ QByteArray pubKey = "", QSocket *socket = NULL);
    explicit User(QObject *parent = 0, qint64 = 0, QString userName = "", /*qint32 host = 0,*/ QByteArray pubKey = "", QSocket *socket = NULL);
    static const unsigned int MAX_USERNAME_LENGTH = 32;

    qint64 getID() const;
    QString getUsername() const;
    //qint32 getHost() const;
    QByteArray getPubKey() const;
    QSocket* getSocket() const;

    bool operator==(const User &) const;

signals:
    void propertyChanged();

public slots:
    bool setID(const qint64);
    bool setUsername(const QString);
    //void setHost(const qint32);
    void setPubKey(const QByteArray);
    void setSocket(QSocket *);

private:
    qint64 ID;
    QString username;
    //qint32 host;
    QByteArray pubKey;
    QSocket* socket;
};

#endif // USER_H
