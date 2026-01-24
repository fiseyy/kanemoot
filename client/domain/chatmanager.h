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
    void connectToChat();
    void sendMessage(const QString &text);
    bool isConnected() const;
    void requestUserServers();

    void joinServer(const QString &inviteLink);
    void createServer(const QString &name);

    void createChannel(int serverId, const QString &name, const QString &type);
    void deleteChannel(int channelId, int serverId);

    Q_INVOKABLE void getMessages(int channelId);
    Q_INVOKABLE void sendChatMessage(int guildId, int channelId, const QString &text);

signals:
    void connected();
    void disconnected();
    void messageReceived(const QString &text);
    void newMessageReceived(const QVariantMap &msg);
    void errorOccurred(const QString &error);
    void userServersReceived(const QJsonArray &servers);

public slots:
    void onEndpointChanged(const QString &service, const QUrl &newUrl);

private slots:
    void onConnected();
    void onDisconnected();
    void onMessageReceived(const QString &text);

private:
    void connectToServer(const QUrl &url);
    WebSocketClient *m_socket;
    QQueue<QString> m_pendingMessages;
};

#endif // CHATMANAGER_H
