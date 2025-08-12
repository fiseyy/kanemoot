#ifndef REGISTRATIONPAGE_H
#define REGISTRATIONPAGE_H

#include <QObject>
#include "controller/basepage.h"
#include "domain/authmanager.h"

class RegistrationPage : public BasePage {
    Q_OBJECT
public:
    explicit RegistrationPage(QObject* parent = nullptr);

    void init() override;
    void cleanup() override;

    QString qmlPath() const override;

private:
    void fail(const QString& error);
    AuthManager* m_authmgr = nullptr;

signals:
    void regSuccessful();

public slots:
    void onRegAttemped(const QString& username, const QString& password, const QString& email);
    void loginRedirect();
    void chatRedirect();
};

#endif // REGISTRATIONPAGE_H
