#include "main_window.h"
#include <QApplication>

#include <QStyle>
#include <QDesktopWidget>

int main(int argc, char *argv[])
{   
    QApplication app(argc, argv);
    app.setOrganizationName("gnss-sdr");
    app.setOrganizationDomain("gnss-sdr.org");
    app.setApplicationName("gnss-sdr-monitor");

    Main_Window w;
    w.show();

    return app.exec();
}
