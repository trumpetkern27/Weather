#ifndef CITYPROMPT_H
#define CITYPROMPT_H

#include <QWidget>
#include <QLineEdit>
#include <QKeyEvent>
#include <QVBoxLayout>

namespace Ui {
class cityPrompt;
}

class cityPrompt : public QWidget
{
    Q_OBJECT

public:
    explicit cityPrompt(QWidget *parent = nullptr);
    ~cityPrompt();
    void showError();
signals:
    void cityEntered(const QString &city);
private:
    Ui::cityPrompt *ui;

protected:
    //void keyPressEvent(QKeyEvent *event) override;
};

#endif // CITYPROMPT_H
