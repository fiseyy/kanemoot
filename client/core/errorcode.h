#ifndef ERRORCODE_H
#define ERRORCODE_H

#include <cstdint>
#include <QString>
#include <QMap>
class ErrorCode {
public:
    enum Category : uint8_t { UI = 0x01, Account = 0x02, System = 0x03, Network = 0x04 };
    enum Module : uint8_t { AppController = 0x01, ChatPage = 0x02, LoginPage = 0x03, RegistrationPage = 0x04,
                            ApiEndpoints = 0x05, App = 0x06, ErrorHandler = 0x07, SecureStorage = 0x08,
                            SettingsManager = 0x09, SystemFatalError = 0x0A, AuthManager = 0x0B,
                            WebSocketClient = 0x0C, Logging = 0x0D, ChatManager = 0x0E };

    static constexpr uint32_t make(Category cat, uint8_t errorCode, Module mod, uint8_t flag = 0x00) {
        return (uint32_t(cat) << 24) | (uint32_t(errorCode) << 16) | (uint32_t(mod) << 8) | flag;
    }

    static inline const QMap<uint8_t, QString> categoryNames {
        {0x01, "UI"},
        {0x02, "Account"},
        {0x03, "System"},
        {0x04, "Network"}
    };

    static inline const QMap<uint8_t, QString> moduleNames {
        {0x01, "AppController"},
        {0x02, "ChatPage"},
        {0x03, "LoginPage"},
        {0x04, "RegistrationPage"},
        {0x05, "ApiEndpoints"},
        {0x06, "App"},
        {0x07, "ErrorHandler"},
        {0x08, "SecureStorage"},
        {0x09, "SettingsManager"},
        {0x0A, "SystemFatalError"},
        {0x0B, "AuthManager"},
        {0x0C, "WebSocketClient"},
        {0x0D, "Logging"},
        {0x0E, "ChatManager"}
    };

    static inline const QMap<uint16_t, QString> errorMessages {
        // UI (AA=0x01)
        {0x0101, "Ошибка с QML Engine"},
        {0x0102, "Ошибка компонента QML"},
        {0x0103, "Не удалось создать QML-страницу"},
        {0x0104, "Недопустимый объект страницы или окна"},
        {0x0105, "Не удалось создать объект темы"},
        {0x0106, "Объект root не инициализирован"},
        {0x0107, "QML не загружен"},
        {0x0108, "Не удалось загрузить QML"},
        {0x0109, "Не удалось вызвать сигнал QML"},
        {0x010A, "Объект, необходимый для работы UI, не инициализирован"},

        // Account (AA=0x02)
        {0x0201, "Неверный пароль"},
        {0x0202, "Пользователь заблокирован"},
        {0x0203, "Аккаунт не найден"},
        {0x0204, "Имя пользователя занято"},
        {0x0205, "Почта занята"},

        // System (AA=0x03)
        {0x0301, "Ошибка инициализации незащищённого хранилища"},
        {0x0302, "Ошибка инициализации защищённого хранилища"},
        {0x0303, "Не удалось открыть файл лога для записи"},
        {0x0304, "Не удалось переименовать файл"},

        // Network (AA=0x04)
        {0x0401, "Нет связи с сервером"},
        {0x0402, "Неверный формат ответа сервера"},
        {0x0403, "Подключение уже выполняется"},
        {0x0404, "Сервер временно недоступен"},
        {0x0405, "Ошибка шифрования"},
        {0x0406, "Неизвестный сервис"}
    };


    static QString decode(uint32_t code);
};


#endif // ERRORCODE_H
