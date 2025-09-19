#ifndef KEYCATCHER_H
#define KEYCATCHER_H

#include <QGuiApplication>
#include <QKeyEvent>
#include <QObject>
#include <QDebug>

class KeyFilter : public QObject {
    Q_OBJECT
protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

signals:
    void keyPressed(int key);
};

#endif // KEYCATCHER_H
