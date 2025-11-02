#include "ui_first_instance.h"

#include "first_instance.h"
#include "helpers.h"


first_instance::first_instance(QWidget *parent)
    : QDialog(parent), ui(new Ui::Form()), dummy(new QWidget())
{


    ui->setupUi(this);

    ui->widthSlider->setValue(150);

    // make dummy box

    int tBarHeight = get_taskbar_height();
    QRect screenGeometry = QApplication::primaryScreen()->availableGeometry();
    int x = screenGeometry.right() - 1000;
    int y = screenGeometry.bottom();
    dummy->setGeometry(100, 100, 150, tBarHeight);
    dummy->setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
    dummy->setStyleSheet("background-color: blue");
    dummy->move(x, y);
    dummy->setFocusPolicy(Qt::StrongFocus);
    dummy->setAttribute(Qt::WA_Hover, true);
    dummy->setMouseTracking(true);
    dummy->show();
    dummy->installEventFilter(this);

    // link timer to flash
    t = new QTimer();
    connect(t, &QTimer::timeout, this, &first_instance::flash);
    t->start(500);

    // connect width resize
    connect(ui->widthSlider, &QSlider::valueChanged,
            this, &first_instance::resizeDummy);

    // connect auto location checkbox
    connect(ui->locationDetect, &QCheckBox::checkStateChanged, this, &first_instance::saveSettings);

    // connect reset button
    connect(ui->resetButton, &QPushButton::clicked, this, &first_instance::reset);

}

first_instance::~first_instance() {

    if (t) {
        t->stop();
        delete t;
        t = nullptr;
    }
    if (dummy) {
        dummy->deleteLater();
        dummy = nullptr;
    }
    delete ui;
}


// make the box flash
void first_instance::flash() {

    if (!dummy || !t) {
        return; // in case dummy or t isn't destroyed properly for whatever reason
    }

    if (isBlue) {
        dummy->setStyleSheet("background-color: red");
    } else {
        dummy->setStyleSheet("background-color: blue");
    }
    isBlue = !isBlue;
    enforceTopmost();
    dummy->update();
}


// handle movement
bool first_instance::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == dummy)
    {
        switch (event->type())
        {


        case QEvent::MouseButtonPress:
        {
            QMouseEvent *me = static_cast<QMouseEvent*>(event);
            isDragging = true;
            dragStart = me->globalPos();
            windowStart = dummy->frameGeometry().topLeft();

            break;
        }
        case QEvent::MouseButtonRelease:
        {
            isDragging = false;

            saveSettings();
            break;
        }
        case QEvent::MouseMove:
        {
            if (isDragging) {
                QMouseEvent *me = static_cast<QMouseEvent*>(event);
                QPoint delta = me->globalPos() - dragStart;
                dummy->move(windowStart.x() + delta.x(), windowStart.y());
            }
            break;
        }

        default:
            break;
        }
    }

    return QDialog::eventFilter(obj, event);
}


// reset
void first_instance::reset() {

    int tBarHeight = get_taskbar_height();
    QRect screenGeometry = QApplication::primaryScreen()->availableGeometry();
    int x = screenGeometry.right() - 1000;
    int y = screenGeometry.bottom();
    dummy->setGeometry(100, 100, 150, tBarHeight);
    dummy->move(x, y);

    ui->widthSlider->setValue(150);
    ui->locationDetect->setCheckState(Qt::Unchecked);

    saveSettings();

}


void first_instance::resizeDummy(int value) {
    if (!dummy)
        return;
    QRect geom = dummy->geometry();
    geom.setWidth(value);
    dummy->setGeometry(geom);

    saveSettings();

}


void first_instance::saveSettings() {
    // save to settings
    QSettings settings("weatherWidget", "weather");
    settings.setValue("winX", dummy->x());
    settings.setValue("winY", dummy->y());
    settings.setValue("width", dummy->width());
    settings.setValue("height", dummy->height());
    settings.setValue("autoLocation", ui->locationDetect->checkState());
    settings.setValue("uom", "C"); // yes, celsius is superior
}

//ensure window is topmost on taskbar
void first_instance::enforceTopmost() {
    HWND hwnd = (HWND)dummy->winId();
    SetWindowPos(hwnd, HWND_TOPMOST, dummy->x(), dummy->y(), dummy->width(), dummy->height(), SWP_NOACTIVATE);
}
