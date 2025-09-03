#include "controller/chatpage.h"
#include "utils/logging.h"
#include "core/errorcode.h"
#include "core/apiendpoints.h"
#include <QTimer>
#include <QQmlComponent>
#include <qqmlcontext.h>

ChatPage::ChatPage(QObject *parent) {
    m_chatmgr = new ChatManager(this);
    connect(m_chatmgr, &ChatManager::connected, this, &ChatPage::connectedToChat);
    connect(m_chatmgr, &ChatManager::disconnected, this, [](){
        Logging::instance().log(Logging::Debug, "Отключено от Chat Service.");
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
    connect(this, &ChatPage::connectedToChat, [loadingPage]() {
        if (loadingPage) {
            QMetaObject::invokeMethod(loadingPage, "hide");
        }
        else {
            LOG(Logging::Critical, ErrorCode::make(ErrorCode::UI, 0x0A, ErrorCode::ChatPage), "loadingPage не объявлен");
        }
    });

    m_chatmgr->connectToServer(ApiEndpoints::instance().getEndpoint("chat"));
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
