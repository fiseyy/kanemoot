#ifndef WEBSOCKETCLIENT_H
#define WEBSOCKETCLIENT_H

#include <QObject>
#include <QtWebSockets/QWebSocket>
#include <QSslConfiguration>

class WebSocketClient : public QObject {
    Q_OBJECT

public:
    explicit WebSocketClient(QObject *parent = nullptr);
    ~WebSocketClient();
    void connectToServer(const QUrl &url);
    void sendMessage(const QString &message);
    void disconnect();

signals:
    void connected();
    void disconnected();
    void messageReceived(const QString &message);
    void errorOccurred(const QString &error);

private:
    QWebSocket m_webSocket;
};


#endif // WEBSOCKETCLIENT_H
