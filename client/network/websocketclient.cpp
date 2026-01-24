#include "network/websocketclient.h"

WebSocketClient::WebSocketClient(QObject *parent)
    : QObject(parent),
    m_state(SocketState::Idle),
    m_lastDisconnectReason(DisconnectReason::None)
{
    setupSocket();
    m_socket.setSslConfiguration(QSslConfiguration::defaultConfiguration());
}

void WebSocketClient::connectToServer(const QUrl& url)
{
    if (m_state == SocketState::Connecting ||
        m_state == SocketState::Connected ||
        m_state == SocketState::Reconnecting)
        return;

    if (!m_lastUrl.isValid())
        return;

    m_lastUrl = url;
    m_state = SocketState::Connecting;
    m_lastDisconnectReason = DisconnectReason::None;

    m_socket.open(url);
}

void WebSocketClient::disconnectFromServer()
{
    cleanupReconnect();

    m_lastDisconnectReason = DisconnectReason::Manual;

    m_socket.close();
}

void WebSocketClient::forceReconnect() {
    m_retryCount = 0;
    m_lastDisconnectReason = DisconnectReason::None;
    m_socket.abort();
    connectToServer(m_lastUrl);
}

void WebSocketClient::sendMessage(const QString& msg)
{
    if (m_state != SocketState::Connected)
        return;

    m_socket.sendTextMessage(msg);
}

void WebSocketClient::setDisconnectReason(DisconnectReason reason)
{
    m_lastDisconnectReason = reason;
}

SocketState WebSocketClient::state() const {
    return m_state;
}

DisconnectReason WebSocketClient::lastDisconnectReason() const {
    return m_lastDisconnectReason;
}

void WebSocketClient::onConnected()
{
    cleanupReconnect();

    m_state = SocketState::Connected;
    m_lastDisconnectReason = DisconnectReason::None;

    emit connected();
}

void WebSocketClient::onDisconnected()
{
    if (m_state == SocketState::Disconnected &&
        m_lastDisconnectReason == DisconnectReason::Manual) {
        emit disconnected(m_lastDisconnectReason);
        return;
    }

    if (m_lastDisconnectReason == DisconnectReason::None)
        m_lastDisconnectReason = DisconnectReason::ServerClosed;

    m_state = SocketState::Disconnected;
    emit disconnected(m_lastDisconnectReason);

    if (m_lastDisconnectReason != DisconnectReason::Manual)
        scheduleReconnect();
}

void WebSocketClient::onError(QAbstractSocket::SocketError)
{
    if (m_lastDisconnectReason == DisconnectReason::None)
        m_lastDisconnectReason = DisconnectReason::NetworkError;

    emit errorOccurred(m_socket.errorString());

    if (m_state != SocketState::Disconnected)
        m_socket.close();
}



void WebSocketClient::setupSocket()
{
    connect(&m_socket, &QWebSocket::connected,
            this, &WebSocketClient::onConnected);

    connect(&m_socket, &QWebSocket::disconnected,
            this, &WebSocketClient::onDisconnected);

    connect(&m_socket,
            QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error),
            this,
            &WebSocketClient::onError);

    connect(&m_socket, &QWebSocket::textMessageReceived,
            this, &WebSocketClient::onTextMessageReceived);

    connect(&m_reconnectTimer, &QTimer::timeout,
            this, &WebSocketClient::doReconnect);

    m_reconnectTimer.setSingleShot(true);
}

void WebSocketClient::cleanupReconnect()
{
    m_reconnectTimer.stop();
    m_retryCount = 0;
}


void WebSocketClient::scheduleReconnect()
{
    if (m_retryCount >= m_retryPolicy.maxRetries) {
        cleanupReconnect();
        return;
    }

    m_state = SocketState::Reconnecting;

    int delay = m_retryPolicy.baseDelayMs * (m_retryCount + 1);
    delay = qMin(delay, m_retryPolicy.maxDelayMs);

    ++m_retryCount;
    m_reconnectTimer.start(delay);
}

void WebSocketClient::doReconnect()
{
    if (m_state != SocketState::Reconnecting)
        return;

    m_state = SocketState::Connecting;

    connectToServer(m_lastUrl);
}
