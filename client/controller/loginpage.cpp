#include "controller/loginpage.h"
#include "core/errorhandler.h"
#include "utils/logging.h"
#include "utils/authvalidator.h"

LoginPage::LoginPage(QObject *parent) {}

void LoginPage::init()
{
    QObject *root = getRootObject();

    if (!root) {
        qWarning() << "LoginPage: root object is null!";
        return;
    }

    connect(root, SIGNAL(loginAttempted(QString,QString)), this, SLOT(onLoginButtonClicked(QString,QString)));
    connect(root, SIGNAL(regRedirectRequested()), this, SLOT(regRedirect()));
    connect(this, SIGNAL(loginSuccessful()), this, SLOT(chatRedirect()));
}

void LoginPage::cleanup()
{

}

QString LoginPage::qmlPath() const {
    return "qrc:/kanemoot/ui/pages/LoginPage.qml";
}

void LoginPage::fail(const QString &error)
{
    LOG(Logging::Debug, "Не удалось войти в аккаунт: " + error);
    ErrorHandler::instance().showError("Ошибка", "Не удалось войти в аккаунт: " + error);
}

void LoginPage::onLoginButtonClicked(const QString &username, const QString &password)
{
    auto validator_result = AuthValidator::validateLogin(username, password);
    if (!validator_result.valid) {
        fail(validator_result.errorMessage);
        return;
    }

    emit loginSuccessful();
}


void LoginPage::regRedirect()
{
    emit requestPageChange(Page_Register);
}

void LoginPage::chatRedirect()
{
    emit requestPageChange(Page_Chat);
}
