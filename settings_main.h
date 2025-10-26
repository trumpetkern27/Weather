#ifndef SETTINGS_MAIN_H
#define SETTINGS_MAIN_H

#include <QWidget>
#include <QPushButton>

namespace Ui {
class settings_main;
}

class settings_main : public QWidget
{
    Q_OBJECT

public:
    explicit settings_main(QWidget *parent = nullptr);
    ~settings_main();
signals:
    void openAdvanced();
private:
    Ui::settings_main *ui;
};

#endif // SETTINGS_MAIN_H
