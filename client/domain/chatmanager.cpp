#include "domain/chatmanager.h"
#include "core/apiendpoints.h"
#include "core/securestorage.h"
#include "utils/logging.h"
#include "core/errorcode.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QVariantMap>

ChatManager::ChatManager(QObject* parent)
    : QObject(parent)
{
    m_socket = new WebSocketClient(this);

    connect(m_socket, &WebSocketClient::connected, this, &ChatManager::onSocketConnected);
    connect(m_socket, &WebSocketClient::disconnected, this, &ChatManager::onSocketDisconnected);
    connect(m_socket, &WebSocketClient::errorOccurred, this, &ChatManager::onSocketError);
    connect(m_socket, &WebSocketClient::messageReceived, this, &ChatManager::onMessageReceived);

    connect(&ApiEndpoints::instance(), &ApiEndpoints::endpointChanged,
            this, &ChatManager::onEndpointChanged);
}

// ------------------- Public API -------------------

void ChatManager::connectToChat()
{
    if (!m_socket) return;

    auto state = m_socket->state();
    if (state == SocketState::Connecting || state == SocketState::Connected) {
        LOG(Logging::Warning, ErrorCode::make(ErrorCode::Network, 0x03, ErrorCode::ChatManager),
            "Попытка подключения к чату при уже существующем соединении");
        return;
    }

    m_socket->connectToServer(ApiEndpoints::instance().getEndpoint("chat"));
}

bool ChatManager::isConnected() const
{
    if (!m_socket) return false;
    return m_socket->state() == SocketState::Connected;
}

// ------------------- События WebSocket -------------------

void ChatManager::onSocketConnected()
{
    LOG(Logging::Debug, ErrorCode::make(ErrorCode::Network, 0x00, ErrorCode::ChatManager),
        "Соединение с чатом установлено");
    emit connected();
    sendPendingMessages();
}

void ChatManager::onSocketDisconnected(DisconnectReason reason)
{
    Q_UNUSED(reason);
    LOG(Logging::Debug, ErrorCode::make(ErrorCode::Network, 0x00, ErrorCode::ChatManager),
        "Соединение с чатом разорвано");
    emit disconnected();
}

void ChatManager::onSocketError(const QString &error)
{
    if (error.contains("502")) {
        LOG(Logging::Warning, ErrorCode::make(ErrorCode::Network, 0x04, ErrorCode::ChatManager), error);
    } else if (error.contains("SSL") || error.contains("handshake")) {
        LOG(Logging::Warning, ErrorCode::make(ErrorCode::Network, 0x05, ErrorCode::ChatManager), error);
    } else {
        LOG(Logging::Warning, ErrorCode::make(ErrorCode::Network, 0x01, ErrorCode::ChatManager), error);
    }
    emit errorOccurred(error);
}

// ------------------- Получение сообщений -------------------

void ChatManager::onMessageReceived(const QString &text)
{
    if (text.isEmpty()) return;

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(text.toUtf8(), &parseError);
    if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
        LOG(Logging::Warning, ErrorCode::make(ErrorCode::Network, 0x02, ErrorCode::ChatManager),
            "Неверный формат JSON: " + text.toUtf8());
        return;
    }

    QJsonObject obj = doc.object();
    QString action = obj.value("action").toString();

    if (action == "error") {
        QString code = obj.value("code").toString();
        QString message = obj.value("message").toString();
        LOG(Logging::Warning, ErrorCode::make(ErrorCode::Network, 0x06, ErrorCode::ChatManager),
            "Ошибка сервера: " + code.toUtf8() + " - " + message.toUtf8());

        if (code == "JWT_EXPIRED") {
            emit jwtExpired();
        }
        return;
    }

    if (action == "user_servers") {
        QJsonArray servers = obj.value("servers").toArray();
        emit userServersReceived(servers);
    } else if (action == "new_message") {
        QVariantMap msgMap = jsonObjectToVariantMap(obj.value("message").toObject());
        emit newMessageReceived(msgMap);
    } else if (action == "messages") {
        QJsonArray messagesArray = obj.value("messages").toArray();
        for (const QJsonValue &val : messagesArray) {
            QVariantMap msgMap = jsonObjectToVariantMap(val.toObject());
            emit newMessageReceived(msgMap);
        }
    }

    emit messageReceived(text);
}

