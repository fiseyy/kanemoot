#include "core/errorhandler.h"
#include "utils/logging.h"
#include "core/errorcode.h"

void ErrorHandler::init(QQmlApplicationEngine *engine) { m_engine = engine; }

void ErrorHandler::showError(const QString &title, const QString &message) {
    // if (!m_engine || m_engine->rootObjects().isEmpty()) {
    //     // LOG(Logging::Critical, "ErrorHandler: engine не инициализирован или объекты root пусты");
    //     return;
    // }
    if(!m_engine) {
        LOG(Logging::Fatal, ErrorCode::make(ErrorCode::UI, 0x01, ErrorCode::ErrorHandler), "");
    }
    if(m_engine->rootObjects().isEmpty()) {
        LOG(Logging::Fatal, ErrorCode::make(ErrorCode::UI, 0x06, ErrorCode::ErrorHandler), "");
    }
    const QList<QObject *> rootObjects = m_engine->rootObjects();
    QObject *rootObj = rootObjects.first();
    if (!rootObj) {
        // LOG(Logging::Warning, "ErrorHandler: объект root не иницализирован");
        LOG(Logging::Fatal, ErrorCode::make(ErrorCode::UI, 0x06, ErrorCode::ErrorHandler), "");
        return;
    }

    bool invoked = QMetaObject::invokeMethod(rootObj, "handleError",
                                             Q_ARG(QVariant, QVariant(title)),
                                             Q_ARG(QVariant, QVariant(message)));

    if (!invoked) {
        // LOG(Logging::Critical, "ErrorHandler: не удалось вызвать handleError");
        LOG(Logging::Fatal, ErrorCode::make(ErrorCode::UI, 0x09, ErrorCode::ErrorHandler), "Не удалось вызвать handleError");
    }
}

ErrorHandler::ErrorHandler(QObject *parent)
    : QObject(parent), m_engine(nullptr) {}
