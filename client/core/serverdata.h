#ifndef SERVERDATA_H
#define SERVERDATA_H

#include <QObject>
#include <QString>
#include <QList>
#include <QVariant>

class ChannelData : public QObject {
    Q_OBJECT
    Q_PROPERTY(int id MEMBER m_id CONSTANT)
    Q_PROPERTY(QString name MEMBER m_name CONSTANT)
    Q_PROPERTY(QString type MEMBER m_type CONSTANT)
public:
    ChannelData(int id, const QString &name, const QString &type, QObject* parent = nullptr)
        : QObject(parent), m_id(id), m_name(name), m_type(type) {}

    int id() const { return m_id; }
    QString name() const { return m_name; }
    QString type() const { return m_type; }
private:
    int m_id;
    QString m_name;
    QString m_type;
};

class ServerData : public QObject {
    Q_OBJECT
    Q_PROPERTY(int id READ id CONSTANT)
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString avatarUrl READ avatarUrl CONSTANT)
    Q_PROPERTY(QString role READ role CONSTANT)
    Q_PROPERTY(QList<QObject*> channels READ channels CONSTANT)
public:
    ServerData(int id, const QString &name, const QString &avatarUrl, const QString &role,
               const QList<QObject*> &channels, QObject* parent = nullptr)
        : QObject(parent), m_id(id), m_name(name), m_avatarUrl(avatarUrl), m_role(role), m_channels(channels) {}

    int id() const { return m_id; }
    QString name() const { return m_name; }
    QString avatarUrl() const { return m_avatarUrl; }
    QString role() const { return m_role; }
    QList<QObject*> channels() const { return m_channels; }
private:
    int m_id;
    QString m_name;
    QString m_avatarUrl;
    QString m_role;
    QList<QObject*> m_channels;
};


#endif // SERVERDATA_H
