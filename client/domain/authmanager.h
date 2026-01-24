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
    void tryReg(const QString& login, const QString& password, const QString& email);
    void tryAutoTokenLogin(const QString &access_token);

public slots:
    void onEndpointChanged(const QString &service, const QUrl &newUrl);

signals:
    void authSucceeded();
    void authFailed(const QString& error);

private slots:
    void onConnected();
    void onMessageReceived(const QString &text);

private:
    WebSocketClient *m_socket;
    QString m_pendingRequest;
    QString username;
};

#endif // AUTHMANAGER_H
