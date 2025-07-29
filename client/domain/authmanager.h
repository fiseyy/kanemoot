#ifndef AUTHMANAGER_H
#define AUTHMANAGER_H

#include <QObject>
#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include "network/websocketclient.h"

class AuthManager : public QObject {
    Q_OBJECT
public:
    explicit AuthManager(QObject *parent = nullptr);

    void tryAuth(const QString& login, const QString& password);

signals:
    void authSucceeded();
    void authFailed(const QString& error);

private slots:
    void onConnected();
    void onMessageReceived(const QString &text);

private:
    WebSocketClient *m_socket;
    QString m_pendingRequest;
};

#endif // AUTHMANAGER_H
