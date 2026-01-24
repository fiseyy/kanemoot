#ifndef AUTHMANAGER_H
#define AUTHMANAGER_H

#include <QObject>
#include <QString>
#include <QJsonObject>

#include "network/websocketclient.h"

/*
 * Тип выполняемого auth-запроса
 * Нужен, чтобы:
 *  - не хранить "сырой JSON"
 *  - чётко понимать, какой ответ мы ждём
 */
enum class AuthRequestType {
    None,
    Login,
    Register,
    AutoToken
};

/*
 * Описание одного активного auth-запроса
 * В системе одновременно может быть ТОЛЬКО один
 */
struct PendingAuthRequest {
    AuthRequestType type = AuthRequestType::None;
    QJsonObject payload;
};

class AuthManager : public QObject {
    Q_OBJECT
public:
    explicit AuthManager(QObject* parent = nullptr);

    // Публичный API
    void tryAuth(const QString& login, const QString& password);
    void tryReg(const QString& login, const QString& password, const QString& email);
    void tryAutoTokenLogin(const QString& accessToken);

signals:
    // Контракт с UI / AppController
    void authSucceeded();
    void authFailed(const QString& errorMessage);

private slots:
    // WebSocketClient events
    void onSocketConnected();
    void onSocketDisconnected(DisconnectReason reason);
    void onSocketError(const QString& error);
    void onMessageReceived(const QString& message);

    // ApiEndpoints
    void onEndpointChanged(const QString& service, const QUrl& newUrl);

private:
    // === helpers ===
    void sendPendingRequest();
    void clearPendingRequest();
    void failAuth(const QString& errorMessage, bool markAuthFailed = true);

    // === data ===
    WebSocketClient* m_socket = nullptr;

    PendingAuthRequest m_pendingRequest;

    // используется для сохранения username при login/reg
    QString m_username;
};

#endif // AUTHMANAGER_H
