#include "controller/chatpage.h"
#include "utils/logging.h"
#include "core/errorcode.h"
#include <QTimer>
#include <QQmlComponent>
#include <qqmlcontext.h>

ChatPage::ChatPage(QObject *parent) {
}

void ChatPage::init()
{
    QObject* root = getRootObject();
    if (!root) {
        // LOG(Logging::Critical, "ChatPage: объект root не инициализирован");
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
            // LOG(Logging::Critical, "loadingPage не объявлен!");
            LOG(Logging::Critical, ErrorCode::make(ErrorCode::UI, 0x0A, ErrorCode::ChatPage), "loadingPage не объявлен");
        }
    });

    QTimer::singleShot(5000, [this]() { emit connectedToChat(); });
}


void ChatPage::cleanup()
{

}

// Устанавливаем тему
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

// Получаем текущую тему
QObject* ChatPage::currentTheme() const {
    QObject* rootItem = getRootObject();
    if (!rootItem) return nullptr;
    return rootItem->property("currentTheme").value<QObject*>();
}


QString ChatPage::qmlPath() const
{
    return "qrc:/kanemoot/ui/pages/ChatPage.qml";
}
