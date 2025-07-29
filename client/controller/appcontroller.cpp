#include "controller/appcontroller.h"
#include "controller/loginpage.h"
#include "controller/registrationpage.h"

AppController::AppController(QObject *parent, QQmlApplicationEngine* engine)
    : QObject(parent), engine(engine)
{}

void AppController::setCurrentPage(BasePage *newPage)
{
    if (currentPage) {
        if (auto oldRootObj = currentPage->getRootObject()) {
            if (auto oldRootItem = qobject_cast<QQuickItem*>(oldRootObj)) {
                oldRootItem->setParentItem(nullptr);
                oldRootItem->deleteLater();
            } else {
                oldRootObj->deleteLater();
            }
        }
        currentPage->cleanup();
        currentPage->deleteLater();
        currentPage = nullptr;
    }

    connect(newPage, &BasePage::requestPageChange, this, &AppController::onPageChangeRequested);

    if (!engine) {
        qWarning() << "QML Engine is null";
        return;
    }

    QQmlComponent component(engine, QUrl(newPage->qmlPath()));

    if (component.status() != QQmlComponent::Ready) {
        qWarning() << "QML Component Error:" << component.errorString();
        return;
    }

    QObject* obj = component.create();

    if (!obj) {
        qWarning() << "Failed to create page QML";
        return;
    }

    QQuickItem* pageItem = qobject_cast<QQuickItem*>(obj);
    QObject* rootWindowObj = engine->rootObjects().value(0);
    QQuickWindow* rootWindow = qobject_cast<QQuickWindow*>(rootWindowObj);

    if (!pageItem || !rootWindow) {
        qWarning() << "Invalid page or window object";
        qDebug() << "pageItem:" << pageItem;
        qDebug() << "rootWindowObj:" << rootWindowObj;
        qDebug() << "rootWindow:" << rootWindow;
        return;
    }

    pageItem->setParentItem(rootWindow->contentItem());

    newPage->setRootObject(pageItem);
    currentPage = newPage;
    currentPage->init();
}

void AppController::start()
{
    LoginPage* loginPage = new LoginPage(this);
    setCurrentPage(loginPage);
}

void AppController::onPageChangeRequested(int newPageId)
{
    BasePage* newPage = nullptr;
    switch(newPageId) {
        case Page_Login: newPage = new LoginPage(this); break;
        case Page_Register: newPage = new RegistrationPage(this); break;
    }
    if (newPage != nullptr) setCurrentPage(newPage);
}
