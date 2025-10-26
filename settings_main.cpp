#include "settings_main.h"
#include "ui_settings_main.h"

settings_main::settings_main(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::settings_main)
{
    ui->setupUi(this);

    connect(ui->buttonAdvancedSettings, &QPushButton::clicked, this, [this]() {
        emit openAdvanced();
    });
}

settings_main::~settings_main()
{
    delete ui;
}

