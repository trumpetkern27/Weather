#ifndef SETTINGS_MAIN_H
#define SETTINGS_MAIN_H

#include <QWidget>
#include <QPushButton>
#include <windows.h>

class change_location;
class fetch;


namespace Ui {
class settings_main;
}

class settings_main : public QWidget
{
    Q_OBJECT

public:
    explicit settings_main(QWidget *parent = nullptr);
    ~settings_main();
    fetch *weather_harvester;
signals:
    void openAdvanced();
private:
    Ui::settings_main *ui;
    void ChangeLocation();
    void ResetAll();
    change_location *change = nullptr;
};

#endif // SETTINGS_MAIN_H
