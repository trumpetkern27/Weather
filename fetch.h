// fetch.h
#ifndef FETCH_H
#define FETCH_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QGeoPositionInfoSource>
#include <QSettings>
#include <QTimer>

class fetch : public QObject
{
    Q_OBJECT
public:
    explicit fetch(QObject *parent = nullptr);
    void promptCity();
    void getForecast(const QString &forecastUrl);
public slots:
    void getWeather();
signals:
    void weatherDataReceived(const QByteArray &data);
    void errorOccurred(const QString &error);

private:
    QNetworkAccessManager *manager;
    void export_json();
    void getLocation();
    void setLocation();
    void loadLocation();
    void getWeatherData();

    void getCityCoordinates(const QString &cityName, std::function<void(bool)> callback);
    double lat = 500, lon = 500;
};

#endif // FETCH_H
