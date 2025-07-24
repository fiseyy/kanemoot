#include "controller/appcontroller.h"
#include "controller/loginpage.h"

AppController::AppController(QObject *parent)
    : QObject(parent) {

    LoginPage* loginPage = new LoginPage(this);

    connect(loginPage, &BasePage::requestPageChange, this, &AppController::onPageChangeRequested);
    setCurrentPage(loginPage);
}

void AppController::setCurrentPage(BasePage *newPage)
{
    if (currentPage) {
        currentPage->cleanup();
    }
    currentPage = newPage;
    currentPage->init();
}

void AppController::onPageChangeRequested(BasePage *newPage)
{
    setCurrentPage(newPage);
}
