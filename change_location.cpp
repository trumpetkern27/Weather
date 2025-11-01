#include "change_location.h"
#include "ui_change_location.h"
#include "settings_main.h"
#include "fetch.h"
#include <QPushButton>

change_location::change_location(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::change_location)
{
    ui->setupUi(this);



    // connect buttons
    connect(ui->autodetect, &QPushButton::clicked, this, [this] () {
        QSettings settings("weatherWidget", "weather");
        settings.setValue("autoDetect", true);
        close();
    });

    connect(ui->manual, &QPushButton::clicked, this, [this, &parent] () {
        auto *parent = qobject_cast<settings_main*>(parentWidget());
        if (parent && parent->weather_harvester) {
            parent->weather_harvester->promptCity();
            close();
        }
    });
}

change_location::~change_location()
{
    delete ui;
}



