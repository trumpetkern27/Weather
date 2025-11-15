// fetch.cpp
#include "fetch.h"
#include "cityprompt.h"
#include <windows.h>


QString userAgent = QString("WeatherWidget/%1 (UserID/%2)")
                        .arg("1.0")
                        .arg(QUuid::createUuid().toString());


//QString userAgent = QString("weather-widget/1.0 (+https://github.com/trumpetkern27/weather)");
fetch::fetch(QWidget *parentWidget, QObject *parent) : QObject(parent), mainWidget(parentWidget) {
    manager = new QNetworkAccessManager(this);

}

void fetch::getWeather() {

    loadLocation();

}


void fetch::getWeatherData() {

    QString url = QString("https://api.weather.gov/points/%1,%2").arg(lat, 0, 'f', 4).arg(lon, 0, 'f', 4);

    QNetworkRequest request{QUrl(url)};
    request.setHeader(QNetworkRequest::UserAgentHeader, userAgent.toUtf8());
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                         QNetworkRequest::NoLessSafeRedirectPolicy);
    QNetworkReply *reply = manager->get(request);

    // *graceful* error handling
    connect(reply, &QNetworkReply::errorOccurred, this, [reply](QNetworkReply::NetworkError code) {
        QString errStr = reply->errorString();
        QString message = QStringLiteral(
                              "An error has occurred:\n%1\n\nPlease check your connection or try restarting this application."
                              ).arg(errStr);

        MessageBoxW(
            NULL,
            reinterpret_cast<LPCWSTR>(message.utf16()),
            L"Error",
            MB_ICONERROR | MB_OK
            );
    });


    connect(reply, &QNetworkReply::finished, this, [this, reply]() {



        if (reply->error()) {
            emit errorOccurred(reply->errorString());
        } else {
            QByteArray data = reply->readAll();

            // Parse JSON to get the forecast URL

            QJsonDocument doc = QJsonDocument::fromJson(data);
            QJsonObject obj = doc.object();
            QJsonObject properties = obj["properties"].toObject();

            QString forecastUrl = properties["forecastGridData"].toString();

            // Now fetch the actual forecast
            getForecast(forecastUrl);

            // get local forecast
            QString observationsUrl = properties["observationStations"].toString();

            getObservationStations(observationsUrl);
        }
        reply->deleteLater();
    });

}

// actually get forecast
void fetch::getForecast(const QString &forecastUrl) {

    QNetworkRequest request{QUrl(forecastUrl)};
    request.setHeader(QNetworkRequest::UserAgentHeader, userAgent.toUtf8());
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                         QNetworkRequest::NoLessSafeRedirectPolicy);
    QNetworkReply *reply = manager->get(request);

    // Connect to THIS specific reply only
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error()) {
            emit errorOccurred(reply->errorString());
        } else {
            QByteArray data = reply->readAll();

            emit weatherDataReceived(data);


        }
        reply->deleteLater();
    });
}

void fetch::getObservationStations(const QString &stationsUrl) {

    QNetworkRequest request{QUrl(stationsUrl)};
    request.setHeader(QNetworkRequest::UserAgentHeader, userAgent.toUtf8());
    QNetworkReply *reply = manager->get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (!reply->error()) {
            QByteArray responseData = reply->readAll();

            QJsonDocument doc = QJsonDocument::fromJson(responseData);
            QJsonObject root = doc.object();

            // Try different paths to find the stations array

            QJsonArray stations;

            // Could be at different paths depending on response format
            if (root.contains("observationStations")) {
                stations = root["observationStations"].toArray();
            } else if (root.contains("features")) {
                stations = root["features"].toArray();
            }


            if (!stations.isEmpty()) {

                // Check if it's a string (URL) or object
                if (stations[0].isString()) {
                    QString stationUrl = stations[0].toString();

                    // Extract ID from URL
                    QStringList parts = stationUrl.split('/');
                    QString stationId = parts.isEmpty() ? "" : parts.last();

                    if (!stationId.isEmpty()) {
                        getLatestObservation(stationId);
                    }
                } else if (stations[0].isObject()) {
                    QJsonObject stationObj = stations[0].toObject();

                    // Try to find station ID in properties
                    QJsonObject props = stationObj["properties"].toObject();
                    QString stationId = props["stationIdentifier"].toString();

                    if (!stationId.isEmpty()) {
                        getLatestObservation(stationId);
                    }
                }
            }
        }
        reply->deleteLater();
    });
}

