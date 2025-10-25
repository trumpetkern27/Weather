// fetch.cpp
#include "fetch.h"
#include "cityprompt.h"


/*QString userAgent = QString("WeatherWidget/%1 (UserID/%2)")
                        .arg("1.0")
                        .arg(QUuid::createUuid().toString());
*/

QString userAgent = QString("weather-widget/1.0 (+https://github.com/trumpetkern27/weather)");
fetch::fetch(QObject *parent) : QObject(parent) {
    manager = new QNetworkAccessManager(this);

}

void fetch::getWeather() {

    loadLocation();

}


void fetch::getWeatherData() {
    qDebug() << "entered get";
    QString url = QString("https://api.weather.gov/points/%1,%2").arg(lat, 0, 'f', 4).arg(lon, 0, 'f', 4);

    QNetworkRequest request{QUrl(url)};
    request.setHeader(QNetworkRequest::UserAgentHeader, userAgent.toUtf8());
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                         QNetworkRequest::NoLessSafeRedirectPolicy);
    QNetworkReply *reply = manager->get(request);

    qDebug() << "Request URL:" << request.url().toString();
    qDebug() << "SSL supported:" << QSslSocket::supportsSsl();
    qDebug() << "Manager:" << manager;
    qDebug() << "reply object:" << reply;

    connect(reply, &QNetworkReply::errorOccurred, this, [reply](QNetworkReply::NetworkError code) {
        qDebug() << "Network error:" << code << reply->errorString();
    });

    qDebug() << "connecting...." << userAgent;
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        qDebug() << "finished signal fired";


        if (reply->error()) {
            qDebug() << "Error:" << reply->errorString();
            emit errorOccurred(reply->errorString());
        } else {
            QByteArray data = reply->readAll();

            // Parse JSON to get the forecast URL

            QJsonDocument doc = QJsonDocument::fromJson(data);
            QJsonObject obj = doc.object();
            QJsonObject properties = obj["properties"].toObject();
            qDebug() << "properties:" << properties;
            QString forecastUrl = properties["forecast"].toString();

            qDebug() << "Forecast URL:" << forecastUrl;

            // Now fetch the actual forecast
            getForecast(forecastUrl);
        }
        qDebug() << "outside if/else";
        reply->deleteLater();
        qDebug() << "set to delete later";
    });

    qDebug() << "outside connection";
}

void fetch::getForecast(const QString &forecastUrl) {

    qDebug() << "getting forecast...";
    QNetworkRequest request{QUrl(forecastUrl)};
    request.setHeader(QNetworkRequest::UserAgentHeader, userAgent.toUtf8());
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                         QNetworkRequest::NoLessSafeRedirectPolicy);
    QNetworkReply *reply = manager->get(request);

    // Connect to THIS specific reply only
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error()) {
            qDebug() << "Error:" << reply->errorString();
            emit errorOccurred(reply->errorString());
        } else {
            QByteArray data = reply->readAll();

            QJsonDocument doc = QJsonDocument::fromJson(data);
            QJsonObject root = doc.object();
            QJsonObject properties = root["properties"].toObject();
            QJsonArray periods = properties["periods"].toArray();

            if (!periods.isEmpty()) {
                QJsonObject currentPeriod = periods[0].toObject();

                QString name = currentPeriod["name"].toString();
                int temp = currentPeriod["temperature"].toInt();
                QString tempUnit = currentPeriod["temperatureUnit"].toString();
                QString shortForecast = currentPeriod["shortForecast"].toString();
                QString detailedForecast = currentPeriod["detailedForecast"].toString();

                qDebug() << name << ":" << temp << tempUnit;
                qDebug() << shortForecast;
                qDebug() << detailedForecast;
                emit weatherDataReceived(data);
            }


            //qDebug() << "Forecast data:" << data;

        }
        reply->deleteLater();
    });
}

void fetch::setLocation() {
    // save to settings
    QSettings settings("weatherWidget", "weather");
    settings.setValue("lat", lat);
    settings.setValue("lon", lon);
    qDebug() << "set lat:" << lat << "lon:" << lon;


}

void fetch::loadLocation() {
    // load from settings
    QSettings settings("weatherWidget", "weather");
    lat = settings.value("lat", 500).toDouble();
    lon = settings.value("lon", 500).toDouble();

    bool autoDetect = settings.value("autoLocation", false).toBool();


    qDebug() << "load lat:" << lat << "lon:" << lon << "auto:" << autoDetect;
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

void fetch::getLocation() {

    QGeoPositionInfoSource *source = QGeoPositionInfoSource::createDefaultSource(this);

    if (source) {
        connect(source, &QGeoPositionInfoSource::positionUpdated, this,
                [this, source](const QGeoPositionInfo &info) {

            qDebug() << "source";

            lat = info.coordinate().latitude();
            lon = info.coordinate().longitude();

            qDebug() << "gps loc:" << lat << lon;

            setLocation();
            getWeatherData();
            source->deleteLater();

        });

        source->requestUpdate();
    } else {

        qDebug() << "ip";
        QNetworkRequest request{QUrl("https://ipapi.co/json/")};

        QNetworkReply *reply = manager->get(request);

        connect(reply, &QNetworkReply::finished, this, [this, reply]() {

            if (!reply->error()) {
                QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
                QJsonObject obj = doc.object();

                lat = obj["latitude"].toDouble();
                lon = obj["longitude"].toDouble();

                qDebug() << "location:" << lat << lon;

                setLocation();
                getWeatherData();
            }
            reply->deleteLater();
        });
    }


}

void fetch::promptCity() {

    cityPrompt *prompt = new cityPrompt();
    prompt->setAttribute(Qt::WA_DeleteOnClose);



    connect(prompt, &cityPrompt::cityEntered, this, [this, prompt](const QString &city) {
        qDebug() << "entered:" << city;

        getCityCoordinates(city, [this, prompt](bool success) {
            if (success) {
                qDebug() << "coords retreived";

                prompt->setVisible(false);

                getWeatherData();

                QTimer::singleShot(2000, prompt, &QWidget::close);
            } else {
                prompt->showError();
            }
        });

    });

    prompt->show();


}

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
                lat = location["lat"].toString().toDouble();
                lon = location["lon"].toString().toDouble();

                qDebug() << "gotma:" << lat << lon;
                setLocation();

                success = true;
            }
        }
        callback(success);
        reply->deleteLater();
    });

}


