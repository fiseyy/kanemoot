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

private:
    void fail(const QString& error);

};

#endif // CHATPAGE_H
