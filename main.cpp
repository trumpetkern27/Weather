#include "mainwindow.h"

#include <QApplication>
#include "taskbarWindow.h"
#include <QSettings>
#include <windows.h>
#include <cstdlib>
#include "first_instance.h"
#include <QDialog>
#include <QThread>
#include "fetch.h"
#include "settings_data.h"

void check_first_instance();
bool first_instance_setup();


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setQuitOnLastWindowClosed(false); // ensure child windows don't shut everything down

    check_first_instance();

    weather *w = new weather();
    w->setAttribute(Qt::WA_DeleteOnClose);
    w->show();

    return a.exec();
}

// check if first instance of running
void check_first_instance(){
    QSettings settings("weatherWidget", "weather");
    if (!settings.contains("first_instance")) {
        if ( first_instance_setup() ) {
            settings.setValue("first_instance", true);

            QCoreApplication::processEvents();
            QThread::msleep(100);
            return;
        } else {
            exit(0);
        }
    }
}



// setup if first time, ensures returns true after successful completion
bool first_instance_setup() {

    first_instance *f = new first_instance;

    f->show();
    f->exec();
    delete f;
    return true;
}
