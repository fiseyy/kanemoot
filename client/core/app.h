#ifndef APP_H
#define APP_H

#include <QObject>
#include <QQmlApplicationEngine>
#include <QCoreApplication>
#include <QQmlContext>
#include <QResource>
#include <QQmlComponent>
#include <QQuickItem>
#include <QQmlProperty>
#include <cstdint>
#include "controller/appcontroller.h"
#include "utils/logging.h"

class App : public QObject {
    Q_OBJECT

public:
    explicit App(QObject* parent = nullptr);
    void init();
    void run();
    QQmlApplicationEngine* getEngine() const;
    void setTheme(bool useLightTheme);
private slots:
    void onErrorOccured(Logging::LogLevel level, uint32_t code, const QString &text);
private:
    void setupLogging();
    void setupDependencies();
    void setupSettings();
    void setupSecureStorage();
    void setupKeyCatcher();

    QQmlApplicationEngine* engine = nullptr;
    AppController* appController = nullptr;
};

#endif // APP_H
