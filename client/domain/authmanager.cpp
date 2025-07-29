#include "domain/authmanager.h"
#include "network/websocketclient.h"

AuthManager::AuthManager(QObject *parent)
    : QObject(parent)
{
    m_socket = new WebSocketClient(this);
    connect(m_socket, &WebSocketClient::connected, this, &AuthManager::onConnected);
    connect(m_socket, &WebSocketClient::messageReceived, this, &AuthManager::onMessageReceived);
}

void AuthManager::tryAuth(const QString &login, const QString &password)
{
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
        QString err = obj.value("error").toString();
        emit authFailed(err);
    }
}
