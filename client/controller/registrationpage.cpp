#include "controller/registrationpage.h"
#include "core/errorhandler.h"
#include "utils/logging.h"
#include "utils/authvalidator.h"

RegistrationPage::RegistrationPage(QObject *parent) {}

void RegistrationPage::init()
{
    QObject *root = getRootObject();

    if (!root) {
        LOG(Logging::Critical, "RegistrationPage: объект root не инициализован");
        return;
    }

    connect(root, SIGNAL(loginRedirectRequested()), this, SLOT(loginRedirect()));
    connect(root, SIGNAL(regAttemped(QString, QString, QString)), this, SLOT(onRegAttemped(QString, QString, QString)));
    connect(this, SIGNAL(regSuccessful()), this, SLOT(chatRedirect()));
}

void RegistrationPage::cleanup()
{

}

QString RegistrationPage::qmlPath() const
{
    return "qrc:/kanemoot/ui/pages/RegistrationPage.qml";
}

void RegistrationPage::fail(const QString &error)
{
    LOG(Logging::Debug, "Не удалось зарегистрировать аккаунт: " + error);
    ErrorHandler::instance().showError("Ошибка", "Не удалось зарегистрировать аккаунт: " + error);
}

void RegistrationPage::onRegAttemped(const QString &username, const QString &password, const QString &email)
{
    auto validator_result = AuthValidator::validateRegistration(username, password, email);
    if (!validator_result.valid) {
        fail(validator_result.errorMessage);
        return;
    }

    emit regSuccessful();
}

void RegistrationPage::loginRedirect()
{
    emit requestPageChange(Page_Login);
}

void RegistrationPage::chatRedirect()
{
    emit requestPageChange(Page_Chat);
}
