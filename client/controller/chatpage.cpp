#include "controller/chatpage.h"
#include "utils/logging.h"

ChatPage::ChatPage(QObject *parent) {
}

void ChatPage::init()
{
    QObject *root = getRootObject();

    if (!root) {
        LOG(Logging::Critical, "RegistrationPage: объект root не инициализован");
        return;
    }
}

void ChatPage::cleanup()
{

}

QString ChatPage::qmlPath() const
{
    return "qrc:/kanemoot/ui/pages/ChatPage.qml";
}
