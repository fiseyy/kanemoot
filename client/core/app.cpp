#include <QQuickStyle>
#include "core/app.h"
#include "core/errorhandler.h"
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
    appController->start();
    ErrorHandler::instance().init(engine);
}

QQmlApplicationEngine *App::getEngine() const { return engine; }

void App::setupLogging()
{
    SET_LOG_LEVEL(Logging::Debug);
}

void App::setupDependencies()
{
    // подключение сетевого слоя, хранилища токена, и т.п.
}
