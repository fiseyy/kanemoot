#ifndef APP_H
#define APP_H

#include <QObject>
#include <QQmlApplicationEngine>
#include <QCoreApplication>
#include <QQmlContext>
#include <QResource>
#include "controller/appcontroller.h"

class App : public QObject {
    Q_OBJECT

public:
    explicit App(QObject* parent = nullptr);
    void init();
    void run();
    QQmlApplicationEngine* getEngine() const;
private:
    void setupLogging();
    void setupDependencies();

    QQmlApplicationEngine* engine = nullptr;
    AppController* appController = nullptr;
};

#endif // APP_H
