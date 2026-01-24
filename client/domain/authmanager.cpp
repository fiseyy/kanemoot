#include "domain/authmanager.h"
#include "network/websocketclient.h"
#include "core/apiendpoints.h"
#include "utils/logging.h"
#include "core/errorcode.h"
#include "core/securestorage.h"

AuthManager::AuthManager(QObject *parent)
    : QObject(parent)
{
    m_socket = new WebSocketClient(this);

    connect(m_socket, &WebSocketClient::connected, this, &AuthManager::onSocketConnected);
    connect(m_socket, &WebSocketClient::disconnected, this, &AuthManager::onSocketDisconnected);
    connect(m_socket, &WebSocketClient::errorOccurred, this, &AuthManager::onSocketError);
    connect(m_socket, &WebSocketClient::messageReceived, this, &AuthManager::onMessageReceived);

    connect(&ApiEndpoints::instance(), &ApiEndpoints::endpointChanged,
            this, &AuthManager::onEndpointChanged);
}

// ------------------- Public API -------------------

void AuthManager::tryAuth(const QString &login, const QString &password)
{
    qDebug() << "AUTHMANAGER --> tryAuth --> start";
    if (!m_socket) return;

    if (m_socket->state() == SocketState::Connecting) {
        LOG(Logging::Warning, ErrorCode::make(ErrorCode::Network, 0x03, ErrorCode::AuthManager), "Попытка Auth во время подключения");
        return;
    }

    m_username = login;
    m_pendingRequest.type = AuthRequestType::Login;
    m_pendingRequest.payload = {
        {"action", "login"},
        {"user", login},
        {"password", password}
    };

    sendPendingRequest();
    qDebug() << "AUTHMANAGER --> tryAuth --> finish";
}

void AuthManager::tryReg(const QString &login, const QString &password, const QString &email)
{
    if (!m_socket) return;

    if (m_socket->state() == SocketState::Connecting) {
        LOG(Logging::Warning, ErrorCode::make(ErrorCode::Network, 0x03, ErrorCode::AuthManager), "Попытка Registration во время подключения");
        return;
    }

    m_username = login;
    m_pendingRequest.type = AuthRequestType::Register;
    m_pendingRequest.payload = {
        {"action", "reg"},
        {"user", login},
        {"password", password},
        {"email", email}
    };

    sendPendingRequest();
}

void AuthManager::tryAutoTokenLogin(const QString &accessToken)
{
    if (!m_socket) return;

    if (m_socket->state() == SocketState::Connecting) {
        LOG(Logging::Warning, ErrorCode::make(ErrorCode::Network, 0x03, ErrorCode::AuthManager), "Попытка AutoTokenLogin во время подключения");
        return;
    }

    m_pendingRequest.type = AuthRequestType::AutoToken;
    m_pendingRequest.payload = {
        {"action", "auto_token_login"},
        {"token", accessToken}
    };

    sendPendingRequest();
}

// ------------------- Slots -------------------

void AuthManager::onSocketConnected()
{
    sendPendingRequest();
}

void AuthManager::onSocketDisconnected(DisconnectReason reason)
{
    Q_UNUSED(reason);
    // здесь пока ничего не делаем, обработка ошибок через errorOccurred
}

void AuthManager::onSocketError(const QString &error)
{
    if (error.contains("502")) {
        LOG(Logging::Warning, ErrorCode::make(ErrorCode::Network, 0x04, ErrorCode::AuthManager), error);
    } else if (error.contains("SSL") || error.contains("handshake")) {
        LOG(Logging::Warning, ErrorCode::make(ErrorCode::Network, 0x05, ErrorCode::AuthManager), error);
    } else {
        LOG(Logging::Warning, ErrorCode::make(ErrorCode::Network, 0x01, ErrorCode::AuthManager), error);
    }
    failAuth("");
}

void AuthManager::onEndpointChanged(const QString &service, const QUrl &newUrl)
{
    if (service != "auth" || !m_socket) return;

    LOG(Logging::Debug, ErrorCode::make(ErrorCode::Network, 0x00, ErrorCode::ChatManager),
        "Endpoint chat изменён: " + newUrl.toString().toUtf8());

    m_socket->setLastUrl(newUrl);
    m_socket->forceReconnect();
}

void AuthManager::onMessageReceived(const QString &message)
{
    if (message.isEmpty()) {
        failAuth("Пустой ответ сервера");
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
    if (!doc.isObject()) {
        LOG(Logging::Debug, ErrorCode::make(ErrorCode::Network, 0x02, ErrorCode::AuthManager),
            "Неверный формат ответа сервера: " + message.toUtf8());
        failAuth("Неверный формат ответа сервера");
        return;
    }

    QJsonObject obj = doc.object();
    bool success = false;
    QJsonValue successVal = obj.value("success");
    if (successVal.isBool()) {
        success = successVal.toBool();
    } else if (successVal.isString()) {
        success = successVal.toString().toLower() == "true";
    }

    QString jwtToken = obj.value("jwt").toString();
    QString accessToken = obj.value("access_token").toString();
    int userId = obj.value("user_id").toInt(-1);

    if (success) {
        if (!jwtToken.isEmpty()) {
            SecureStorage::instance().setValue("jwt-token", jwtToken);
            Logging::instance().log(Logging::Debug, "JWT-токен записан");
        }
        if (!accessToken.isEmpty()) {
            SecureStorage::instance().setValue("access-token", accessToken);
            Logging::instance().log(Logging::Debug, "Access-токен записан");
        }
        if (!m_username.isEmpty()) {
            SecureStorage::instance().setValue("username", m_username);
            Logging::instance().log(Logging::Debug, "Имя пользователя записано");
        }
        if (userId != -1) {
            SecureStorage::instance().setValue("user_id", QString::number(userId));
            Logging::instance().log(Logging::Debug, "Имя пользователя записано");
        }

        clearPendingRequest();
        emit authSucceeded();
    } else {
        QString raw = obj.value("error").toString();
        failAuth(raw);
    }
}

// ------------------- Private helpers -------------------

void AuthManager::sendPendingRequest()
{
    qDebug() << "AUTHMANAGER --> sendPendingRequest begin --> start";
    if (!m_socket) return;
    if (m_pendingRequest.type == AuthRequestType::None) return;

    QJsonDocument doc(m_pendingRequest.payload);
    QString jsonStr = doc.toJson(QJsonDocument::Compact);

    if (m_socket->state() == SocketState::Connected) {
        m_socket->sendMessage(jsonStr);
    } else if (m_socket->state() == SocketState::Idle ||
               m_socket->state() == SocketState::Disconnected) {
        m_socket->connectToServer(ApiEndpoints::instance().getEndpoint("auth"));
    }
    qDebug() << "AUTHMANAGER --> sendPendingRequest begin --> finish";
}

void AuthManager::clearPendingRequest()
{
    m_pendingRequest.type = AuthRequestType::None;
    m_pendingRequest.payload = {};
    m_username.clear();
}

void AuthManager::failAuth(const QString &errorMessage, bool markAuthFailed)
{
    if (markAuthFailed) clearPendingRequest();
    emit authFailed(errorMessage);
}
