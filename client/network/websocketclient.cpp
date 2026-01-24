#include "network/websocketclient.h"
#include "utils/logging.h"
#include "core/errorcode.h"
#include <QTimer>

WebSocketClient::WebSocketClient(QString serviceName, QObject *parent)
    : QObject(parent), m_serviceName(serviceName)
{
    m_webSocket = new QWebSocket();

    connectSignals();

    Logging::instance().log(Logging::Debug, "(" + m_serviceName + ") WebSocketClient инициализирован");
}

WebSocketClient::~WebSocketClient()
{
    if (m_webSocket->state() != QAbstractSocket::UnconnectedState)
        m_webSocket->abort();
}

void WebSocketClient::connectToServer(const QUrl &url)
{
    if (!m_webSocket) {
        m_webSocket = new QWebSocket();
        connectSignals();
    }

    if (m_webSocket->state() == QAbstractSocket::ConnectedState ||
        m_webSocket->state() == QAbstractSocket::ConnectingState) {
        qDebug() << "DEBUG --- Не закрыт сокет, сброс команды connectToServer..";
        return;
    }

    qDebug() << "DEBUG --- connectToServer, WebSocketClient start. url = " << url << ", socket state = " << m_webSocket->state();
    m_webSocket->setSslConfiguration(QSslConfiguration::defaultConfiguration());
    m_webSocket->open(url);
    qDebug() << "DEBUG --- connectToServer, WebSocketClient finish";

}

void WebSocketClient::sendMessage(const QString &message)
{
    if (m_webSocket->state() == QAbstractSocket::ConnectedState) {
        m_webSocket->sendTextMessage(message);
    } else {
        LOG(Logging::Warning, ErrorCode::make(ErrorCode::Network, 0x04, ErrorCode::WebSocketClient), "");
        qDebug() << m_webSocket->state();
    }
}

void WebSocketClient::close()
{
    if (!m_webSocket) return;

    if (m_webSocket->state() != QAbstractSocket::UnconnectedState) {
        m_webSocket->close();
    }

    m_webSocket->disconnect();
    m_webSocket->deleteLater();
    m_webSocket = nullptr;

    Logging::instance().log(Logging::Info, "(" + m_serviceName + ") WebSocket-соединение закрыто вручную");
}

void WebSocketClient::reconnect(const QUrl &url)
{
    if (!m_webSocket) {
        createAndConnect(url);
        return;
    }

    if (m_webSocket->state() == QAbstractSocket::UnconnectedState) {
        createAndConnect(url);
        return;
    }

    // Сохраняем URL
    m_pendingUrl = url;

    // Таймаут форсированного закрытия
    QTimer::singleShot(3000, this, [this]() {
        if (m_webSocket && m_webSocket->state() != QAbstractSocket::UnconnectedState) {
            m_webSocket->abort(); // форсируем завершение SSL/подключения
        }
    });

    // Ждём события disconnected для нового подключения
    connect(m_webSocket, &QWebSocket::disconnected, this, [this]() {
        QWebSocket *oldSocket = m_webSocket;
        createAndConnect(m_pendingUrl);
        oldSocket->deleteLater();  // удаляем старый только после старта нового
        m_pendingUrl = QUrl();
    }, Qt::SingleShotConnection);

    m_webSocket->close();
}

void WebSocketClient::onReconnectSlot()
{
    if (m_pendingUrl.isValid()) {
        QUrl url = m_pendingUrl;
        m_pendingUrl = QUrl();

        if (m_webSocket) {
            m_webSocket->deleteLater();
        }
        createAndConnect(url);
    }
}

void WebSocketClient::createAndConnect(const QUrl &url)
{
    m_webSocket = new QWebSocket();
    connectSignals();
    m_webSocket->open(url);
}


QAbstractSocket::SocketState WebSocketClient::getState()
{
    return m_webSocket->state();
}

void WebSocketClient::ping()
{
    m_webSocket->ping();
}

bool WebSocketClient::isConnected() const {
    return m_webSocket && m_webSocket->state() == QAbstractSocket::ConnectedState;
}


void WebSocketClient::onSocketError(QAbstractSocket::SocketError)
{
    QString error_str = m_webSocket->errorString();
    emit errorOccurred(error_str);
}

void WebSocketClient::connectSignals()
{
    connect(m_webSocket, &QWebSocket::connected, this, &WebSocketClient::connected);
    connect(m_webSocket, &QWebSocket::disconnected, this, &WebSocketClient::disconnected);
    connect(m_webSocket, &QWebSocket::textMessageReceived, this, &WebSocketClient::messageReceived);
    connect(m_webSocket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::errorOccurred),
            this, &WebSocketClient::onSocketError);
}

