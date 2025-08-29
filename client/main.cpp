#include <QGuiApplication>
#include "core/app.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    App application;
    application.init();
    application.run();
    app.setWindowIcon(QIcon(":///assets/icons/logo.png"));
    return app.exec();
}