void fetch::getLatestObservation(const QString &stationId) {
    QString url = QString("https://api.weather.gov/stations/%1/observations/latest").arg(stationId);

    QNetworkRequest request{QUrl(url)};
    request.setHeader(QNetworkRequest::UserAgentHeader, userAgent.toUtf8());
    QNetworkReply *reply = manager->get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply, url]() {
        if (!reply->error()) {
            QByteArray data = reply->readAll();


            QJsonDocument doc = QJsonDocument::fromJson(data);
            QJsonObject root = doc.object();
            QJsonObject properties = root["properties"].toObject();
            emit stationDataReceived(data);
        }
        reply->deleteLater();
    });
}

void fetch::setLocation() {
    // save to settings
    QSettings settings("weatherWidget", "weather");
    settings.setValue("lat", lat);
    settings.setValue("lon", lon);
}

void fetch::loadLocation() {
    // load from settings
    QSettings settings("weatherWidget", "weather");
    lat = settings.value("lat", 500).toDouble();
    lon = settings.value("lon", 500).toDouble();

    bool autoDetect = settings.value("autoLocation", false).toBool();

    if (lat == 500 || lon == 500) {
        if (autoDetect) {
            getLocation();
        } else {
            promptCity();
        }
    } else {
        getWeatherData();
    }
}

// autodetect location
void fetch::getLocation() {

    QGeoPositionInfoSource *source = QGeoPositionInfoSource::createDefaultSource(this);

    if (source) {
        connect(source, &QGeoPositionInfoSource::positionUpdated, this,
                [this, source](const QGeoPositionInfo &info) {

            lat = info.coordinate().latitude();
            lon = info.coordinate().longitude();

            setLocation();
            getWeatherData();
            source->deleteLater();

        });

        source->requestUpdate();
    } else { // use ip if geoposition doesn't work

        QNetworkRequest request{QUrl("https://ipapi.co/json/")};

        QNetworkReply *reply = manager->get(request);

        connect(reply, &QNetworkReply::finished, this, [this, reply]() {

            if (!reply->error()) {
                QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
                QJsonObject obj = doc.object();

                lat = obj["latitude"].toDouble();
                lon = obj["longitude"].toDouble();

                setLocation();
                getWeatherData();
            }
            reply->deleteLater();
        });
    }


}

// prompt location
void fetch::promptCity() {

    prompt = new cityPrompt(mainWidget);
    prompt->setAttribute(Qt::WA_DeleteOnClose);

    connect(prompt, &cityPrompt::cityEntered, this, [this](const QString &city) {

        getCityCoordinates(city, [this](bool success) {
            if (success) {

                prompt->setVisible(false);

                getWeatherData();

                QTimer::singleShot(2000, prompt, &QWidget::close);
            } else {
                prompt->showError();
            }
        });

    });
    prompt->setWindowFlag(Qt::Dialog);
    prompt->setWindowModality(Qt::WindowModal);

    prompt->show();
    prompt->raise();
    prompt->activateWindow();


}

// get city coordinates
void fetch::getCityCoordinates(const QString &cityName, std::function<void(bool)> callback) {
    QString url = QString("https://nominatim.openstreetmap.org/search?q=%1&format=json&limit=1").arg(cityName);

    QNetworkRequest request{QUrl(url)};
    request.setHeader(QNetworkRequest::UserAgentHeader, userAgent.toUtf8());
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                         QNetworkRequest::NoLessSafeRedirectPolicy);
    QNetworkReply *reply = manager->get(request);


    connect(reply, &QNetworkReply::finished, this, [this, reply, callback]() {
        bool success = false;

        if (!reply->error()) {
            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
            QJsonArray results = doc.array();

            if (!results.isEmpty()) {
                QJsonObject location = results[0].toObject();
                double test_lat = location["lat"].toString().toDouble();
                double test_lon = location["lon"].toString().toDouble();

                verifyCoordinates(test_lat, test_lon, callback);
                reply->deleteLater();
                return;
            }
        }
        callback(false);
        reply->deleteLater();
    });

}

void fetch::verifyCoordinates(double test_lat, double test_lon, std::function<void(bool)> callback) {
    QString url = QString("https://api.weather.gov/points/%1,%2")
                        .arg(test_lat, 0, 'f', 4)
                        .arg(test_lon, 0, 'f', 4);
    QNetworkRequest request{QUrl(url)};
    request.setHeader(QNetworkRequest::UserAgentHeader, userAgent.toUtf8());
    QNetworkReply *reply = manager->get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply, test_lat, test_lon, callback] {
        bool success = false;
        if (!reply->error()) {
            lat = test_lat;
            lon = test_lon;
            setLocation();
            success = true;
        }
        callback(success);
        reply->deleteLater();
    });
}


