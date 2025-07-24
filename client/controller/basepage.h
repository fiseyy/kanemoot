#ifndef BASEPAGE_H
#define BASEPAGE_H

#include <QObject>
#include <QQmlApplicationEngine>
#include <QCoreApplication>

class BasePage : public QObject {
    Q_OBJECT
public:
    explicit BasePage(QObject *parent = nullptr) : QObject(parent) {}

    virtual void init() = 0;
    virtual void cleanup() = 0;

    void setRootObject(QObject* obj) { m_rootObject = obj; }
    QObject* getRootObject() const { return m_rootObject; }
signals:
    void requestPageChange(BasePage* newPage);
protected:
    QObject* m_rootObject = nullptr;
};

#endif // BASEPAGE_H
