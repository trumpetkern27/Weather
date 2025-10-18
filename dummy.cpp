#include "dummy.h"
#include "helpers.h"

dummy::dummy() {
    int tBarHeight = get_taskbar_height();

    QRect screenGeometry = QApplication::primaryScreen()->availableGeometry();
    int x = screenGeometry.right() - 1000;
    int y = screenGeometry.bottom();

    setGeometry(100,100,100,tBarHeight);
    setWindowFlags(Qt::Popup | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
    setStyleSheet("background-color: lightblue");
    move(x,y);
    /*setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_MouseTracking, true);*/
    setMouseTracking(true);

    show();

    //flash
    isBlue = true;
    dTimer = new QTimer(this);
    connect(dTimer, &QTimer::timeout, this, &dummy::flash);
    dTimer->start(500);
}

dummy::~dummy() {
    dTimer->stop();
}

void dummy::flash() {
    if (isBlue) {
        setStyleSheet("background-color: white");

    } else {
        setStyleSheet("background-color: lightblue");
    }

    isBlue = !isBlue;

}

void dummy::mousePressEvent(QMouseEvent *event) {
    MessageBox(NULL, L"Mouse pressed!", L"Debug", MB_OK);
    isDragging = true;
    dragStart = event->globalPos();
    windowStart = frameGeometry().topLeft();
}

void dummy::mouseMoveEvent(QMouseEvent *event) {
    if (isDragging) {
        QPoint d = event->globalPos() - dragStart;
        move(windowStart + d);
    }
}

void dummy::mouseReleaseEvent(QMouseEvent *event) {
    isDragging = false;
}

void dummy::enterEvent(QEnterEvent *event) {
    MessageBox(NULL, L"ligma", L"", 0);
}
