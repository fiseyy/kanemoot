#ifndef BASEPAGE_H
#define BASEPAGE_H

#include <QObject>

class BasePage : public QObject {
    Q_OBJECT
public:
    virtual void init() = 0;
    virtual void cleanup() = 0;
signals:
    void requestPageChange(BasePage* newPage);
};

#endif // BASEPAGE_H
