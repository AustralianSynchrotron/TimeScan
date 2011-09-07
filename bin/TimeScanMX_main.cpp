#include <QtGui/QApplication>
#include "TimeScanMX_mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w(argc, argv);
    w.show();

    return a.exec();
}
