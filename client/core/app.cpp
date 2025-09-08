#include <QQuickStyle>
#include "core/app.h"
#include "core/errorhandler.h"
#include "core/settingsmanager.h"
#include "core/securestorage.h"
#include "utils/logging.h"
#include "core/errorcode.h"
App::App(QObject *parent)
{

}

void App::init()
{
    QQuickStyle::setStyle("Fusion");

    QCoreApplication::setOrganizationName("KANEMOOT");
    QCoreApplication::setApplicationName("KANEMOOT Messenger");

    setupLogging();
    setupDependencies();
}

void testLogs() {
    // UI
    LOG(Logging::Critical, ErrorCode::make(ErrorCode::UI, 0x01, ErrorCode::App), "");
    LOG(Logging::Critical, ErrorCode::make(ErrorCode::UI, 0x02, ErrorCode::App), "");
    LOG(Logging::Critical, ErrorCode::make(ErrorCode::UI, 0x03, ErrorCode::App), "");
    LOG(Logging::Critical, ErrorCode::make(ErrorCode::UI, 0x04, ErrorCode::App), "");
    LOG(Logging::Critical, ErrorCode::make(ErrorCode::UI, 0x05, ErrorCode::App), "");
    LOG(Logging::Critical, ErrorCode::make(ErrorCode::UI, 0x06, ErrorCode::App), "");
    LOG(Logging::Critical, ErrorCode::make(ErrorCode::UI, 0x07, ErrorCode::App), "");
    LOG(Logging::Critical, ErrorCode::make(ErrorCode::UI, 0x08, ErrorCode::App), "");
    LOG(Logging::Critical, ErrorCode::make(ErrorCode::UI, 0x09, ErrorCode::App), "");
    LOG(Logging::Critical, ErrorCode::make(ErrorCode::UI, 0x0A, ErrorCode::App), "");
    //Account
    LOG(Logging::Critical, ErrorCode::make(ErrorCode::Account, 0x01, ErrorCode::App), "");
    LOG(Logging::Critical, ErrorCode::make(ErrorCode::Account, 0x02, ErrorCode::App), "");
    LOG(Logging::Critical, ErrorCode::make(ErrorCode::Account, 0x03, ErrorCode::App), "");
    LOG(Logging::Critical, ErrorCode::make(ErrorCode::Account, 0x04, ErrorCode::App), "");
    LOG(Logging::Critical, ErrorCode::make(ErrorCode::Account, 0x05, ErrorCode::App), "");
    //System
    LOG(Logging::Critical, ErrorCode::make(ErrorCode::System, 0x01, ErrorCode::App), "");
    LOG(Logging::Critical, ErrorCode::make(ErrorCode::System, 0x02, ErrorCode::App), "");
    LOG(Logging::Critical, ErrorCode::make(ErrorCode::System, 0x03, ErrorCode::App), "");
    LOG(Logging::Critical, ErrorCode::make(ErrorCode::System, 0x04, ErrorCode::App), "");
    //Network
    LOG(Logging::Critical, ErrorCode::make(ErrorCode::Network, 0x01, ErrorCode::App), "");
    LOG(Logging::Critical, ErrorCode::make(ErrorCode::Network, 0x02, ErrorCode::App), "");
    LOG(Logging::Critical, ErrorCode::make(ErrorCode::Network, 0x03, ErrorCode::App), "");
    LOG(Logging::Critical, ErrorCode::make(ErrorCode::Network, 0x04, ErrorCode::App), "");
    LOG(Logging::Critical, ErrorCode::make(ErrorCode::Network, 0x05, ErrorCode::App), "");
    LOG(Logging::Critical, ErrorCode::make(ErrorCode::Network, 0x06, ErrorCode::App), "");

    Logging::instance().log(Logging::Info, "Завершение работы тестера логов");
}

void App::run()
{
    engine = new QQmlApplicationEngine(this);

    SettingsManager &settings = SettingsManager::instance();
    qmlRegisterSingletonInstance<SettingsManager>("Core", 1, 0, "SettingsManager", &settings);

    appController = new AppController(this, engine);

    engine->rootContext()->setContextProperty("appController", appController);
    engine->load(QUrl(QStringLiteral("qrc:/kanemoot/ui/App.qml")));
    if (engine->rootObjects().isEmpty()) {
        // LOG(Logging::Critical, "Не удалось загрузить QML");
        LOG(Logging::Critical, ErrorCode::make(ErrorCode::UI, 0x06, ErrorCode::App), "");
        qFatal("Failed to load QML");
    }
    setTheme(true);
    appController->start();
    ErrorHandler::instance().init(engine);
}

