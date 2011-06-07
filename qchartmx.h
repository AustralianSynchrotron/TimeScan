#ifndef QCHARTMX_H
#define QCHARTMX_H

#include <QMainWindow>
#include <QSettings>
#include <QList>
#include <QHash>
#include <QDebug>
#include <QProcess>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QTimer>
#include <QFile>
#include <QColor>
#include <QPen>
#include <poptmx.h>


#include "ui_qchartmx.h"

#include <qtpv.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>

#include <blitz/array.h>

typedef blitz::Array<double,1> Line;



struct clargs {

  std::string command;
  bool start;
  double interval;
  double period;
  bool cont;
  std::string saveDir;
  std::string saveName;
  std::vector<std::string> detectors;
  double min;
  double max;
  bool log;
  bool norma;
  bool grid;
  bool collapseControl;

  poptmx::OptionTable table;

  clargs(int argc, char *argv[]);
};










namespace Ui {
    class TimeScanMX;
}

class QChartMX : public QMainWindow {
  Q_OBJECT;

public:

  QChartMX(const clargs & args, QWidget *parent = 0);
  ~QChartMX();

private:

  QString qtiCommand;
  QSettings * localSettings;
  static QSettings * globalSettings;
  static const QString badStyle;
  static const QString goodStyle;

  QList<QColor> colorsLeft;

  Ui::TimeScanMX *ui;

  QTimer * timer;

  Line timeData;
  int point;
  QwtPlotGrid * grid;

  class Signal;
  QList<Signal*> signalsE;
  void constructSignalsLayout();

  int column(Signal* sig) const;

  QString tableWasSavedTo;
  QFile dataFile;
  QTextStream dataStr;

  bool isLog() {return ui->logY->isChecked() ;}
  bool isNorm() {return ui->norma->isChecked() ;}
  double min();
  double max();

private slots:

  void browseAutoSave();
  QString prepareAutoSave();
  void printResult();
  void saveResult();
  void startStop();
  void addSignal(const QString & pvName="");
  void removeSignal();
  void storeSettings();
  void openQti();
  void preparePlot();
  void updateTimes();
  void getData();
  void showGrid(bool show);
  void normalizePlots(bool norm);
  void logScale();
  void setRanges();

};



class QChartMX::Signal : public QObject {
  Q_OBJECT;

public:

  static QStringList knownDetectors;
  QPushButton * rem;
  QComboBox * sig;
  QLabel * val;
  QTableWidgetItem * tableItem;
  QEpicsPv * pv;
  QwtPlotCurve * curve;
  QPen pen;
  Line data;
  Line normal_data;
  static Line * xData; // dont forget to reinit
  double min;
  double max;
  bool normalized;
  bool logscaled;

public:

  Signal(QWidget* parent=0);
  ~Signal();

  inline void needUpdated() {pv->needUpdated();}
  inline double getUpdated() { return pv->getUpdated().toDouble(); }

  double get();

private slots:

  inline void setConnected(bool con) {
    rem->setStyleSheet( con ? "" :
                        "background-color: rgba(255, 0, 0,64);");
    val->setText( con ? "" : "disconnected");
    emit connectionChanged(con);
  }

  inline void setHeader(const QString & text) {
    tableItem->setText(text);
  }

public slots:

  void preparePlot(bool norma, bool log);
  void resetData(bool norm, bool log);
  inline void updateValue(const QVariant & data) { val->setText(data.toString()); }

private:

  //int column(Signal * sig) const;

  double prep(double value);
  void prep();

signals:

  void remove();
  void connectionChanged(bool);

};


#endif // QCHARTMX_H
