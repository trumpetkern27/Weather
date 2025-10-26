#ifndef DUMMY_H
#define DUMMY_H

#include <QWidget>
#include <QTimer>
#include <QApplication>
#include <windows.h>
#include <qscreen.h>
#include <QMouseEvent>
#include <qevent.h>
#include <QEnterEvent>

class dummy : public QWidget
{
public:
    dummy();
    ~dummy();
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
private:
    QTimer *dTimer;
    void flash();
    bool isBlue;

    QPoint dragStart;
    QPoint windowStart;
    bool isDragging;

};

#endif // DUMMY_H
