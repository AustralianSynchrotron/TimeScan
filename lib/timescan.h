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
#include <QLabel>
#include <QTableWidget>


#include <qtpv.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>

#include <blitz/array.h>

typedef blitz::Array<double,1> Line;










namespace Ui {
    class TimeScan;
}

class QChartMX : public QWidget {
  Q_OBJECT;

public:

  QChartMX(QWidget *parent = 0);
  ~QChartMX();

  double interval() const;
  double period() const;
  bool isContinious() const;
  QString saveDir() const;
  QString saveName() const;
  bool isAutoName() const;
  double min() const;
  bool isAutoMin() const;
  double max() const;
  bool isAutoMax() const;
  bool isLogarithmic() const;
  bool isNormalized() const;
  bool isGridVisible() const;
  bool isControlCollapsed() const;

  QStringList allSignals() const ;
  bool isRunning() const ;


public slots:

  void setInterval(double val);
  void setPeriod(double val);
  void setContinious(bool val);
  void setSaveDir(const QString & val);
  void setSaveName(const QString & val=QString());
  void setAutoName(bool val);
  void addSignal(const QString & pvName=QString());
  void removeSignal(const QString & pvName=QString());
  void saveResult(const QString & resFile=QString());
  void setMin(double val);
  void setAutoMin(bool val);
  void setMax(double val);
  void setAutoMax(bool val);
  void setLogarithmic(bool val);
  void setNormalized(bool norm);
  void setGridVisible(bool val);
  void setControlCollapsed(bool val);
  void lock(bool val);
  void start();
  void stop();


private:

  double dataMin();
  double dataMax();

  static const QStringList knownDetectors;
  static QStringList initDetectors();
  static const QString qtiCommand;
  static QString initQti();

  static const QString badStyle;
  static const QString goodStyle;

  QList<QColor> colorsLeft;

  Ui::TimeScan *ui;

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

private slots:

  void browseAutoSave();
  void printResult();
  void openQti();
  void startStop();
  void preparePlot();
  void getData();
  void logScale();
  void setRanges();

signals:

  void configurationChanged();

};



class QChartMX::Signal : public QObject {
  Q_OBJECT;

private:

  double _min;
  double _max;
  QEpicsPv * _pv;
  QEpicsPv * _desc;
  Line data;
  Line normal_data;
  const Line * xData; // from the parent
  bool normalized;
  bool logscaled;
  void preparePlot();

public:

  static QStringList knownDetectors;
  QPushButton * rem;
  QComboBox * sig;
  QLabel * val;
  QTableWidgetItem * tableItem;
  QwtPlotCurve * curve;

  Signal(QChartMX* parent=0);
  ~Signal();

  QVariant get();
  inline const QString & pv() const {return _pv->pv();};
  inline double min() const {return _min;}
  inline double max() const {return _max;}
  void resetData();
  inline void setNormalized(bool nrm) {normalized=nrm; preparePlot(); }
  inline void setLogarithmic(bool log) {logscaled=log; preparePlot(); }

private slots:

  inline void setPV(const QString & pvname) {
    _pv->setPV(pvname);
    _desc->setPV(pvname+".DESC");
  }

  inline void setConnected(bool con) {
    rem->setStyleSheet( con ? "" :
                        "background-color: rgba(255, 0, 0,64);");
    val->setText( con ? "" : "disconnected");
  }

  inline void setHeader() {
    QString header = _pv->pv();
    if (_desc->isConnected())
      header += '\n' + _desc->get().toString();
    tableItem->setText(header);
  }


  inline void updateValue(const QVariant & data) {
    val->setText(data.toString());
  }

};


#endif // QCHARTMX_H
