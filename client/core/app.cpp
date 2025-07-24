#include "core/app.h"
#include "utils/logging.h"

App::App(QObject *parent)
{

}

void App::init()
{
    QCoreApplication::setOrganizationName("KANEMOOT");
    QCoreApplication::setApplicationName("KANEMOOT Messenger");

    setupLogging();
    setupDependencies();
}

void App::run()
{
    appController = new AppController(this);
}

void App::setupLogging()
{
    SET_LOG_LEVEL(Logging::Debug);
}

void App::setupDependencies()
{
    // подключение сетевого слоя, хранилища токена, и т.п.
}