// ------------------- Отправка сообщений -------------------

void ChatManager::sendPendingMessages()
{
    if (!m_socket) return;

    while (!m_pendingMessages.isEmpty()) {
        QJsonObject obj = m_pendingMessages.dequeue().payload;
        QJsonDocument doc(obj);
        QString jsonStr = QString::fromUtf8(doc.toJson(QJsonDocument::Compact));
        m_socket->sendMessage(jsonStr);
        LOG(Logging::Debug, ErrorCode::make(ErrorCode::Network, 0x00, ErrorCode::ChatManager),
            "Отправлено отложенное сообщение: " + jsonStr.toUtf8());
    }
}

void ChatManager::enqueueMessage(const QJsonObject &obj)
{
    m_pendingMessages.enqueue({ obj });
}

// ------------------- JWT и запросы -------------------

bool ChatManager::addJwtToObject(QJsonObject &obj, const QString &action)
{
    auto jwt_opt = SecureStorage::instance().getValue("jwt-token");
    if (!jwt_opt.has_value() || jwt_opt.value().isEmpty()) {
        LOG(Logging::Warning, ErrorCode::make(ErrorCode::Network, 0x07, ErrorCode::ChatManager),
            "JWT отсутствует, " + action + " не отправлен");
        return false;
    }
    obj["jwt"] = jwt_opt.value();
    return true;
}

void ChatManager::requestUserServers()
{
    QJsonObject obj { {"action", "get_user_servers"} };
    if (!addJwtToObject(obj, "requestUserServers")) return;
    enqueueMessage(obj);
    sendPendingMessages();
}

void ChatManager::joinServer(const QString &inviteLink)
{
    QJsonObject obj { {"action", "join_server"}, {"url", inviteLink} };
    if (!addJwtToObject(obj, "joinServer")) return;
    enqueueMessage(obj);
    sendPendingMessages();
}

void ChatManager::createServer(const QString &name)
{
    QJsonObject obj { {"action", "create_server"}, {"name", name} };
    if (!addJwtToObject(obj, "createServer")) return;
    enqueueMessage(obj);
    sendPendingMessages();
}

void ChatManager::createChannel(int serverId, const QString &name, const QString &type)
{
    QJsonObject obj {
        {"action", "create_channel"},
        {"server_id", serverId},
        {"channel_name", name},
        {"channel_type", type}
    };
    if (!addJwtToObject(obj, "createChannel")) return;
    enqueueMessage(obj);
    sendPendingMessages();
}

void ChatManager::deleteChannel(int channelId, int serverId)
{
    QJsonObject obj {
        {"action", "delete_channel"},
        {"channel_id", channelId},
        {"server_id", serverId}
    };
    if (!addJwtToObject(obj, "deleteChannel")) return;
    enqueueMessage(obj);
    sendPendingMessages();
}

void ChatManager::getMessages(int channelId, int guildId)
{
    QJsonObject obj {
        {"action", "get_messages"},
        {"channel_id", channelId},
        {"guild_id", guildId}
    };
    if (!addJwtToObject(obj, "getMessages")) return;
    enqueueMessage(obj);
    sendPendingMessages();
}

void ChatManager::sendChatMessage(int guildId, int channelId, const QString &text)
{
    if (text.trimmed().isEmpty()) return;

    QJsonObject obj {
        {"action", "send_message"},
        {"guild_id", guildId},
        {"channel_id", channelId},
        {"content", text}
    };
    if (!addJwtToObject(obj, "sendChatMessage")) return;
    enqueueMessage(obj);
    sendPendingMessages();
}

// ------------------- ApiEndpoints -------------------

void ChatManager::onEndpointChanged(const QString &service, const QUrl &newUrl)
{
    if (service != "chat" || !m_socket) return;

    LOG(Logging::Debug, ErrorCode::make(ErrorCode::Network, 0x00, ErrorCode::ChatManager),
        "Endpoint chat изменён: " + newUrl.toString().toUtf8());

    m_socket->setLastUrl(newUrl);
    m_socket->forceReconnect();
}

// ------------------- Вспомогательные методы -------------------

QVariantMap ChatManager::jsonObjectToVariantMap(const QJsonObject &obj)
{
    QVariantMap map;
    for (auto it = obj.begin(); it != obj.end(); ++it) {
        map[it.key()] = it.value().toVariant();
    }
    return map;
}
