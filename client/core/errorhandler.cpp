#include "core/errorhandler.h"
#include "utils/logging.h"
#include "core/errorcode.h"

void ErrorHandler::init(QQmlApplicationEngine *engine) { m_engine = engine; }

void ErrorHandler::showError(const QString &title, const QString &message) {
    if (!hiden) {
        if(!m_engine) {
            LOG(Logging::Fatal, ErrorCode::make(ErrorCode::UI, 0x01, ErrorCode::ErrorHandler), "");
        }
        if(m_engine->rootObjects().isEmpty()) {
            LOG(Logging::Fatal, ErrorCode::make(ErrorCode::UI, 0x06, ErrorCode::ErrorHandler), "");
        }
        const QList<QObject *> rootObjects = m_engine->rootObjects();
        QObject *rootObj = rootObjects.first();
        if (!rootObj) {
            LOG(Logging::Fatal, ErrorCode::make(ErrorCode::UI, 0x06, ErrorCode::ErrorHandler), "");
            return;
        }

        bool invoked = QMetaObject::invokeMethod(rootObj, "handleError",
                                                 Q_ARG(QVariant, QVariant(title)),
                                                 Q_ARG(QVariant, QVariant(message)));

        if (!invoked) {
            LOG(Logging::Fatal, ErrorCode::make(ErrorCode::UI, 0x09, ErrorCode::ErrorHandler), "Не удалось вызвать handleError");
        }
    }
}

void ErrorHandler::setHiden(bool m_hiden)
{
    this->hiden = m_hiden;
}

ErrorHandler::ErrorHandler(QObject *parent)
    : QObject(parent), m_engine(nullptr) {}
