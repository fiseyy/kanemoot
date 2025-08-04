#include "domain/authmanager.h"
#include "network/websocketclient.h"
#include "core/errorhandler.h"

AuthManager::AuthManager(QObject *parent)
    : QObject(parent)
{
    m_socket = new WebSocketClient(this);
    connect(m_socket, &WebSocketClient::connected, this, &AuthManager::onConnected);
    connect(m_socket, &WebSocketClient::messageReceived, this, &AuthManager::onMessageReceived);
    connect(m_socket, &WebSocketClient::errorOccurred, this, [this](const QString &error) {
        QString userFriendly;
        if (error.contains("502")) {
            userFriendly = "Сервер временно недоступен. Повторите попытку позже.";
        } else if (error.contains("SSL") || error.contains("handshake")) {
            userFriendly = "Ошибка шифрования. Проверьте подключение.";
        } else {
            userFriendly = error;
        }
        ErrorHandler::instance().showError("Ошибка", userFriendly);
    });
}

void AuthManager::tryAuth(const QString &login, const QString &password)
{
    if (m_socket && m_socket->getState() == QAbstractSocket::ConnectingState) {
        ErrorHandler::instance().showError("Предупреждение", "Подключение уже выполняется. Подождите.");
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
        m_socket->connectToServer(QUrl("wss://auth-service.kanemoot.ru/ws"));
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
        return;
    }
    QJsonObject obj = doc.object();
    bool success = obj.value("success").toString() == "true";
    if (success) {
        emit authSucceeded();
    } else {
        QString raw = obj.value("error").toString();
        emit authFailed(raw);
    }
}
