#include "controller/chatpage.h"
#include "utils/logging.h"
#include <QTimer>
#include <QQmlComponent>
#include <qqmlcontext.h>

ChatPage::ChatPage(QObject *parent) {
}

void ChatPage::init()
{
    QObject* root = getRootObject();
    if (!root) {
        LOG(Logging::Critical, "ChatPage: объект root не инициализирован");
        return;
    }
    QObject* loadingPage = root->findChild<QObject*>("loadingPage");
    setTheme(true);
    connect(this, &ChatPage::connectedToChat, [loadingPage]() {
        if (loadingPage) {
            QMetaObject::invokeMethod(loadingPage, "hide");
        }
        else {
            LOG(Logging::Critical, "loadingPage не объявлен!");
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
        LOG(Logging::Critical, "Не удалось создать объект темы!");
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
