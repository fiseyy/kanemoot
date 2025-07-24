#ifndef APPCONTROLLER_H
#define APPCONTROLLER_H

#include <QObject>
#include "controller/basepage.h"

class AppController : public QObject
{
    Q_OBJECT
public:
    AppController(QObject* parent);
    void setCurrentPage(BasePage* newPage);
public slots:
    void onPageChangeRequested(BasePage* newPage);
private:
    BasePage* currentPage = nullptr;
};

#endif // APPCONTROLLER_H
