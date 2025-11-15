#include "settings_data.h"
#include "ui_settings_data.h"

settings_data::settings_data(QWidget *parent)
    : QWidget(parent), ui(new Ui::settings_data)
{
    ui->setupUi(this);

    // connect buttons
    connect(ui->selectAllButton, &QPushButton::clicked, this, &settings_data::on_selectAllButton_click);
    connect(ui->deselectAllButton, &QPushButton::clicked, this, &settings_data::on_deselectAllButton_click);
    connect(ui->applyButton, &QPushButton::clicked, this, &settings_data::on_applyButton_click);

}

settings_data::~settings_data()
{
    delete ui;
}

// select all
void settings_data::on_selectAllButton_click() {
    QList<QCheckBox*> checkboxes = ui->scrollAreaWidgetContents->findChildren<QCheckBox*>();
    for (QCheckBox* checkbox : checkboxes) {
        checkbox->setChecked(true);
    }

}

// deselect all
void settings_data::on_deselectAllButton_click() {
    QList<QCheckBox*> checkboxes = ui->scrollAreaWidgetContents->findChildren<QCheckBox*>();
    for (QCheckBox* checkbox : checkboxes) {
        checkbox->setChecked(false);
    }
}

// apply settings - emits map from getSettings()
void settings_data::on_applyButton_click() {
    emit settingsApplied(getSettings());
    this->close();

}

// get settings
QMap<QString, bool> settings_data::getSettings() const {
    QMap<QString, bool> settings;
    QSettings user_settings("weatherWidget", "weather");
    QList<QCheckBox*> checkboxes = ui->scrollAreaWidgetContents->findChildren<QCheckBox*>();
    for (QCheckBox* checkbox : checkboxes) {
        settings[checkbox->objectName()] = checkbox->isChecked();
        user_settings.setValue(checkbox->objectName(), checkbox->isChecked());
    }

    return settings;

}

// load settings - also emits settings map to ensure main window shows saved settings
void settings_data::loadSettings() {
    QSettings user_settings("weatherWidget", "weather");
    QList<QCheckBox*> checkboxes = ui->scrollAreaWidgetContents->findChildren<QCheckBox*>();
    for (QCheckBox* checkbox : checkboxes) {
        bool check = user_settings.value(checkbox->objectName(),true).toBool();
        checkbox->setChecked(check);
    }
    emit settingsApplied(getSettings());
}
