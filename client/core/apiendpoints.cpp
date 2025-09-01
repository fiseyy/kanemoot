#include "core/apiendpoints.h"
#include "utils/logging.h"
#include "core/errorcode.h"

ApiEndpoints &ApiEndpoints::instance() {
    static ApiEndpoints instance;
    return instance;
}

ApiEndpoints::ApiEndpoints(QObject *parent) : QObject(parent) {
    initializeDefaultEndpoints();
}

void ApiEndpoints::initializeDefaultEndpoints() {
    m_endpoints["auth"] = QUrl("wss://auth-service.kanemoot.ru/ws");
    m_endpoints["chat"] = QUrl("wss://chat-service.kanemoot.ru/ws");
    m_endpoints["dm"] = QUrl("wss://dm-service.kanemoot.ru/ws");
}

QUrl ApiEndpoints::getEndpoint(const QString &service) const {
    if (m_endpoints.contains(service)) {
        return m_endpoints[service];
    }
    // LOG(Logging::Warning, QString("Неизвестный сервис: %1").arg(service));
    LOG(Logging::Warning, ErrorCode::make(ErrorCode::Network, 0x06, ErrorCode::ApiEndpoints), "");
    return QUrl();
}

QString ApiEndpoints::getServiceName(const QUrl &url) const {
    for (auto it = m_endpoints.begin(); it != m_endpoints.end(); ++it) {
        if (it.value() == url) {
            return it.key();
        }
    }
    return "unknown";
}
