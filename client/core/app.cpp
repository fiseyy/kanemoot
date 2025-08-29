#include <QQuickStyle>
#include "core/app.h"
#include "core/errorhandler.h"
#include "core/settingsmanager.h"
#include "core/securestorage.h"
#include "utils/logging.h"
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

void App::run()
{
    engine = new QQmlApplicationEngine(this);
    appController = new AppController(this, engine);

    engine->rootContext()->setContextProperty("appController", appController);
    engine->load(QUrl(QStringLiteral("qrc:/kanemoot/ui/App.qml")));
    if (engine->rootObjects().isEmpty()) {
        LOG(Logging::Critical, "Не удалось загрузить QML");
        qFatal("Failed to load QML");
    }
    setTheme(true);
    appController->start();
    ErrorHandler::instance().init(engine);
}

QQmlApplicationEngine *App::getEngine() const { return engine; }

void App::setTheme(bool useLightTheme) {
    if (!engine || engine->rootObjects().isEmpty()) {
        LOG(Logging::Warning, "Не удалось установить тему: QML не загружен");
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
        LOG(Logging::Critical, "Не удалось создать объект темы!");
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

void App::setupSettings() {
    LOG(Logging::Info,"Инициализация незащищенного хранилища конфигурации...");
    auto &settings = SettingsManager::instance();
    LOG(Logging::Info,"Инициализация незащищенного хранилища конфигурации прошла успешно. Проверка работоспособности...");
    settings.setValue("init_test", "ok");
    auto val = settings.getValue("init_test", "");
    if (val != "ok") {
        LOG(Logging::Critical, "Ошибка инициализации хранилища конфигурации!");
    } else {
        LOG(Logging::Info, "Хранилище конфигурации работает корректно.");
    }
    settings.removeValue("init_test");
}

void App::setupSecureStorage() {
    LOG(Logging::Info, "Инициализация защищенного хранилища (SecureStorage)...");

    auto &storage = SecureStorage::instance();

    LOG(Logging::Info, "Инициализация защищенного хранилища прошла успешно. Проверка работоспособности...");

    // Тестовая запись
    if (!storage.setValue("init_test", "ok")) {
        LOG(Logging::Critical, "Не удалось записать тестовое значение в SecureStorage!");
        return;
    }

    auto val = storage.getValue("init_test");
    if (!val.has_value() || val.value() != "ok") {
        LOG(Logging::Critical, "Ошибка проверки SecureStorage: прочитанное значение не совпадает с тестовым!");
    } else {
        LOG(Logging::Info, "SecureStorage работает корректно.");
    }

    // Очистка тестового ключа
    if (!storage.removeValue("init_test")) {
        LOG(Logging::Warning, "Не удалось удалить тестовое значение из SecureStorage.");
    }
}

