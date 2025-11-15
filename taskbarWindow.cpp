#include "taskbarWindow.h"
#include <windows.h>
#include "helpers.h"
#include "mainwindow.h"
#include <vector>


int posX, posY, tBarHeight, width, height;

QTimer *topmostTimer;

static bool GetWindowRectDwm(HWND hwnd, RECT* rc)
{
    if (GetWindowRect(hwnd, rc))
        return true;
    return SUCCEEDED(DwmGetWindowAttribute(hwnd,
                                           DWMWA_EXTENDED_FRAME_BOUNDS, rc, sizeof(RECT)));
}

// ----------------------------------------------------------------
//  Global data that lives for the whole EnumWindows call
static struct FullscreenEnumData {
    HWND me;
    HMONITOR mon;
    RECT monitor;
    int margin;
    bool found = false;
} g_enumData;

// ----------------------------------------------------------------
//  C-style callback – NO CAPTURES, NO REFERENCES
static BOOL CALLBACK EnumFullscreenProc(HWND hwnd, LPARAM)
{
    if (hwnd == g_enumData.me) return TRUE;

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid == GetCurrentProcessId()) return TRUE;

    LONG style = GetWindowLongW(hwnd, GWL_STYLE);
    if (style & WS_CHILD) return TRUE;

    RECT rc;
    if (!GetWindowRectDwm(hwnd, &rc)) return TRUE;

    if (MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST) != g_enumData.mon)
        return TRUE;

    const RECT& m = g_enumData.monitor;
    if (rc.left   <= m.left   + g_enumData.margin &&
        rc.top    <= m.top    + g_enumData.margin &&
        rc.right  >= m.right  - g_enumData.margin &&
        rc.bottom >= m.bottom - g_enumData.margin) {

        // ---- skip known system windows ----
        wchar_t cls[256] = {};
        GetClassNameW(hwnd, cls, _countof(cls));

        if (wcscmp(cls, L"Progman") == 0 ||
            wcscmp(cls, L"WorkerW") == 0 ||
            wcsstr(cls, L"Tray") != nullptr ||
            wcsstr(cls, L"Shell_") != nullptr)
            return TRUE;

        // ---- ignore maximized (not fullscreen) ----
        WINDOWPLACEMENT wp{ sizeof(wp) };
        if (GetWindowPlacement(hwnd, &wp) && wp.showCmd == SW_SHOWMAXIMIZED)
            return TRUE;

        g_enumData.found = true;
        return FALSE;               // stop enumerating
    }
    return TRUE;
}

// ----------------------------------------------------------------
//  PUBLIC – called from timer
bool IsAnyWindowFullscreenOnMyMonitor(HWND myHwnd)
{
    // ---- 1. my monitor ------------------------------------------------
    HMONITOR mon = MonitorFromWindow(myHwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    GetMonitorInfoW(mon, &mi);

    // ---- 2. foreground fast-path (catches YouTube instantly) ----------
    HWND fg = GetForegroundWindow();
    if (fg && fg != myHwnd) {
        RECT rc;
        if (GetWindowRectDwm(fg, &rc) &&
            MonitorFromWindow(fg, MONITOR_DEFAULTTONEAREST) == mon &&
            rc.left   <= mi.rcMonitor.left   + 20 &&
            rc.top    <= mi.rcMonitor.top    + 20 &&
            rc.right  >= mi.rcMonitor.right  - 20 &&
            rc.bottom >= mi.rcMonitor.bottom - 20) {

            // **Do NOT check SW_SHOWMAXIMIZED here** – YouTube is NOT maximized
            return true;
        }
    }

    // ---- 3. full enumeration -----------------------------------------
    g_enumData = { myHwnd, mon, mi.rcMonitor, 20, false };
    EnumWindows(EnumFullscreenProc, 0);
    return g_enumData.found;
}



weather::weather() {

    setWindowTitle("Weather");


    // get default pos if sttings corrupted
    tBarHeight = get_taskbar_height();
     QRect screenGeometry = QApplication::primaryScreen()->availableGeometry();
    int x = screenGeometry.right() - 1000;
    int y = screenGeometry.bottom();
    setGeometry(100,100,150,tBarHeight);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::ToolTip | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::WindowDoesNotAcceptFocus);
    //setStyleSheet("background-color: red");
    setMouseTracking(true);




    // load pos
    loadPosition(x, y);

    // label for data
    label = new QLabel("loading...", this);
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("color: white; font-size: 14px; background: transparent;");
    label->setGeometry(0, 0, width, height);
    label->raise();
    // set timer to enforce always on top
    topmostTimer = new QTimer(this);
    connect(topmostTimer, &QTimer::timeout, this, &weather::enforceTopmost);
    topmostTimer->start(200);

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
    bool isFullscreen = IsAnyWindowFullscreenOnMyMonitor(hwnd);

    LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    bool hasTopmost = (exStyle & WS_EX_TOPMOST) != 0;

    if (isFullscreen && hasTopmost) {
        SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle & ~WS_EX_TOPMOST);
        SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_FRAMECHANGED);
        label->hide();
    }
    else if (!isFullscreen && !hasTopmost) {
        SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle | WS_EX_TOPMOST);
        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_FRAMECHANGED);
        label->show();
    }


}




