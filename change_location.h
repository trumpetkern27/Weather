#ifndef CHANGE_LOCATION_H
#define CHANGE_LOCATION_H

#include <QWidget>
#include <QSettings>

class fetch;

namespace Ui {
class change_location;
}

class change_location : public QWidget
{
    Q_OBJECT

public:
    explicit change_location(QWidget *parent = nullptr);
    ~change_location();
    void showError();
signals:
    void cityEntered(const QString &city);

private:
    Ui::change_location *ui;
};


#endif // CHANGE_LOCATION_H
