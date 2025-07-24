#include "errorhandler.h"

void ErrorHandler::init(QQmlApplicationEngine *engine) { m_engine = engine; }

void ErrorHandler::showError(const QString &title, const QString &message) {
    if (!m_engine || m_engine->rootObjects().isEmpty()) {
        qWarning() << "ErrorHandler: engine not initialized or rootObjects empty";
        return;
    }

    QObject *rootObj = m_engine->rootObjects().first();
    if (!rootObj) {
        qWarning() << "ErrorHandler: root object is null";
        return;
    }

    bool invoked = QMetaObject::invokeMethod(rootObj, "handleError",
                                             Q_ARG(QVariant, QVariant(title)),
                                             Q_ARG(QVariant, QVariant(message)));

    if (!invoked) {
        qWarning() << "ErrorHandler: failed to invoke handleError";
    }
}

ErrorHandler::ErrorHandler(QObject *parent)
    : QObject(parent), m_engine(nullptr) {}
