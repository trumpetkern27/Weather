#ifndef TASKBARWINDOW_H
#define TASKBARWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QEvent>
#include <QMouseEvent>
#include <QApplication>
#include <QSettings>
#include <QTimer>
#include <QLabel>


class weather : public QWidget {
public:
    weather();
    ~weather();

protected:
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    //void closeEvent(QCloseEvent *event) override;
private:
    void savePosition();
    void loadPosition(int x = 100, int y = 100);
    int posX, posY, tBarHeight, width, height;
    QTimer *topmostTimer;
    void enforceTopmost();

    QMainWindow *main;

    QLabel *label;
};



#endif // TASKBARWINDOW_H
