#include "domain/authmanager.h"
#include "network/websocketclient.h"
#include "core/apiendpoints.h"
#include "utils/logging.h"
#include "core/errorcode.h"
#include "core/securestorage.h"

AuthManager::AuthManager(QObject *parent)
    : QObject(parent)
{
    m_socket = new WebSocketClient("auth", this);
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
        emit authFailed("");
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
    this->username = login;

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
    this->username = login;

    QJsonDocument doc(obj);
    m_pendingRequest = doc.toJson(QJsonDocument::Compact);

    if (m_socket->getState() == QAbstractSocket::ConnectedState) {
        m_socket->sendMessage(m_pendingRequest);
    } else {
        m_socket->connectToServer(ApiEndpoints::instance().getEndpoint("auth"));
    }
}

void AuthManager::tryAutoTokenLogin(const QString &access_token)
{
    if (m_socket && m_socket->getState() == QAbstractSocket::ConnectingState) {
        LOG(Logging::Warning, ErrorCode::make(ErrorCode::Network, 0x03, ErrorCode::AuthManager), "");
        return;
    }

    QJsonObject obj;
    obj["action"] = "auto_token_login";
    obj["token"] = access_token;

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
        LOG(Logging::Debug, ErrorCode::make(ErrorCode::UI, 0x02, ErrorCode::AuthManager), "Полученный ответ сервера: " + text.toUtf8());
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

    QString jwt_token = obj.value("jwt").toString();
    QString access_token = obj.value("access_token").toString();


    if (success) {
        if (!jwt_token.isEmpty()) {
            SecureStorage::instance().setValue("jwt-token", jwt_token);
            Logging::instance().log(Logging::Debug, "JWT-токен записан.");
        }
        if (!access_token.isEmpty()) {
            SecureStorage::instance().setValue("access-token", access_token);
            Logging::instance().log(Logging::Debug, "Access-токен записан.");
        }

        if (!username.isEmpty()) {
            SecureStorage::instance().setValue("username", username);
            Logging::instance().log(Logging::Debug, "Имя пользователя записано.");
        }
        emit authSucceeded();
    } else {
        QString raw = obj.value("error").toString();
        emit authFailed(raw);
    }

    this->m_socket->disconnect();
}
