#ifndef LOGINPAGE_H
#define LOGINPAGE_H

#include <QObject>
#include <QQmlApplicationEngine>
#include <QCoreApplication>
#include "controller/basepage.h"

class LoginPage : public BasePage {
    Q_OBJECT
public:
    explicit LoginPage(QObject* parent = nullptr);

    void init() override;
    void cleanup() override;

signals:
    void loginSuccessful();
    void loginFailed(const QString& error);

public slots:
    void onLoginButtonClicked(const QString& username, const QString& password);
};

#endif // LOGINPAGE_H