QQmlApplicationEngine *App::getEngine() const { return engine; }

void App::setTheme(bool useLightTheme) {
    if (!engine || engine->rootObjects().isEmpty()) {
        // LOG(Logging::Warning, "Не удалось установить тему: QML не загружен");
        LOG(Logging::Warning, ErrorCode::make(ErrorCode::UI, 0x01, ErrorCode::App), "Не удалось установить тему: QML не загружен");
        return;
    }

    QObject* rootItem = engine->rootObjects().first();
    if (!rootItem) return;

    QObject* theme = nullptr;
    if (useLightTheme) {
        QQmlComponent component(engine, QUrl("qrc:/kanemoot/ui/themes/Light.qml"));
        theme = component.create();
    } else {
        QQmlComponent component(engine, QUrl("qrc:/kanemoot/ui/themes/Dark.qml"));
        theme = component.create();
    }

    if (!theme) {
        // LOG(Logging::Critical, "Не удалось создать объект темы!");
        LOG(Logging::Critical, ErrorCode::make(ErrorCode::UI, 0x05, ErrorCode::App), "");
        return;
    }

    rootItem->setProperty("currentTheme", QVariant::fromValue(theme));
}


void App::setupLogging()
{
    SET_LOG_LEVEL(Logging::Debug);
}

void App::setupDependencies()
{
    setupSettings();
    setupSecureStorage();
    qmlRegisterSingletonType(QUrl("qrc:/kanemoot/ui/themes/Dark.qml"), "Themes", 1, 0, "DarkTheme");
    qmlRegisterSingletonType(QUrl("qrc:/kanemoot/ui/themes/Light.qml"), "Themes", 1, 0, "LightTheme");
}

template<typename Storage, typename ValueType = QString>
bool testStorage(Storage &storage,
                 QString &outError,
                 const QString &key = "init_test",
                 const ValueType &value = "ok")
{
    outError.clear();

    if constexpr (std::is_same_v<decltype(storage.setValue(key, value)), bool>) {
        if (!storage.setValue(key, value)) {
            outError = "Не удалось записать тестовое значение в хранилище!";
            return false;
        }
    } else {
        storage.setValue(key, value);
    }

    auto val = storage.getValue(key);
    bool ok = false;
    if constexpr (std::is_same_v<decltype(val), std::optional<ValueType>>) {
        ok = val.has_value() && val.value() == value;
    } else {
        ok = (val == value);
    }

    if (!ok) {
        outError = "Ошибка проверки хранилища: прочитанное значение не совпадает с тестовым!";
        return false;
    }

    if constexpr (std::is_same_v<decltype(storage.removeValue(key)), bool>) {
        if (!storage.removeValue(key)) {
            outError = "Не удалось удалить тестовое значение из хранилища, но тест успешен.";
        }
    } else {
        storage.removeValue(key);
    }

    return true;
}

void App::setupSettings() {
    Logging::instance().log(Logging::Info,"Инициализация незащищенного хранилища конфигурации...");
    auto &settings = SettingsManager::instance();
    QString error;
    if (testStorage(settings, error)) {
        Logging::instance().log(Logging::Info,"Незащищённое хранилище инициализировано успешно.");
    } else {
        // LOG(Logging::Fatal, "Ошибка инициализации незащищённого хранилища: " + error);
        LOG(Logging::Fatal, ErrorCode::make(ErrorCode::System, 0x01, ErrorCode::App), error);
    }
}

void App::setupSecureStorage() {
    Logging::instance().log(Logging::Info,"Инициализация защищенного хранилища...");
    auto &storage = SecureStorage::instance();
    QString error;
    if (testStorage(storage, error)) {
        Logging::instance().log(Logging::Info,"Защищённое хранилище инициализировано успешно.");
    } else {
        LOG(Logging::Fatal, ErrorCode::make(ErrorCode::System, 0x02, ErrorCode::App), error);
    }
}

