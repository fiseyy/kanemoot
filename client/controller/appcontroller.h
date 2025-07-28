#ifndef APPCONTROLLER_H
#define APPCONTROLLER_H

#include <QObject>
#include <QQmlComponent>
#include <QQuickItem>
#include <QQuickWindow>
#include <QQmlEngine>
#include "controller/basepage.h"

class AppController : public QObject
{
    Q_OBJECT
public:
    explicit AppController(QObject* parent = nullptr, QQmlApplicationEngine* engine = nullptr);
    void setCurrentPage(BasePage* newPage);
    void start();
public slots:
    void onPageChangeRequested(int newPageId);
private:
    BasePage* currentPage = nullptr;
    QQmlApplicationEngine* engine = nullptr;
};

#endif // APPCONTROLLER_H
