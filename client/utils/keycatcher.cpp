#include "utils/keycatcher.h"

bool KeyFilter::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::KeyPress) {
        auto *keyEvent = static_cast<QKeyEvent *>(event);

        if (keyEvent->isAutoRepeat())
            return false; // игнорируем автоповтор

        qDebug() << "Global key:" << keyEvent->key();
        emit keyPressed(keyEvent->key());
        return false; // оставляем событие живым
    }
    return QObject::eventFilter(obj, event);
}
