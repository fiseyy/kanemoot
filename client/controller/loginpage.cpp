#include <QDebug>
#include "controller/loginpage.h"

LoginPage::LoginPage(QObject *parent)
{

}

void LoginPage::init()
{
    QObject *root = getRootObject();

    if (!root) {
        qWarning() << "LoginPage: root object is null!";
        return;
    }

    connect(root, SIGNAL(loginAttempted(QString, QString)),
            this, SLOT(onLoginButtonClicked(QString, QString)));
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
