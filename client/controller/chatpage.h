#ifndef CHATPAGE_H
#define CHATPAGE_H

#include <QObject>
#include <QTimer>
#include "controller/basepage.h"
#include "domain/chatmanager.h"

class ChatPage : public BasePage {
    Q_OBJECT
public:
    explicit ChatPage(QObject* parent = nullptr);

    void init() override;
    void cleanup() override;

    QString qmlPath() const override;
signals:
    void connectedToChat();
private:
    void fail(const QString& error);

    void setTheme(bool useLightTheme);
    QObject *currentTheme() const;
    bool m_showingLoading = true;
    QTimer* m_reconnectTimer = nullptr;
    ChatManager* m_chatmgr = nullptr;
};

#endif // CHATPAGE_H
