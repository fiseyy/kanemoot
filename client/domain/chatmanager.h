#ifndef CHATMANAGER_H
#define CHATMANAGER_H

#include <QObject>
#include <QString>
#include <QQueue>
#include "network/websocketclient.h"

class ChatManager : public QObject {
    Q_OBJECT
public:
    explicit ChatManager(QObject *parent = nullptr);
    void connectToServer(const QUrl &url);
    void sendMessage(const QString &text);

signals:
    void connected();
    void disconnected();
    void messageReceived(const QString &text);
    void errorOccurred(const QString &error);

private slots:
    void onConnected();
    void onDisconnected();
    void onMessageReceived(const QString &text);

private:
    WebSocketClient *m_socket;
    QQueue<QString> m_pendingMessages;
};

#endif // CHATMANAGER_H
