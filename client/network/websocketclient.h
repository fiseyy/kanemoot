#ifndef WEBSOCKETCLIENT_H
#define WEBSOCKETCLIENT_H

#include <QObject>
#include <QtWebSockets/QWebSocket>
#include <QSslConfiguration>
#include <QTimer>
#include <QUrl>

enum class SocketState {
    Idle,
    Connecting,
    Connected,
    Reconnecting,
    Disconnected
};

enum class DisconnectReason {
    None,
    NetworkError,
    ServerClosed,
    AuthFailed,
    Manual,
    Unknown
};

struct RetryPolicy {
    int maxRetries = 5;
    int baseDelayMs = 1000; // 1 сек
    int maxDelayMs  = 10000; // 10 сек
};

class WebSocketClient : public QObject {
    Q_OBJECT
public:
    explicit WebSocketClient(QObject* parent = nullptr);

    void connectToServer(const QUrl& url);
    void disconnectFromServer();
    void forceReconnect();
    void sendMessage(const QString&);
    void setDisconnectReason(DisconnectReason reason);
    void setLastUrl(const QUrl& url);

    SocketState state() const;
    DisconnectReason lastDisconnectReason() const;

signals:
    void connected();
    void disconnected(DisconnectReason reason);
    void errorOccurred(QString error);
    void messageReceived(QString message);

private slots:
    void onConnected();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError error);

private:
    QWebSocket m_socket;

    QUrl m_lastUrl;

    SocketState m_state;
    DisconnectReason m_lastDisconnectReason;

    RetryPolicy m_retryPolicy;
    int m_retryCount = 0;
    QTimer m_reconnectTimer;
    QTimer m_pingTimer;

    void setupSocket();
    void cleanupReconnect();
    void scheduleReconnect();
    void doReconnect();
    void startPing();
    void stopPing();
};


#endif // WEBSOCKETCLIENT_H
