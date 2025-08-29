#ifndef CHATPAGE_H
#define CHATPAGE_H

#include <QObject>
#include "controller/basepage.h"

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
};

#endif // CHATPAGE_H
