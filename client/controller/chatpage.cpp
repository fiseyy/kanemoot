#include "controller/chatpage.h"
#include "utils/logging.h"
#include "core/errorcode.h"
#include "core/apiendpoints.h"
#include "core/securestorage.h"
#include "core/errorhandler.h"
#include <QQmlComponent>
#include <qqmlcontext.h>

ChatPage::ChatPage(QObject *parent) {
    m_chatmgr = new ChatManager(this);
    m_reconnectTimer = new QTimer(this);
    m_reconnectTimer->setInterval(2000);
    m_reconnectTimer->setSingleShot(false);
    connect(m_reconnectTimer, &QTimer::timeout, [this]() {
        if (!m_chatmgr->isConnected()) {
            Logging::instance().log(Logging::Debug, "Пробуем переподключиться к Chat Service...");
            m_chatmgr->connectToChat();
        }
    });

    connect(m_chatmgr, &ChatManager::connected, [this]() {
        m_reconnectTimer->stop();
        if(m_showingLoading)
            m_showingLoading = false;
            emit connectedToChat();
    });
    connect(m_chatmgr, &ChatManager::disconnected, [this]() {
        Logging::instance().log(Logging::Debug, "Отключено от Chat Service.");

        if (!m_showingLoading) {
            QObject* root = getRootObject();
            if (!root) return;
            QObject* loadingPage = root->findChild<QObject*>("loadingPage");
            if (loadingPage) {
                QMetaObject::invokeMethod(loadingPage, "show");
                m_showingLoading = true;
                ErrorHandler::instance().setHiden(true);
            }
        }

        if (!m_reconnectTimer->isActive()) m_reconnectTimer->start();
    });
}

void ChatPage::init()
{
    QObject* root = getRootObject();
    if (!root) {
        LOG(Logging::Critical, ErrorCode::make(ErrorCode::UI, 0x06, ErrorCode::ChatPage), "");
        return;
    }
    QObject* loadingPage = root->findChild<QObject*>("loadingPage");
    setTheme(false);
    ErrorHandler::instance().setHiden(true);
    connect(this, &ChatPage::connectedToChat, [loadingPage]() {
        ErrorHandler::instance().setHiden(false);
        if (loadingPage) {
            QMetaObject::invokeMethod(loadingPage, "hide");
        }
        else {
            LOG(Logging::Critical, ErrorCode::make(ErrorCode::UI, 0x0A, ErrorCode::ChatPage), "loadingPage не объявлен");
        }
    });
    m_chatmgr->connectToChat();
    QString username;
    auto optUsername = SecureStorage::instance().getValue("username");
    if(!optUsername) LOG(Logging::Fatal, ErrorCode::make(ErrorCode::System, 0x02, ErrorCode::ChatPage), "");
    else {
        username = optUsername.value();
    }
    root->setProperty("username", username);
}


void ChatPage::cleanup()
{

}

void ChatPage::setTheme(bool useLightTheme) {
    QObject* rootItem = getRootObject();
    if (!rootItem) return;

    QQmlEngine* engine = QQmlEngine::contextForObject(rootItem)->engine();

    QQmlComponent component(engine, QUrl(useLightTheme
                                             ? "qrc:/kanemoot/ui/themes/Light.qml"
                                             : "qrc:/kanemoot/ui/themes/Dark.qml"));
    QObject* theme = component.create();
    if (!theme) {
        // LOG(Logging::Critical, "Не удалось создать объект темы!");
        LOG(Logging::Critical, ErrorCode::make(ErrorCode::UI, 0x05, ErrorCode::ChatPage), "");
        return;
    }

    rootItem->setProperty("currentTheme", QVariant::fromValue(theme));
}

QObject* ChatPage::currentTheme() const {
    QObject* rootItem = getRootObject();
    if (!rootItem) return nullptr;
    return rootItem->property("currentTheme").value<QObject*>();
}


QString ChatPage::qmlPath() const
{
    return "qrc:/kanemoot/ui/pages/ChatPage.qml";
}
