#ifndef SETTINGS_DATA_H
#define SETTINGS_DATA_H

#include <QWidget>
#include <QSettings>

QT_BEGIN_NAMESPACE
namespace Ui {
class settings_data;
}
QT_END_NAMESPACE

class settings_data : public QWidget
{
    Q_OBJECT

public:
    explicit settings_data(QWidget *parent = nullptr);
    ~settings_data();

    QMap<QString, bool> getSettings() const;
    void loadSettings();
private slots:
    void on_selectAllButton_click();
    void on_deselectAllButton_click();
    void on_applyButton_click();
signals:
    void settingsApplied(const QMap<QString, bool>& settings);
private:
    Ui::settings_data *ui;


};

#endif // SETTINGS_DATA_H
