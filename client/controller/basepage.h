#ifndef BASEPAGE_H
#define BASEPAGE_H

#include <QObject>
#include <QQmlApplicationEngine>
#include <QCoreApplication>

enum PageType {
    Page_Login,
    Page_Register,
    Page_Profile,
    Page_Chat,
    // ...
};

class BasePage : public QObject {
    Q_OBJECT
public:
    explicit BasePage(QObject *parent = nullptr) : QObject(parent) {}

    virtual void init() = 0;
    virtual void cleanup() = 0;

    virtual QString qmlPath() const = 0;

    void setRootObject(QObject* obj) { m_rootObject = obj; }
    QObject* getRootObject() const { return m_rootObject; }

signals:
    void requestPageChange(int pageId);
protected:
    QObject* m_rootObject = nullptr;
};

#endif // BASEPAGE_H
