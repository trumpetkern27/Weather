#include "cityprompt.h"
#include "ui_cityprompt.h"

cityPrompt::cityPrompt(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::cityPrompt)
{
    ui->setupUi(this);

    ui->errorLabel->setVisible(false);

    // allow normal enter event
    connect(ui->cityBox, &QLineEdit::returnPressed, this, [this]() {
        QString city = ui->cityBox->text();
        if (!city.isEmpty()) {
            emit cityEntered(city);
        }
    });

    // clear error message once continue typing
    connect(ui->cityBox, &QLineEdit::textChanged, this, [this]() {
        if (ui->errorLabel->isVisible()) {
            ui->errorLabel->setVisible(false);
        }
    });
}

cityPrompt::~cityPrompt()
{
    delete ui;
}

void cityPrompt::showError() {
    ui->errorLabel->setVisible(true);
}

/*
void cityPrompt::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        QString city = ui->cityBox->text();
        if (!city.isEmpty()) {
            emit cityEntered(city);
        }
    }
    QWidget::keyPressEvent(event);
}

*/

