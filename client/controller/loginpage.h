#ifndef LOGINPAGE_H
#define LOGINPAGE_H

#include <QObject>
#include <QQmlApplicationEngine>
#include <QCoreApplication>
#include "controller/basepage.h"
#include "domain/authmanager.h"

class LoginPage : public BasePage {
    Q_OBJECT
public:
    explicit LoginPage(QObject* parent = nullptr);

    void init() override;
    void cleanup() override;
    void tryAutoLogIn(const QString& jwt);
    QString qmlPath() const override;

private:
    void fail(const QString& error);
    AuthManager* m_authmgr = nullptr;

signals:
    void loginSuccessful();
    void loginFailed(const QString& error);
public slots:
    void onLoginButtonClicked(const QString& username, const QString& password);
    void regRedirect();
    void chatRedirect();
};

#endif // LOGINPAGE_H
