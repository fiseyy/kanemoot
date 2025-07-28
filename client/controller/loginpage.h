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

    QString qmlPath() const override;

private:
    void fail(const QString& error);

signals:
    void loginSuccessful();

public slots:
    void onLoginButtonClicked(const QString& username, const QString& password);
    void regRedirect();
    void chatRedirect();
};

#endif // LOGINPAGE_H
