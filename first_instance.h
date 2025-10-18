#ifndef FIRST_INSTANCE_H
#define FIRST_INSTANCE_H

#include "ui_first_instance.h"
#include <QDialog>
#include <QSettings>
#include <QWidget>
#include <QApplication>
#include <qscreen.h>
#include <windows.h>
#include <QTimer>
#include <QMouseEvent>

class first_instance : public QDialog
{
    Q_OBJECT
public:
    first_instance(QWidget *parent = nullptr);
    ~first_instance();
private slots:
    void flash();
protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
private:
    Ui::Form *ui;
    QWidget *dummy;
    QTimer *dTimer;
    QTimer *t;
    bool isBlue;
    bool isDragging;
    QPoint dragStart;
    QPoint windowStart;
    void reset();
    void resizeDummy(int value);
    void saveSettings();
    void enforceTopmost();
};

#endif // FIRST_INSTANCE_H
