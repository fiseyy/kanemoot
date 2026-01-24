#include "utils/keycatcher.h"
#include "core/apiendpoints.h"
#include <QUrl>
bool KeyFilter::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::KeyPress) {
        auto *keyEvent = static_cast<QKeyEvent *>(event);

        if (keyEvent->isAutoRepeat())
            return false;

        qDebug() << "Global key:" << keyEvent->key();
        if(keyEvent->key() == 16777301) {
            //DEBUG FEATURE -- смена ApiEndpoints
            qDebug() << "DEBUG FEATURE -- смена ApiEndpoints";
            ApiEndpoints::instance().setEndpoint("auth", QUrl("wss://auth-service.kanemoot.ru/ws"));
            ApiEndpoints::instance().setEndpoint("chat", QUrl("wss://chat-service.kanemoot.ru/ws"));
        }
        emit keyPressed(keyEvent->key());
        return false;
    }
    return QObject::eventFilter(obj, event);
}
