#ifndef ERRORHANDLER_H
#define ERRORHANDLER_H

#include <QObject>
#include <QQmlApplicationEngine>

class ErrorHandler : public QObject {
    Q_OBJECT

public:
    static ErrorHandler& instance() {
        static ErrorHandler _instance;
        return _instance;
    }

    void init(QQmlApplicationEngine *engine);

    void showError(const QString &title, const QString &message);

    void setHiden(bool m_hiden);

private:
    explicit ErrorHandler(QObject *parent = nullptr);
    ErrorHandler(const ErrorHandler &) = delete;
    ErrorHandler &operator=(const ErrorHandler &) = delete;

    bool hiden = false;

    QQmlApplicationEngine *m_engine;
};

#endif // ERRORHANDLER_H
