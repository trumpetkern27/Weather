#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "fetch.h"



const QStringList orderedKeys = {
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
    connect(weather_clock, &QTimer::timeout, this, [this]() {
        weather_harvester->getWeather();
    });

    connect(weather_harvester, &fetch::weatherDataReceived, this, [this](const QByteArray &data){

        QJsonDocument doc = QJsonDocument::fromJson(data);
        update_grid_data(doc);
    });

    connect(weather_harvester, &fetch::stationDataReceived, this, [this](const QByteArray &data) {

        QJsonDocument doc = QJsonDocument::fromJson(data);
        update_grid_data_observation(doc);
    });

    weather_clock->start(600000);
    QTimer::singleShot(0, weather_harvester, &fetch::getWeather);

    // get weather on startup

    weather_harvester->getWeather();

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

void MainWindow::update_grid_data(QJsonDocument doc) {
    QJsonObject root = doc.object();
    QJsonObject properties = root["properties"].toObject();

    QSettings settings("weatherWidget", "weather");

    QMap<QString, QString> checkboxToApiProperty = {
        {"checkBoxTemperature", "temperature"},
        {"checkBoxDewpoint", "dewpoint"},
        {"checkBoxMaxTemperature", "maxTemperature"},
        {"checkBoxMinTemperature", "minTemperature"},
        {"checkBoxRelativeHumidity", "relativeHumidity"},
        {"checkBoxApparentTemperature", "apparentTemperature"},
        {"checkBoxWetBulbGlobeTemperature", "wetBulbGlobeTemperature"},
        {"checkBoxHeatIndex", "heatIndex"},
        {"checkBoxWindChill", "windChill"},
        {"checkBoxSkyCover", "skyCover"},
        {"checkBoxWindDirection", "windDirection"},
        {"checkBoxWindSpeed", "windSpeed"},
        {"checkBoxWindGust", "windGust"},
        {"checkBoxWeather", "weather"},
        {"checkBoxHazards", "hazards"},
        {"checkBoxProbabilityOfPrecipitation", "probabilityOfPrecipitation"},
        {"checkBoxQuantitativePrecipitation", "quantitativePrecipitation"},
        {"checkBoxIceAccumulation", "iceAccumulation"},
        {"checkBoxSnowfallAmount", "snowfallAmount"},
        {"checkBoxSnowLevel", "snowLevel"},
        {"checkBoxCeilingHeight", "ceilingHeight"},
        {"checkBoxVisibility", "visibility"},
        {"checkBoxTransportWindSpeed", "transportWindSpeed"},
        {"checkBoxTransportWindDirection", "transportWindDirection"},
        {"checkBoxMixingHeight", "mixingHeight"},
        {"checkBoxHainesIndex", "hainesIndex"},
        {"checkBoxLightningActivityLevel", "lightningActivityLevel"},
        {"checkBoxTwentyFootWindSpeed", "twentyFootWindSpeed"},
        {"checkBoxTwentyFootWindDirection", "twentyFootWindDirection"},
        {"checkBoxWaveHeight", "waveHeight"},
        {"checkBoxWavePeriod", "wavePeriod"},
        {"checkBoxWaveDirection", "waveDirection"},
        {"checkBoxProbabilityOfThunder", "probabilityOfThunder"},
        {"checkBoxAtmosphericDispersionIndex", "atmosphericDispersionIndex"},
        {"checkBoxLowVisibilityOccurrenceRiskIndex", "lowVisibilityOccurrenceRiskIndex"}
    };

    QDateTime now = QDateTime::currentDateTime();

    for (auto it = checkboxToApiProperty.constBegin(); it != checkboxToApiProperty.constEnd(); ++it) {
        QString checkboxName = it.key();
        QString apiProperty = it.value();

        if (!settings.value(checkboxName, false).toBool()) {
            continue;
        }

        QJsonObject propertyData = properties[apiProperty].toObject();
        QJsonArray values = propertyData["values"].toArray();

        if (values.isEmpty()) {
            continue;
        }

        for (const QJsonValue &val : values) {
            QJsonObject valueObj = val.toObject();
            QString validTime = valueObj["validTime"].toString();

            QStringList timeParts = validTime.split('/');
            if (timeParts.isEmpty()) continue;

            QDateTime startTime = QDateTime::fromString(timeParts[0], Qt::ISODate);
            if (startTime <= now) {
                double value = valueObj["value"].toDouble();
                QString uom = propertyData["uom"].toString();

                QGroupBox *groupBox = propertyBoxMap.value(checkboxName, nullptr);
                if (groupBox) {
                    updateGroupBox(groupBox, value, uom, apiProperty);
                }
                break;
            }
        }
    }
}

void MainWindow::update_grid_data_observation(QJsonDocument doc) {
    QJsonObject root = doc.object();
    QJsonObject properties = root["properties"].toObject();

    QSettings settings("weatherWidget", "weather");

    QMap<QString, QString> checkboxToApiProperty = {
        {"checkBoxTemperature", "temperature"},
        {"checkBoxDewpoint", "dewpoint"},
        {"checkBoxRelativeHumidity", "relativeHumidity"},
        {"checkBoxWindDirection", "windDirection"},
        {"checkBoxWindSpeed", "windSpeed"},
        {"checkBoxWindGust", "windGust"},
        {"checkBoxWindChill", "windChill"},
        {"checkBoxHeatIndex", "heatIndex"},
        {"checkBoxVisibility", "visibility"}
    };

    for (auto it = checkboxToApiProperty.constBegin(); it != checkboxToApiProperty.constEnd(); ++it) {
        QString checkboxName = it.key();
        QString apiProperty = it.value();

        if (!settings.value(checkboxName, false).toBool()) {
            continue;
        }

        weather_value weatherVal = extractWeatherValue(properties, apiProperty, true);

        if (weatherVal.isValid) {
            QGroupBox *groupBox = propertyBoxMap.value(checkboxName, nullptr);
            if (groupBox) {
                updateGroupBox(groupBox, weatherVal.value, weatherVal.unit, apiProperty);
            }
        }
    }
}

weather_value MainWindow::extractWeatherValue(const QJsonObject& properties, const QString& apiProperty, bool isObservation) {
    weather_value result{0.0, "", false};

    if (isObservation) {
        // Observation format: direct value
        QJsonObject propertyData = properties[apiProperty].toObject();
        if (!propertyData.isEmpty() && !propertyData["value"].isNull()) {
            result.value = propertyData["value"].toDouble();
            result.unit = propertyData["unitCode"].toString();
            result.isValid = true;
        }
    } else {
        // Gridpoint format: values array
        QJsonObject propertyData = properties[apiProperty].toObject();
        QJsonArray values = propertyData["values"].toArray();

        if (!values.isEmpty()) {
            QDateTime now = QDateTime::currentDateTime();

            for (const QJsonValue &val : values) {
                QJsonObject valueObj = val.toObject();
                QString validTime = valueObj["validTime"].toString();
                QStringList timeParts = validTime.split('/');

                if (!timeParts.isEmpty()) {
                    QDateTime startTime = QDateTime::fromString(timeParts[0], Qt::ISODate);
                    if (startTime <= now) {
                        result.value = valueObj["value"].toDouble();
                        result.unit = propertyData["uom"].toString();
                        result.isValid = true;
                        break;
                    }
                }
            }
        }
    }

    return result;
}

// actually update the grid with data
void MainWindow::updateGroupBox(QGroupBox *groupBox, double value, const QString &uom, const QString &property) {
    QString displayText;

    if (property.contains("temperature", Qt::CaseInsensitive) ||
        property.contains("dewpoint", Qt::CaseInsensitive)) {

        QSettings settings("weatherWidget", "weather");

        QString set_uom = settings.value("uom", "C").toString();

        double temp;

        if (set_uom == "C") {
            if (uom.contains("degC") || uom.contains("wmoUnit:degC")) {
                temp = value;
            } else {
                temp = (value - 32) * 5 / 9;
            }
        } else {
            if (uom.contains("degC") || uom.contains("wmoUnit:degC")) {
                temp = value * 9 / 5 + 32;
            } else {
                temp = value;
            }
        }

        displayText = QString::number(temp, 'f', 2) + "°" + set_uom;


        // put it on the bar
        QLabel *label = parentWidget()->findChild<QLabel*>();
        if (label) {
            label->setText(displayText);
        }
        /*
        PAINTSTRUCT ps;
        HWND hWnd = (HWND)parentWidget()->winId();

        HDC hdc = GetDC(hWnd);
        RECT rc;
        GetClientRect(hWnd, &rc);

        COLORREF taskbarColor = GetTaskbarColor();
        HBRUSH brush = CreateSolidBrush(taskbarColor);

        FillRect(hdc, &rc, brush);
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(255,255,255));
        std::wstring wtext = displayText.toStdWString();
        DrawText(hdc, wtext.c_str(), -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        ReleaseDC(hWnd, hdc);
        */

    } else if (property.contains("humidity", Qt::CaseInsensitive) ||
               property.contains("probability", Qt::CaseInsensitive) ||
               property.contains("skyCover", Qt::CaseInsensitive)) {
        displayText = QString::number(value, 'f', 1) + "%";
    } else if (property.contains("speed", Qt::CaseInsensitive)) {
        if (uom.contains("km_h")) {
            value = value * 0.621371;
        }
        displayText = QString::number(value, 'f', 2) + "mph";
    } else if (property.contains("direction", Qt::CaseInsensitive)) {
        displayText =  QString::number(value, 'f', 1) + "°";
    } else {
        displayText = QString::number(value, 'f', 1);
    }

    QLabel *label = groupBox->findChild<QLabel*>();

    if (label) {
        label->setText(displayText);
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

// get the taskbar's colour
COLORREF MainWindow::GetTaskbarColor() {
    HWND hwnd = FindWindow(L"Shell_TrayWnd", NULL);
    RECT rc;
    GetWindowRect(hwnd, &rc);
    HDC hdcScreen = GetDC(NULL);
    COLORREF color = GetPixel(hdcScreen, rc.left + 10, rc.top + 10);
    ReleaseDC(NULL, hdcScreen);

    return color;

    /*

    DWORD color   = 0;
    BOOL opaque = FALSE;
    if (SUCCEEDED(DwmGetColorizationColor(&color, &opaque))) {
        return RGB(GetRValue(color), GetGValue(color), GetBValue(color));
    }
    return RGB(0,0,0);

    */
}
