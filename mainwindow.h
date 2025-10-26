#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGroupBox>
#include <QMap>
#include "settings_data.h"
#include <QGridLayout>
#include "fetch.h"
#include <QApplication>
#include <QScreen>
#include "helpers.h"
#include <QPushButton>
#include "settings_main.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void updateVisibleBoxes(const QMap<QString, bool>& settings);
protected:
    void leaveEvent(QEvent *event) override;
    void showEvent(QShowEvent *event) override;
private:
    Ui::MainWindow *ui;
    settings_data *settingsDialog;
    settings_main *settingsWindow;

    QMap<QString, QGroupBox*> propertyBoxMap;

    void initializePropertyMap();
    void reorganizeGrid();
    fetch *weather_harvester;
    QTimer *weather_clock;
    QPushButton *gear;
    void openAdvancedSettings();
    void openSettings();
};
#endif // MAINWINDOW_H
