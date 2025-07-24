#include "loginpage.h"

LoginPage::LoginPage(QObject *parent)
{

}

void LoginPage::init()
{

}

void LoginPage::cleanup()
{

}

void LoginPage::onLoginButtonClicked(const QString &username, const QString &password)
{
    // emit loginSuccessful()
    // emit loginFailed(msg)
    emit loginSuccessful();
}
