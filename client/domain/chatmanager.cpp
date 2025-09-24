#include "domain/chatmanager.h"
#include "utils/logging.h"
#include "core/errorcode.h"
#include "core/apiendpoints.h"
#include "core/securestorage.h"
#include <QTimer>
#include <QJsonParseError>
#include <QJsonArray>
#include <QJsonObject>

ChatManager::ChatManager(QObject *parent)
{
    m_socket = new WebSocketClient("chat", this);
    connect(m_socket, &WebSocketClient::connected, this, [this]() {
        emit connected();
        onConnected();
    });
    connect(m_socket, &WebSocketClient::disconnected, this, [this]() {
        emit disconnected();
    });
    connect(m_socket, &WebSocketClient::messageReceived, this, &ChatManager::onMessageReceived);
    connect(m_socket, &WebSocketClient::errorOccurred, this, [this](const QString &error) {
        if (error.contains("502")) {
            LOG(Logging::Warning, ErrorCode::make(ErrorCode::Network, 0x04, ErrorCode::ChatManager), "");
        } else if (error.contains("SSL") || error.contains("handshake")) {
            LOG(Logging::Warning, ErrorCode::make(ErrorCode::Network, 0x05, ErrorCode::ChatManager), "");
        } else {
            LOG(Logging::Warning, ErrorCode::make(ErrorCode::Network, 0x01, ErrorCode::ChatManager), error);
        }
    });
}

void ChatManager::connectToChat()
{
    m_socket->connectToServer(ApiEndpoints::instance().getEndpoint("chat"));
}

void ChatManager::connectToServer(const QUrl &url)
{
    if (!m_socket)
        return;

    auto state = m_socket->getState();
    if (state == QAbstractSocket::ConnectingState || state == QAbstractSocket::ConnectedState) {
        LOG(Logging::Warning, ErrorCode::make(ErrorCode::Network, 0x03, ErrorCode::ChatManager), "Соединение уже выполняется/подключено");
        return;
    }

    m_socket->connectToServer(url);
}

void ChatManager::sendMessage(const QString &text)
{
    if (!m_socket)
        return;

    auto state = m_socket->getState();
    if (state == QAbstractSocket::ConnectedState) {
        m_socket->sendMessage(text);
    } else if (state == QAbstractSocket::UnconnectedState) {
        LOG(Logging::Info, ErrorCode::make(ErrorCode::Network, 0x10, ErrorCode::ChatManager), "Соединение не установлено. Попытка подключиться...");
        m_socket->connectToServer(ApiEndpoints::instance().getEndpoint("chat"));
        m_pendingMessages.enqueue(text);
    }
}

bool ChatManager::isConnected() const {
    if (!m_socket)
        return false;

    auto state = m_socket->getState();
    return state == QAbstractSocket::ConnectedState;
}

void ChatManager::onConnected()
{
    emit connected();
    while (!m_pendingMessages.isEmpty()) {
        m_socket->sendMessage(m_pendingMessages.dequeue());
    }
    QTimer* pingTimer = new QTimer(m_socket);
    QObject::connect(pingTimer, &QTimer::timeout, [this]() {
        m_socket->ping();
    });
    pingTimer->start(20000);
}

void ChatManager::onDisconnected()
{
    emit disconnected();
}

void ChatManager::onMessageReceived(const QString &text)
{
    qDebug() << "RECEIVED " << text;
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(text.toUtf8(), &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject()) {
        LOG(Logging::Warning, ErrorCode::make(ErrorCode::Network, 0x02, ErrorCode::ChatManager), "Invalid JSON");
        return;
    }

    QJsonObject obj = doc.object();
    QString action = obj["action"].toString();

    if (action == "user_servers") {
        QJsonArray servers = obj["servers"].toArray();
        emit userServersReceived(servers);
    } else {
        emit messageReceived(text);
    }
}

void ChatManager::requestUserServers() {
    QJsonObject obj;
    obj["action"] = "get_user_servers";

    auto jwt_opt = SecureStorage::instance().getValue("jwt-token");
    QString jwt_token;

    if (jwt_opt.has_value()) {
        jwt_token = jwt_opt.value();
    }

    if (jwt_token.isEmpty()) {
        Logging::instance().log(Logging::Warning, "JWT отсутствует, запрос серверов не отправлен");
        return;
    }

    obj["jwt"] = jwt_token;

    QJsonDocument doc(obj);
    sendMessage(QString::fromUtf8(doc.toJson(QJsonDocument::Compact)));
}
