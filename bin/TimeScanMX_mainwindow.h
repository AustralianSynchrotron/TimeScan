#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <poptmx.h>
#include "timescan.h"



namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{

  Q_OBJECT;

  QChartMX * chart;
  static QSettings localSettings;

public:
  explicit MainWindow(int argc, char *argv[], QWidget *parent = 0);
  ~MainWindow();

private:
  Ui::MainWindow *ui;

private slots:
  void updateConfiguration();
};

#endif // MAINWINDOW_H
