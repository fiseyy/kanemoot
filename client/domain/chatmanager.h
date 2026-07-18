#ifndef CHATMANAGER_H
#define CHATMANAGER_H

#include <QObject>
#include <QString>
#include <QQueue>
#include <QJsonObject>
#include <QPointer>
#include <QTimer>
#include "network/websocketclient.h"

/*
 * Тип сообщения, ожидающего отправки
 * Нужен для управления очередью сообщений при отсутствии подключения
 */
struct PendingChatMessage {
    QJsonObject payload;
};

class ChatManager : public QObject {
    Q_OBJECT
public:
    explicit ChatManager(QObject* parent = nullptr);

    // === Публичный API (UI / AppController) ===
    void connectToChat();
    bool isConnected() const;

    void requestUserServers();
    void joinServer(const QString &inviteLink);
    void createServer(const QString &name);
    void createChannel(int serverId, const QString &name, const QString &type);
    void deleteChannel(int channelId, int serverId);
    Q_INVOKABLE void getMessages(int channelId, int guildId);
    Q_INVOKABLE void sendChatMessage(int guildId, int channelId, const QString &text);

signals:
    // Контракт с UI / AppController
    void connected();
    void disconnected();
    void messageReceived(const QString &text);
    void newMessageReceived(const QVariantMap &msg);
    void errorOccurred(const QString &error);
    void userServersReceived(const QJsonArray &servers);
    void jwtExpired();

private slots:
    // WebSocketClient события
    void onSocketConnected();
    void onSocketDisconnected(DisconnectReason reason);
    void onSocketError(const QString &error);
    void onMessageReceived(const QString &text);

    // ApiEndpoints
    void onEndpointChanged(const QString &service, const QUrl &newUrl);

private:
    // === helpers ===
    void sendPendingMessages();
    bool addJwtToObject(QJsonObject &obj, const QString &action);
    QVariantMap jsonObjectToVariantMap(const QJsonObject &obj);

    void enqueueMessage(const QJsonObject &obj);

    // === данные ===
    QPointer<WebSocketClient> m_socket = nullptr;
    QQueue<PendingChatMessage> m_pendingMessages;
    QPointer<QTimer> m_pingTimer;
};

#endif // CHATMANAGER_H
