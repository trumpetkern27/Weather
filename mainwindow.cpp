#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "fetch.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);




    initializePropertyMap();


    settingsWindow = new settings_main(this);
    settingsWindow->setWindowFlag(Qt::Dialog);
    settingsWindow->setWindowModality(Qt::WindowModal);
    settingsWindow->setAttribute(Qt::WA_DeleteOnClose);

    connect(settingsWindow, &settings_main::openAdvanced, this, &MainWindow::openAdvancedSettings);
    connect(settingsWindow, &QObject::destroyed, this, [this]() {
        settingsWindow = nullptr;
    });

    settingsDialog = new settings_data(this);
    settingsDialog->setWindowFlag(Qt::Dialog);
    settingsDialog->setWindowModality(Qt::WindowModal);
    settingsDialog->setAttribute(Qt::WA_DeleteOnClose);

    connect(settingsDialog, &settings_data::settingsApplied, this, &MainWindow::updateVisibleBoxes);
    settingsDialog->loadSettings();



    connect(settingsDialog, &QObject::destroyed, this, [this]() {
        settingsDialog = nullptr;
    });




    // make gear icon
    gear = new QPushButton(this);

    gear->setIcon(QIcon::fromTheme("settings", QIcon(":/icons/gear.png")));
    gear->setIconSize(QSize(30,30));
    gear->setFixedSize(30,30);
    gear->setFlat(true);
    gear->setStyleSheet(R"(
      QPushButton {
            border: none;
            background: rgba(255,255,255,0.1);
           border-radius: 11px;
        }
        QPushButton:hover {
            background: rgba(255,255,255,0.25);
       }
    )");
    gear->raise();

    connect(this, &MainWindow::resizeEvent, [=](QResizeEvent*) { // ensure remains in top right
        gear->setGeometry(this->width() - 30, 0, 30, 30);
    });
    connect(gear, &QPushButton::clicked, this, &MainWindow::openSettings);


    // update weather every 10 mins
    weather_harvester = new fetch(this);
    weather_clock = new QTimer(this);
    connect(weather_clock, &QTimer::timeout, weather_harvester, &fetch::getWeather);
    weather_clock->start(600000);
    QTimer::singleShot(0, weather_harvester, &fetch::getWeather);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initializePropertyMap()
{
    // Map each checkbox name to its corresponding group box
    propertyBoxMap["checkBoxTemperature"] = ui->groupBoxTemperature;
    propertyBoxMap["checkBoxDewpoint"] = ui->groupBoxDewpoint;
    propertyBoxMap["checkBoxMaxTemperature"] = ui->groupBoxMaxTemperature;
    propertyBoxMap["checkBoxMinTemperature"] = ui->groupBoxMinTemperature;
    propertyBoxMap["checkBoxRelativeHumidity"] = ui->groupBoxRelativeHumidity;
    propertyBoxMap["checkBoxApparentTemperature"] = ui->groupBoxApparentTemperature;
    propertyBoxMap["checkBoxWetBulbGlobeTemperature"] = ui->groupBoxWetBulbGlobeTemperature;
    propertyBoxMap["checkBoxHeatIndex"] = ui->groupBoxHeatIndex;
    propertyBoxMap["checkBoxWindChill"] = ui->groupBoxWindChill;
    propertyBoxMap["checkBoxSkyCover"] = ui->groupBoxSkyCover;
    propertyBoxMap["checkBoxWindDirection"] = ui->groupBoxWindDirection;
    propertyBoxMap["checkBoxWindSpeed"] = ui->groupBoxWindSpeed;
    propertyBoxMap["checkBoxWindGust"] = ui->groupBoxWindGust;
    propertyBoxMap["checkBoxWeather"] = ui->groupBoxWeather;
    propertyBoxMap["checkBoxHazards"] = ui->groupBoxHazards;
    propertyBoxMap["checkBoxProbabilityOfPrecipitation"] = ui->groupBoxProbabilityOfPrecipitation;
    propertyBoxMap["checkBoxQuantitativePrecipitation"] = ui->groupBoxQuantitativePrecipitation;
    propertyBoxMap["checkBoxIceAccumulation"] = ui->groupBoxIceAccumulation;
    propertyBoxMap["checkBoxSnowfallAmount"] = ui->groupBoxSnowfallAmount;
    propertyBoxMap["checkBoxSnowLevel"] = ui->groupBoxSnowLevel;
    propertyBoxMap["checkBoxCeilingHeight"] = ui->groupBoxCeilingHeight;
    propertyBoxMap["checkBoxVisibility"] = ui->groupBoxVisibility;
    propertyBoxMap["checkBoxTransportWindSpeed"] = ui->groupBoxTransportWindSpeed;
    propertyBoxMap["checkBoxTransportWindDirection"] = ui->groupBoxTransportWindDirection;
    propertyBoxMap["checkBoxMixingHeight"] = ui->groupBoxMixingHeight;
    propertyBoxMap["checkBoxHainesIndex"] = ui->groupBoxHainesIndex;
    propertyBoxMap["checkBoxLightningActivityLevel"] = ui->groupBoxLightningActivityLevel;
    propertyBoxMap["checkBoxTwentyFootWindSpeed"] = ui->groupBoxTwentyFootWindSpeed;
    propertyBoxMap["checkBoxTwentyFootWindDirection"] = ui->groupBoxTwentyFootWindDirection;
    propertyBoxMap["checkBoxWaveHeight"] = ui->groupBoxWaveHeight;
    propertyBoxMap["checkBoxWavePeriod"] = ui->groupBoxWavePeriod;
    propertyBoxMap["checkBoxWaveDirection"] = ui->groupBoxWaveDirection;
    propertyBoxMap["checkBoxProbabilityOfThunder"] = ui->groupBoxProbabilityOfThunder;
    propertyBoxMap["checkBoxAtmosphericDispersionIndex"] = ui->groupBoxAtmosphericDispersionIndex;
    propertyBoxMap["checkBoxLowVisibilityOccurrenceRiskIndex"] = ui->groupBoxLowVisibilityOccurrenceRiskIndex;
}


