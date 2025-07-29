#include "network/websocketclient.h"

WebSocketClient::WebSocketClient(QObject *parent)
    : QObject(parent)
{
    connect(&m_webSocket, &QWebSocket::connected, this, &WebSocketClient::connected);
    connect(&m_webSocket, &QWebSocket::disconnected, this, &WebSocketClient::disconnected);
    connect(&m_webSocket, &QWebSocket::textMessageReceived, this, &WebSocketClient::messageReceived);
    connect(&m_webSocket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::errorOccurred),
            this, [this](QAbstractSocket::SocketError error) {
                Q_UNUSED(error)
                emit errorOccurred(m_webSocket.errorString());
            });
}

void WebSocketClient::connectToServer(const QUrl &url)
{
    QSslConfiguration sslConf = QSslConfiguration::defaultConfiguration();
    m_webSocket.setSslConfiguration(sslConf);
    m_webSocket.open(url);
}

void WebSocketClient::sendMessage(const QString &message)
{
    if (m_webSocket.state() == QAbstractSocket::ConnectedState)
        m_webSocket.sendTextMessage(message);
}

void WebSocketClient::disconnect()
{
    m_webSocket.close();
}
