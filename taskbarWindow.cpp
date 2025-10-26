#include "taskbarWindow.h"
#include <windows.h>
#include "helpers.h"
#include "mainwindow.h"


int posX, posY, tBarHeight, width, height;

QTimer *topmostTimer;

weather::weather() {

    setWindowTitle("Weather");


    // get default pos if sttings corrupted
    tBarHeight = get_taskbar_height();
     QRect screenGeometry = QApplication::primaryScreen()->availableGeometry();
    int x = screenGeometry.right() - 1000;
    int y = screenGeometry.bottom();
    setGeometry(100,100,150,tBarHeight);
    setWindowFlags(Qt::ToolTip | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::WindowDoesNotAcceptFocus);
    setStyleSheet("background-color: red");
    setMouseTracking(true);

    // load pos
    loadPosition(x, y);

    // set timer to enforce always on top
    topmostTimer = new QTimer(this);
    connect(topmostTimer, &QTimer::timeout, this, &weather::enforceTopmost);
    topmostTimer->start(10);

    // init main window
    main = new MainWindow(this);

}


weather::~weather() {
    topmostTimer->stop();
}

// show main window
void weather::enterEvent(QEnterEvent *event) {
    main->show();
}

// allow for exit horizontally
void weather::leaveEvent(QEvent *event) {

    if (main->isVisible()) {
        QPoint cur = QCursor::pos();
        if (cur.x() < posX || cur.x() >= posX + width) {
            main->hide();
        }
    }
}



// load saved position from first intance, otherwise fallback to something apparently safe
void weather::loadPosition(int x, int y) {
    QSettings settings("weatherWidget", "weather");
    posX = settings.value("winX", x).toInt();
    posY = settings.value("winY", y).toInt();
    width = settings.value("width", 150).toInt();
    height = settings.value("height", tBarHeight).toInt();

    setGeometry(posX, posY, width, height);
\
}


//ensure window is topmost on taskbar
void weather::enforceTopmost() {
    HWND hwnd = (HWND)winId();
    SetWindowPos(hwnd, HWND_TOPMOST, posX, posY, width, height, SWP_NOACTIVATE);
}




