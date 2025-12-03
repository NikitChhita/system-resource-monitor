#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Real-Time-System-Monitor");
    app.setApplicationDisplayName("Real-Time-System-Monitor");
    app.setDesktopFileName("Real-Time-System-Monitor");


    MainWindow window;
    window.setWindowTitle("Real-Time-System-Monitor");
    window.resize(1000, 600);
    window.show();

    return app.exec();
}
