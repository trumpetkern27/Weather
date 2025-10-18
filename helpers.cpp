// helpers.cpp
#include "helpers.h"
#include <windows.h>

int get_taskbar_height() {
    RECT tb;
    HWND hTaskbar = FindWindow(L"Shell_TrayWnd", NULL);
    GetWindowRect(hTaskbar, &tb);
    return tb.bottom - tb.top;
}
