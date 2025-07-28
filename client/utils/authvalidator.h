#ifndef AUTHVALIDATOR_H
#define AUTHVALIDATOR_H

#include <QString>
#include <QRegularExpression>

struct ValidationResult {
    bool valid;
    QString errorMessage = "";
};

class AuthValidator {
public:
    static bool isValidUsername(const QString& username);
    static bool isValidPassword(const QString& password);
    static bool isValidEmail(const QString& email);

    static QString validateUsername(const QString& username);
    static QString validatePassword(const QString &password);
    static QString validateEmail(const QString &email);

    static ValidationResult validateLogin(const QString& username, const QString& password);
    static ValidationResult validateRegistration(const QString& username, const QString& password, const QString& email);
};

#endif // AUTHVALIDATOR_H
