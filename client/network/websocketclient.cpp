#include "network/websocketclient.h"
#include "utils/logging.h"
#include "core/errorcode.h"

WebSocketClient::WebSocketClient(QObject *parent)
    : QObject(parent),
    m_state(SocketState::Idle),
    m_lastDisconnectReason(DisconnectReason::None)
{
    setupSocket();
    m_socket.setSslConfiguration(QSslConfiguration::defaultConfiguration());
    connect(&m_socket, &QWebSocket::errorOccurred, this, [](QAbstractSocket::SocketError error) {
        qDebug() << "SOCKET ERROR CODE:" << error;
    });
}

void WebSocketClient::connectToServer(const QUrl& url)
{
    qDebug() << "WebSocketClient --> connectToServer begin --> start";

    if (m_state == SocketState::Connecting || m_state == SocketState::Connected)
        return;

    m_lastUrl = url;
    m_state = SocketState::Connecting;

    QNetworkRequest request(url);

    request.setRawHeader("Host", url.host().toUtf8());
    request.setRawHeader("User-Agent", "Mozilla/5.0 (Macintosh; Intel Mac OS X)");
    request.setRawHeader("Origin", QString("https://%1").arg(url.host()).toUtf8());

    QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
    sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone);

    sslConfig.setProtocol(QSsl::AnyProtocol);
    m_socket.setSslConfiguration(sslConfig);

    QObject::connect(&m_socket, &QWebSocket::sslErrors,
                     [](const QList<QSslError> &errors) {
                         qDebug() << "SSL errors ignored:" << errors;
                     });

    m_socket.open(request);

    qDebug() << "WebSocketClient --> connectToServer begin --> finish";
}

void WebSocketClient::disconnectFromServer()
{
    cleanupReconnect();

    m_lastDisconnectReason = DisconnectReason::Manual;

    m_socket.close();
}

void WebSocketClient::forceReconnect() {
    cleanupReconnect();
    m_retryCount = 0;
    m_lastDisconnectReason = DisconnectReason::None;

    if (m_state == SocketState::Connecting || m_state == SocketState::Reconnecting) {
        m_socket.abort();
    }

    m_state = SocketState::Idle;
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

void WebSocketClient::setLastUrl(const QUrl& url)
{
    if (url.isValid()) {
        m_lastUrl = url;
        LOG(Logging::Debug, ErrorCode::make(ErrorCode::Network, 0x00, ErrorCode::WebSocketClient),
            "WebSocket URL обновлён: " + url.toString().toUtf8());
    }
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

    startPing();
    emit connected();
}

void WebSocketClient::onDisconnected()
{
    stopPing();
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
            &QWebSocket::errorOccurred,
            this,
            &WebSocketClient::onError);

    connect(&m_socket, &QWebSocket::textMessageReceived,
            this, &WebSocketClient::messageReceived);

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

void WebSocketClient::startPing()
{
    if (!m_pingTimer.isActive()) {
        connect(&m_pingTimer, &QTimer::timeout, [this]() {
            if (m_state == SocketState::Connected) {
                m_socket.ping();
            }
        });
        m_pingTimer.start(20000); // каждые 20 сек
    }
}

void WebSocketClient::stopPing()
{
    if (m_pingTimer.isActive()) {
        m_pingTimer.stop();
        disconnect(&m_pingTimer, nullptr, nullptr, nullptr);
    }
}