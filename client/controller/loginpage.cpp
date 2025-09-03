#include "controller/loginpage.h"
#include "core/errorhandler.h"
#include "utils/logging.h"
#include "utils/authvalidator.h"
#include "core/errorcode.h"
#include "core/securestorage.h"

LoginPage::LoginPage(QObject *parent) {
    m_authmgr = new AuthManager(this);

    connect(m_authmgr, &AuthManager::authSucceeded, this, &LoginPage::loginSuccessful);
    connect(m_authmgr, &AuthManager::authFailed, this, [this](const QString& error){
        fail(error);
    });
}

void LoginPage::init()
{
    QObject *root = getRootObject();

    if (!root) {
        // LOG(Logging::Critical, "LoginPage: объект root не инициализован");
        LOG(Logging::Critical, ErrorCode::make(ErrorCode::UI, 0x06, ErrorCode::LoginPage), "");
        return;
    }

    connect(root, SIGNAL(loginAttempted(QString,QString)), this, SLOT(onLoginButtonClicked(QString,QString)));
    connect(root, SIGNAL(regRedirectRequested()), this, SLOT(regRedirect()));
    connect(this, SIGNAL(loginSuccessful()), this, SLOT(chatRedirect()));

}

void LoginPage::cleanup()
{

}

void LoginPage::tryAutoLogIn(const QString &jwt)
{
    m_authmgr->tryAutoLogin(jwt);
}

QString LoginPage::qmlPath() const {
    return "qrc:/kanemoot/ui/pages/LoginPage.qml";
}

void LoginPage::fail(const QString &error)
{
    Logging::instance().log(Logging::Debug, "Не удалось войти в аккаунт: " + error);
    ErrorHandler::instance().showError("Ошибка", "Не удалось войти в аккаунт: " + error);
}

void LoginPage::onLoginButtonClicked(const QString &username, const QString &password)
{
    auto validator_result = AuthValidator::validateLogin(username, password);
    if (!validator_result.valid) {
        fail(validator_result.errorMessage);
        return;
    }
    m_authmgr->tryAuth(username, password);
}


void LoginPage::regRedirect()
{
    emit requestPageChange(Page_Register);
}

void LoginPage::chatRedirect()
{
    emit requestPageChange(Page_Chat);
}
