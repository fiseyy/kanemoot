#include "network/websocketclient.h"
#include "utils/logging.h"

WebSocketClient::WebSocketClient(QObject *parent)
    : QObject(parent)
{
    connect(&m_webSocket, &QWebSocket::connected, this, &WebSocketClient::connected);
    connect(&m_webSocket, &QWebSocket::disconnected, this, &WebSocketClient::disconnected);
    connect(&m_webSocket, &QWebSocket::textMessageReceived, this, &WebSocketClient::messageReceived);
    connect(&m_webSocket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::errorOccurred),
            this, [this](QAbstractSocket::SocketError error) {
                Q_UNUSED(error)
                LOG(Logging::Warning, "WebSocket error: " + m_webSocket.errorString());
                emit errorOccurred(m_webSocket.errorString());
            });
    connect(this, &WebSocketClient::messageReceived, this, [this](QString msg){
        qDebug() << "Получено:" << msg;
    });

    LOG(Logging::Debug, "WebSocketClient constructed");
}

WebSocketClient::~WebSocketClient()
{
    if (m_webSocket.state() == QAbstractSocket::ConnectedState ||
        m_webSocket.state() == QAbstractSocket::ConnectingState) {
        LOG(Logging::Info, "Closing WebSocket connection in destructor");
        m_webSocket.close();
    } else {
        LOG(Logging::Debug, "WebSocket already closed in destructor");
    }
}

void WebSocketClient::connectToServer(const QUrl &url)
{
    QSslConfiguration sslConf = QSslConfiguration::defaultConfiguration();
    m_webSocket.setSslConfiguration(sslConf);
    m_webSocket.open(url);
    LOG(Logging::Info, "Connecting to WebSocket server: " + url.toString());
}

void WebSocketClient::sendMessage(const QString &message)
{
    if (m_webSocket.state() == QAbstractSocket::ConnectedState) {
        m_webSocket.sendTextMessage(message);
        LOG(Logging::Debug, "Sent message: " + message);
    } else {
        LOG(Logging::Warning, "Attempted to send message while not connected");
        qDebug() << m_webSocket.state();
    }
}

void WebSocketClient::disconnect()
{
    if (m_webSocket.state() == QAbstractSocket::ConnectedState) {
        m_webSocket.close();
        LOG(Logging::Info, "Manually closed WebSocket connection");
    } else {
        LOG(Logging::Debug, "WebSocket already disconnected");
    }
}
