#include "utils/authvalidator.h"

bool AuthValidator::isValidUsername(const QString &username)
{
    static const QRegularExpression usernameRegex("^[a-zA-Z0-9_.]{3,32}$");
    return usernameRegex.match(username).hasMatch();
}

bool AuthValidator::isValidPassword(const QString &password)
{
    static const QRegularExpression passwordRegex("^(?=.*[A-Za-z])(?=.*[@$!%*#?&])[A-Za-z\\d@$!%*#?&]{8,}$");
    return passwordRegex.match(password).hasMatch();
}

bool AuthValidator::isValidEmail(const QString &email)
{
    static const QRegularExpression emailRegex("^[\\w.-]+@[\\w.-]+\\.[a-zA-Z]{2,}$");
    return emailRegex.match(email).hasMatch();
}

QString AuthValidator::validateUsername(const QString &username) {
    if (!isValidUsername(username))
        return "неверный формат имени пользователя";
    return "";
}

QString AuthValidator::validatePassword(const QString &password) {
    if (!isValidPassword(password))
        return "пароль должен быть минимум 8 символов, содержать буквы и "
               "спецсимволы";
    return "";
}

QString AuthValidator::validateEmail(const QString &email) {
    if (!isValidEmail(email))
        return "некорректный email";
    return "";
}

ValidationResult AuthValidator::validateLogin(const QString &username, const QString &password)
{
    QStringList emptyFields = {};

    auto addToEmpty = [&emptyFields](const QString &fieldName, const QString &fieldValue){
        if (fieldValue.isEmpty()) emptyFields.append(fieldName);
    };

    addToEmpty("имя пользователя", username);
    addToEmpty("пароль", password);

    if (emptyFields.size() > 0) {
        QString error_msg;
        switch(emptyFields.size()){
        case 1:
            error_msg = QString("%1 не может быть пустым").arg(emptyFields.at(0));
            break;
        case 2:
            error_msg = QString("%1 и %2 не могут быть пустыми")
                            .arg(emptyFields.at(0))
                            .arg(emptyFields.at(1));
            break;
        }
        return {false, error_msg};
    }

    return {true, ""};
}

ValidationResult AuthValidator::validateRegistration(const QString &username, const QString &password, const QString &email)
{
    QStringList emptyFields = {};

    auto addToEmpty = [&emptyFields](const QString &fieldName, const QString &fieldValue){
        if (fieldValue.isEmpty()) emptyFields.append(fieldName);
    };

    addToEmpty("имя пользователя", username);
    addToEmpty("пароль", password);
    addToEmpty("email", email);

    if (emptyFields.size() > 0) {
        QString error_msg;
        switch(emptyFields.size()){
        case 1:
            error_msg = QString("%1 не может быть пустым").arg(emptyFields.at(0));
            break;
        case 2:
            error_msg = QString("%1 и %2 не могут быть пустыми")
                            .arg(emptyFields.at(0))
                            .arg(emptyFields.at(1));
            break;
        case 3:
            error_msg = QString("%1, %2 и %3 не могут быть пустыми")
                            .arg(emptyFields.at(0))
                            .arg(emptyFields.at(1))
                            .arg(emptyFields.at(2));
            break;
        }
        return {false, error_msg};
    }

    QString userError = AuthValidator::validateUsername(username);
    if (!userError.isEmpty()) {
        return {false, userError};
    }

    QString emailError = AuthValidator::validateEmail(email);
    if (!emailError.isEmpty()) {
        return {false, emailError};
    }

    QString passError = AuthValidator::validatePassword(password);
    if (!passError.isEmpty()) {
        return {false, passError};
    }

    return {true, ""};
}

