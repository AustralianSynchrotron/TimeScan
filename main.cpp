#include <QtGui/QApplication>
#include "qchartmx.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    clargs args(argc, argv);
    QChartMX w(args);
    w.show();
    return a.exec();
}