void MainWindow::updateVisibleBoxes(const QMap<QString, bool>& settings)
{
    // First, hide all boxes and remove them from the grid
    QGridLayout* gridLayout = qobject_cast<QGridLayout*>(
        ui->scrollAreaWidgetContents->layout()
        );

    if (!gridLayout) return;

    // Remove all items from the grid layout
    QLayoutItem* item;
    while ((item = gridLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->setVisible(false);
        }
    }

    // Now add back only the checked items in a neat 2-column grid
    int row = 0;
    int col = 0;

    // Iterate through settings in the order they appear
    QStringList orderedKeys = {
        "checkBoxTemperature",
        "checkBoxDewpoint",
        "checkBoxMaxTemperature",
        "checkBoxMinTemperature",
        "checkBoxRelativeHumidity",
        "checkBoxApparentTemperature",
        "checkBoxWetBulbGlobeTemperature",
        "checkBoxHeatIndex",
        "checkBoxWindChill",
        "checkBoxSkyCover",
        "checkBoxWindDirection",
        "checkBoxWindSpeed",
        "checkBoxWindGust",
        "checkBoxWeather",
        "checkBoxHazards",
        "checkBoxProbabilityOfPrecipitation",
        "checkBoxQuantitativePrecipitation",
        "checkBoxIceAccumulation",
        "checkBoxSnowfallAmount",
        "checkBoxSnowLevel",
        "checkBoxCeilingHeight",
        "checkBoxVisibility",
        "checkBoxTransportWindSpeed",
        "checkBoxTransportWindDirection",
        "checkBoxMixingHeight",
        "checkBoxHainesIndex",
        "checkBoxLightningActivityLevel",
        "checkBoxTwentyFootWindSpeed",
        "checkBoxTwentyFootWindDirection",
        "checkBoxWaveHeight",
        "checkBoxWavePeriod",
        "checkBoxWaveDirection",
        "checkBoxProbabilityOfThunder",
        "checkBoxAtmosphericDispersionIndex",
        "checkBoxLowVisibilityOccurrenceRiskIndex"
    };

    for (const QString& key : orderedKeys) {
        if (settings.value(key, false)) {
            QGroupBox* box = propertyBoxMap.value(key, nullptr);
            if (box) {
                box->setVisible(true);
                gridLayout->addWidget(box, row, col);

                // Move to next column, or wrap to next row
                col++;
                if (col >= 2) {
                    col = 0;
                    row++;
                }
            }
        }
    }
}

// hide when leave - not close, otherwise won't continue to collect weather data
void MainWindow::leaveEvent(QEvent *event){
    if (settingsWindow && settingsWindow->isVisible()) {
        return;
    }

    if (settingsDialog && settingsDialog->isVisible()){
        return;
    }



    // if return to the box, it won't hide
    QPoint cur = QCursor::pos();
    if (cur.x() < parentWidget()->x() + parentWidget()->width() && cur.x() >= parentWidget()->x() ) {
       return;
    }

    this->hide();
}

// ensure geometry is right
void MainWindow::showEvent(QShowEvent *event) {
    QRect screenGeometry = QApplication::primaryScreen()->availableGeometry();

    int height = screenGeometry.bottom();
    int width = screenGeometry.width() / 2;
    setGeometry(width, 0, width, height);

    gear->setGeometry(this->width() - 30, 0, 30, 30);

}

// open the settings menu - when it closes, it deletes, so it needs to be reintantiated if it closes
void MainWindow::openAdvancedSettings() {
    if (!settingsDialog) {

        settingsDialog = new settings_data(this);
        settingsDialog->setWindowFlag(Qt::Dialog);
        settingsDialog->setWindowModality(Qt::WindowModal);
        settingsDialog->setAttribute(Qt::WA_DeleteOnClose);

        connect(settingsDialog, &settings_data::settingsApplied, this, &MainWindow::updateVisibleBoxes);
        settingsDialog->loadSettings();

        connect(settingsDialog, &QObject::destroyed, this, [this]() {
            settingsDialog = nullptr;
        });

    }
    settingsDialog->show();
    settingsDialog->raise();
    settingsDialog->activateWindow();
}

// open the settings window
void MainWindow::openSettings() {
    if (!settingsWindow) {
        settingsWindow = new settings_main(this);
        settingsWindow->setWindowFlag(Qt::Dialog);
        settingsWindow->setWindowModality(Qt::WindowModal);
        settingsWindow->setAttribute(Qt::WA_DeleteOnClose);

        connect(settingsWindow, &settings_main::openAdvanced, this, &MainWindow::openAdvancedSettings);
        connect(settingsWindow, &QObject::destroyed, this, [this]() {
            settingsWindow = nullptr;
        });
    }
    settingsWindow->show();
    settingsWindow->raise();
    settingsWindow->activateWindow();
}
