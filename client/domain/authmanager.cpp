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
    connect(m_socket, &WebSocketClient::connected, this, &AuthManager::onConnected);
    connect(m_socket, &WebSocketClient::messageReceived, this, &AuthManager::onMessageReceived);
    connect(m_socket, &WebSocketClient::errorOccurred, this, [this](const QString &error) {
        if (error.contains("502")) {
            // userFriendly = "Сервер временно недоступен. Повторите попытку позже.";
            LOG(Logging::Warning, ErrorCode::make(ErrorCode::Network, 0x04, ErrorCode::AuthManager), "");
        } else if (error.contains("SSL") || error.contains("handshake")) {
            // userFriendly = "Ошибка шифрования. Проверьте подключение.";
            LOG(Logging::Warning, ErrorCode::make(ErrorCode::Network, 0x05, ErrorCode::AuthManager), "");
        } else {
            LOG(Logging::Warning, ErrorCode::make(ErrorCode::Network, 0x01, ErrorCode::AuthManager), error);
        }
    });
}

void AuthManager::tryAuth(const QString &login, const QString &password)
{
    if (m_socket && m_socket->getState() == QAbstractSocket::ConnectingState) {
        // ErrorHandler::instance().showError("Предупреждение", "Подключение уже выполняется. Подождите.");
        LOG(Logging::Warning, ErrorCode::make(ErrorCode::Network, 0x03, ErrorCode::AuthManager), "");
        return;
    }

    QJsonObject obj;
    obj["action"] = "login";
    obj["user"] = login;
    obj["password"] = password;

    QJsonDocument doc(obj);
    m_pendingRequest = doc.toJson(QJsonDocument::Compact);

    if (m_socket->getState() == QAbstractSocket::ConnectedState) {
        m_socket->sendMessage(m_pendingRequest);
    } else {
        m_socket->connectToServer(ApiEndpoints::instance().getEndpoint("auth"));
    }
}

void AuthManager::tryReg(const QString &login, const QString &password, const QString &email)
{
    if (m_socket && m_socket->getState() == QAbstractSocket::ConnectingState) {
        // ErrorHandler::instance().showError("Предупреждение", "Подключение уже выполняется. Подождите.");
        LOG(Logging::Warning, ErrorCode::make(ErrorCode::Network, 0x03, ErrorCode::AuthManager), "");
        return;
    }

    QJsonObject obj;
    obj["action"] = "reg";
    obj["user"] = login;
    obj["password"] = password;
    obj["email"] = email;

    QJsonDocument doc(obj);
    m_pendingRequest = doc.toJson(QJsonDocument::Compact);

    if (m_socket->getState() == QAbstractSocket::ConnectedState) {
        m_socket->sendMessage(m_pendingRequest);
    } else {
        m_socket->connectToServer(ApiEndpoints::instance().getEndpoint("auth"));
    }
}

void AuthManager::tryAutoLogin(const QString &jwt_token)
{
    if (m_socket && m_socket->getState() == QAbstractSocket::ConnectingState) {
        // ErrorHandler::instance().showError("Предупреждение", "Подключение уже выполняется. Подождите.");
        LOG(Logging::Warning, ErrorCode::make(ErrorCode::Network, 0x03, ErrorCode::AuthManager), "");
        return;
    }

    QJsonObject obj;
    obj["action"] = "login";
    obj["jwt"] = jwt_token;

    QJsonDocument doc(obj);
    m_pendingRequest = doc.toJson(QJsonDocument::Compact);

    if (m_socket->getState() == QAbstractSocket::ConnectedState) {
        m_socket->sendMessage(m_pendingRequest);
    } else {
        m_socket->connectToServer(ApiEndpoints::instance().getEndpoint("auth"));
    }
}

void AuthManager::onConnected()
{
    if (!m_pendingRequest.isEmpty()) {
        m_socket->sendMessage(m_pendingRequest);
    }
}

void AuthManager::onMessageReceived(const QString &text)
{
    QJsonDocument doc = QJsonDocument::fromJson(text.toUtf8());
    if (!doc.isObject()) {
        emit authFailed("Неверный формат ответа сервера");
        // LOG(Logging::Debug, "Неверный формат ответа сервера. Полученный ответ сервера: " + text.toUtf8());
        LOG(Logging::Debug, ErrorCode::make(ErrorCode::UI, 0x02, ErrorCode::AuthManager), "Полученный ответ сервера: " + text.toUtf8());
        return;
    }
    QJsonObject obj = doc.object();
    bool success = obj.value("success").toString() == "true";
    QString jwt_token = obj.value("jwt").toString();
    if (success) {
        emit authSucceeded();
        SecureStorage::instance().setValue("jwt-token", jwt_token);
    } else {
        QString raw = obj.value("error").toString();
        emit authFailed(raw);
    }
}
