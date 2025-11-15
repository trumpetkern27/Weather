#include "settings_main.h"
#include "ui_settings_main.h"
#include "change_location.h"
#include "mainwindow.h"
#include "fetch.h"
#include <QProcess>

settings_main::settings_main(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::settings_main)
{
    ui->setupUi(this);

    connect(ui->buttonAdvancedSettings, &QPushButton::clicked, this, [this]() {
        emit openAdvanced();
    });

    connect(ui->buttonChangeLocation, &QPushButton::clicked, this, [this] () {
        ChangeLocation();
    });

    connect(ui->buttonRestart, &QPushButton::clicked, this, [this] () {
        ResetAll();
    });



    connect(ui->groupBox_units, &QGroupBox::toggled, this, [this] () {
        QSettings settings("weatherWidget", "weather");

        if (ui->radioCelsius->isChecked()) {
            settings.setValue("uom", "C");
        } else {
            settings.setValue("uom", "F");
        }

    });
}

settings_main::~settings_main()
{
    delete ui;
}

void settings_main::ResetAll() {
    QSettings settings("weatherWidget", "weather");
    settings.clear();
    settings.sync();

    qApp->quit();
    QProcess::startDetached(qApp->applicationFilePath());
}

void settings_main::ChangeLocation() {
    if (!change) {
        auto *parent = qobject_cast<MainWindow*>(parentWidget());

        if(parent && parent->weather_harvester) {
            weather_harvester = parent->weather_harvester;
        }

        change = new change_location(this);

        connect(change, &QObject::destroyed, this, [this]() {
            change = nullptr;
        });

        change->setWindowFlag(Qt::Dialog);
        change->setWindowModality(Qt::WindowModal);
        change->setAttribute(Qt::WA_DeleteOnClose);
    }

    change->show();
    change->raise();
    change->activateWindow();

}


