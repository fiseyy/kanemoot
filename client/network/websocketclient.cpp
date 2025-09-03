#include "network/websocketclient.h"
#include "utils/logging.h"
#include "core/errorcode.h"

WebSocketClient::WebSocketClient(QString serviceName, QObject *parent)
    : QObject(parent), m_serviceName(serviceName)
{
    connect(&m_webSocket, &QWebSocket::connected, this, &WebSocketClient::connected);
    connect(&m_webSocket, &QWebSocket::disconnected, this, &WebSocketClient::disconnected);
    connect(&m_webSocket, &QWebSocket::textMessageReceived, this, &WebSocketClient::messageReceived);
    connect(&m_webSocket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::errorOccurred),
            this, [this](QAbstractSocket::SocketError error) {
        QString error_str = m_webSocket.errorString();
        emit errorOccurred(error_str);
    });

    Logging::instance().log(Logging::Debug, "(" + m_serviceName + ") WebSocketClient инициализирован");
}

WebSocketClient::~WebSocketClient()
{
    if (m_webSocket.state() != QAbstractSocket::UnconnectedState)
        m_webSocket.abort();  // сразу рвёт SSL и сокет
}

void WebSocketClient::connectToServer(const QUrl &url)
{
    QSslConfiguration sslConf = QSslConfiguration::defaultConfiguration();
    m_webSocket.setSslConfiguration(sslConf);
    m_webSocket.open(url);
    Logging::instance().log(Logging::Info, QString("(" + m_serviceName + ") Подключение к WebSocket-серверу"));
}

void WebSocketClient::sendMessage(const QString &message)
{
    if (m_webSocket.state() == QAbstractSocket::ConnectedState) {
        m_webSocket.sendTextMessage(message);
    } else {
        // LOG(Logging::Warning, "Сервер временно недоступен. Повторите попытку позже.");
        LOG(Logging::Warning, ErrorCode::make(ErrorCode::Network, 0x04, ErrorCode::WebSocketClient), "");
        qDebug() << m_webSocket.state();
    }
}

void WebSocketClient::disconnect()
{
    if (m_webSocket.state() == QAbstractSocket::ConnectedState) {
        m_webSocket.close();
        Logging::instance().log(Logging::Info, "(" + m_serviceName + ") WebSocket-соединение закрыто вручную");
    } else {
        Logging::instance().log(Logging::Debug, "(" + m_serviceName + ") WebSocket-соединение уже закрыто (disconnect)");
    }
}

QAbstractSocket::SocketState WebSocketClient::getState()
{
    return m_webSocket.state();
}

void WebSocketClient::ping()
{
    m_webSocket.ping();
}
