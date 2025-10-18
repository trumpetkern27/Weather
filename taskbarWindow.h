#ifndef TASKBARWINDOW_H
#define TASKBARWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QEvent>
#include <QMouseEvent>
#include <QApplication>
#include <QSettings>
#include <QTimer>


class weather : public QWidget {
public:
    weather();
    ~weather();

protected:
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
private:
    void savePosition();
    void loadPosition(int x = 100, int y = 100);
    int posX, posY, tBarHeight, width, height;
    QTimer *topmostTimer;
    void enforceTopmost();
};



#endif // TASKBARWINDOW_H
