#include "controller/appcontroller.h"
#include "controller/loginpage.h"

AppController::AppController(QObject *parent, QQmlApplicationEngine* engine)
    : QObject(parent), engine(engine)
{}

void AppController::setCurrentPage(BasePage *newPage)
{
    if (!engine) {
        qWarning() << "QML Engine is null";
        return;
    }

    QQmlComponent component(engine, QUrl(QStringLiteral("qrc:/kanemoot/ui/pages/LoginPage.qml")));

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
    connect(loginPage, &BasePage::requestPageChange, this, &AppController::onPageChangeRequested);
    setCurrentPage(loginPage);
}

void AppController::onPageChangeRequested(BasePage *newPage)
{
    setCurrentPage(newPage);
}
