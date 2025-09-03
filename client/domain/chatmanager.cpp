#include "domain/chatmanager.h"
#include "utils/logging.h"
#include "core/errorcode.h"
#include "core/apiendpoints.h"
#include <QTimer>

ChatManager::ChatManager(QObject *parent)
{
    m_socket = new WebSocketClient(this);
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
        QTimer::singleShot(2000, [this]() {
            m_socket->connectToServer(ApiEndpoints::instance().getEndpoint("chat"));
        });
    });
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

void ChatManager::onConnected()
{
    emit connected();
    while (!m_pendingMessages.isEmpty()) {
        m_socket->sendMessage(m_pendingMessages.dequeue());
    }
}

void ChatManager::onDisconnected()
{
    emit disconnected();
}

void ChatManager::onMessageReceived(const QString &text)
{
    emit messageReceived(text);
}
