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
#include <windows.h>
#include <dwmapi.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

struct weather_value {
    double value;
    QString unit;
    bool isValid;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    fetch *weather_harvester;
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

    QTimer *weather_clock;
    QPushButton *gear;
    void openAdvancedSettings();
    void openSettings();

    void update_grid_data(QJsonDocument doc);
    void update_grid_data_observation(QJsonDocument doc);

    weather_value extractWeatherValue(const QJsonObject& properties, const QString& apiProperty, bool isObservation);
    void updateGroupBox(QGroupBox *groupBox, double value, const QString &uom, const QString &property);

    COLORREF GetTaskbarColor();
};
#endif // MAINWINDOW_H
