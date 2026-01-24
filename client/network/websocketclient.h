#ifndef WEBSOCKETCLIENT_H
#define WEBSOCKETCLIENT_H

#include <QObject>
#include <QtWebSockets/QWebSocket>
#include <QSslConfiguration>

class WebSocketClient : public QObject {
    Q_OBJECT

public:
    explicit WebSocketClient(QString serviceName = "", QObject *parent = nullptr);
    ~WebSocketClient();
    void connectToServer(const QUrl &url);
    void close();
    void sendMessage(const QString &message);
    QAbstractSocket::SocketState getState();
    void ping();
    bool isConnected() const;
    void reconnect(const QUrl &url);

public slots:
    void onReconnectSlot();

private slots:
    void onSocketError(QAbstractSocket::SocketError error);

signals:
    void connected();
    void disconnected();
    void messageReceived(const QString &message);
    void errorOccurred(const QString &error);

private:
    QWebSocket* m_webSocket;
    QString m_serviceName;
    QUrl m_pendingUrl;
    void connectSignals();
    void createAndConnect(const QUrl &url);
};


#endif // WEBSOCKETCLIENT_H
