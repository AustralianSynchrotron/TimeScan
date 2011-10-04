#include <QFileDialog>
#include <QFileInfo>
#include <QDir>
#include <QPrinter>
#include <QDate>
#include <QPrintDialog>
#include <QTime>

#include "timescan.h"
#include "ui_timescan.h"

#include <qwt_scale_draw.h>
#include <qwt_scale_engine.h>
#include <qwt_symbol.h>
#include <qwt_plot_renderer.h>



class TimeScaleDraw: public QwtScaleDraw {
public:
  TimeScaleDraw(const QTime &base, double interv=1.0):
    QwtScaleDraw(),
    baseTime(base), interval(interv) {}
  virtual QwtText label(double v) const {
    QTime utime = baseTime.addMSecs( (int)(1000*v*interval) );
    return utime.toString("hh:mm:ss");
  }
private:
  QTime baseTime;
  double interval;
};



const QString QChartMX::qtiCommand = QChartMX::initQti();
const QStringList QChartMX::knownDetectors = QChartMX::initDetectors();
const QString QChartMX::badStyle = "background-color: rgba(255, 0, 0, 64);";
const QString QChartMX::goodStyle = QString();


QChartMX::QChartMX(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TimeScan),
    timer(new QTimer(this)),
    timeData()
{

  colorsLeft
      << Qt::red
      << Qt::blue
      << Qt::green
      << Qt::cyan
      << Qt::magenta
      << Qt::yellow
      << Qt::darkRed
      << Qt::darkBlue
      << Qt::darkGreen
      << Qt::darkCyan
      << Qt::darkMagenta
      << Qt::darkYellow;
  colorsLeft.removeAll(QApplication::palette().color(QPalette::Text));

  ui->setupUi(this);
  ui->splitter->setCollapsible(0, true);

  ui->plot->setAutoReplot(false);
  ui->plot->setAxisMaxMinor(QwtPlot::yLeft,  10);
  ui->qtiResults->setVisible( ! qtiCommand.isEmpty() );
  grid = new QwtPlotGrid;
  grid->enableXMin(true);
  grid->enableYMin(true);
  grid->setMajPen(Qt::DashLine);
  grid->setMinPen(Qt::DotLine);
  setGridVisible(isGridVisible());

  connect(ui->addSignal, SIGNAL(clicked()), SLOT(addSignal()));
  connect(ui->startStop, SIGNAL(clicked()), SLOT(startStop()));
  connect(ui->browseSaveDir, SIGNAL(clicked()), SLOT(browseAutoSave()));
  connect(ui->printResult, SIGNAL(clicked()), SLOT(printResult()));
  connect(ui->saveResult, SIGNAL(clicked()), SLOT(saveResult()));
  connect(ui->qtiResults, SIGNAL(clicked()), SLOT(openQti()));
  connect(ui->saveDir, SIGNAL(textChanged(QString)), SLOT(setSaveDir(QString)));
  connect(ui->saveName, SIGNAL(textChanged(QString)), SLOT(setSaveName(QString)));
  connect(ui->autoName, SIGNAL(toggled(bool)), SLOT(setAutoName(bool)));

  connect(ui->showGrid, SIGNAL(toggled(bool)), SLOT(setGridVisible(bool)));
  connect(ui->norma, SIGNAL(toggled(bool)), SLOT(setNormalized(bool)));
  connect(ui->logY, SIGNAL(toggled(bool)), SLOT(logScale()));
  connect(ui->autoMin, SIGNAL(toggled(bool)), SLOT(setRanges()));
  connect(ui->autoMax, SIGNAL(toggled(bool)), SLOT(setRanges()));
  connect(ui->min, SIGNAL(editingFinished()), SLOT(setRanges()));
  connect(ui->max, SIGNAL(editingFinished()), SLOT(setRanges()));
  connect(ui->interval, SIGNAL(valueChanged(double)), SLOT(setInterval(double)));
  connect(ui->period, SIGNAL(valueChanged(double)), SLOT(setPeriod(double)));

  connect(timer, SIGNAL(timeout()), SLOT(getData()));

  setSaveDir(QDir::homePath());

  preparePlot();

}

QChartMX::~QChartMX() {
  delete ui;
  delete timer;
  blockSignals(true);
  while ( ! signalsE.isEmpty() )
    removeSignal(signalsE.at(0)->pv());
  blockSignals(false);
}

QString QChartMX::initQti() {
  QProcess checkQti;
  checkQti.start("bash -c \"command -v qtiplot\"");
  checkQti.waitForFinished();
  QString comm = checkQti.readAll();
  comm.chop(1);
  return comm;
}

QStringList QChartMX::initDetectors() {
  QFile detFile("/etc/listOfSignals.txt");
  QStringList ret;
  if ( detFile.open(QIODevice::ReadOnly | QIODevice::Text) &&
       detFile.isReadable() )
    while ( !detFile.atEnd() ) {
      QString ln = detFile.readLine();
      if ( ln.at(ln.length()-1) == '\n')
        ln.chop(1);
      ret << ln;
    }
  return ret;
}

double QChartMX::interval() const {
  return ui->interval->value();
}

double QChartMX::period() const {
  return ui->period->value();
}

bool QChartMX::isContinious() const {
  return ui->cont->isChecked();
}

QString QChartMX::saveDir() const {
  QString dn = ui->saveDir->text();
  if ( ! dn.endsWith('/') )
    dn += "/";
  return dn;
}

QString QChartMX::saveName() const {
  return ui->saveName->text();
}

bool QChartMX::isAutoName() const {
  return ui->autoName->isChecked();
}

double QChartMX::min() const {
  return ui->min->value();
}

bool QChartMX::isAutoMin() const {
  return ui->autoMin->isChecked();
}

double QChartMX::max() const {
  return ui->max->value();
}

bool QChartMX::isAutoMax() const {
  return ui->autoMax->isChecked();
}


bool QChartMX::isLogarithmic() const {
  return ui->logY->isChecked();
}

bool QChartMX::isNormalized() const {
  return ui->norma->isChecked();
}

bool QChartMX::isGridVisible() const {
  return ui->showGrid->isChecked();
}

bool QChartMX::isControlCollapsed() const {
  return ui->control->isVisible();
}


QStringList QChartMX::allSignals() const  {
  QStringList sigs;
  foreach (Signal * sig, signalsE)
    sigs << sig->pv();
  return sigs;
}

bool QChartMX::isRunning() const  {
  return timer->isActive();
}

void QChartMX::setInterval(double val) {
  if (sender() != ui->interval) {
    ui->interval->setValue(val);
    return; // will return here from the ui->saveDir->textChanged signal.
  }
  if ( val*2 > period() )
    setPeriod(2*val);
  preparePlot();
  emit configurationChanged();
}

void QChartMX::setPeriod(double val) {
  if ( sender() != ui->period ) {
    ui->period->setValue(val);
    return; // will return here from the ui->saveDir->textChanged signal.
  }
  if ( interval()*2 > val )
    setInterval(val/2);
  preparePlot();
  emit configurationChanged();
}

void QChartMX::setContinious(bool val) {
  ui->cont->setChecked(val);
}

void QChartMX::setSaveDir(const QString & val) {
  if (sender() != ui->saveDir ) {
    ui->saveDir->setText(val);
    return; // will return here from the ui->saveDir->textChanged signal.
  }
  QFileInfo dirInfo(val);
  bool dirOK = dirInfo.exists() && dirInfo.isWritable();
  ui->saveDir->setStyleSheet( dirOK  ?  goodStyle  :  badStyle );
  emit configurationChanged();
}

void QChartMX::setSaveName(const QString & val) {
  if (sender() != ui->saveName ) {
    ui->saveName->setText(val);
    return; // will return here from the ui->saveDir->textChanged signal.
  }
  QFileInfo fileInfo( saveDir() + val );
  bool fileOK = ! fileInfo.exists() ||
                ( ! fileInfo.isDir() && fileInfo.isWritable() );
  ui->saveName->setStyleSheet( fileOK  ?  goodStyle  :  badStyle );
  emit configurationChanged();
}

void QChartMX::setAutoName(bool val) {

  if ( sender() != ui->autoName ) {
    ui->autoName->setChecked(val);
    return; // will return here from the ui->autoName->toggeled signal.
  }

  if (val) {
    const QString fn = "time_scan_" +
        QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss");
    const QString ext = ".dat";

    int count = 1;
    QString dn = saveDir();
    QString tryName = fn + ext;
    while ( QFile::exists( dn + tryName ) )
      tryName = fn + "_(" + QString::number(count++) + ")" + ext;
    setSaveName(tryName);
  }

  emit configurationChanged();

}

void QChartMX::setMin(double val) {
  setAutoMin(false);
  ui->min->setValue(val);
  setRanges();
}

void QChartMX::setAutoMin(bool val) {
  ui->autoMin->setChecked(val);
}

void QChartMX::setMax(double val) {
  setAutoMax(false);
  ui->max->setValue(val);
  setRanges();
}

void QChartMX::setAutoMax(bool val) {
  ui->autoMax->setChecked(val);
}


void QChartMX::addSignal(const QString & pvName) {

  Signal * sg = new Signal(this);
  sg->sig->addItems(knownDetectors);
  sg->sig->addItem(pvName);
  sg->sig->setCurrentIndex( sg->sig->findText(pvName) );

  QColor sigcolor = colorsLeft.size() ?
        colorsLeft.takeFirst() :
        QApplication::palette().color(QPalette::Text);

  QPen pen = sg->curve->pen();
  pen.setColor(sigcolor);
  pen.setWidth(2);
  sg->curve->setPen(pen);

  QwtSymbol * symbol = new QwtSymbol(*sg->curve->symbol());
  symbol->setPen(pen);
  sg->curve->setSymbol(symbol);

  connect(sg->rem, SIGNAL(clicked()), SLOT(removeSignal()));

  sg->tableItem = new QTableWidgetItem(pvName);
  signalsE.append(sg);
  int tablePos = ui->dataTable->columnCount();
  ui->dataTable->insertColumn(tablePos);
  sg->tableItem->setTextColor(sigcolor);
  ui->dataTable->setHorizontalHeaderItem(tablePos, sg->tableItem);

  sg->sig->setStyleSheet("color: " + sigcolor.name() + ";");

  constructSignalsLayout();

  preparePlot();
  sg->curve->attach(ui->plot);
  ui->plot->replot();

  emit configurationChanged();

}

void QChartMX::removeSignal(const QString & pvName) {

  Signal * sg = 0;
  if (pvName.isEmpty()) {
    foreach(Signal * sig, signalsE)
      if ( sig->rem == sender() )
        sg = sig;
  } else {
    foreach(Signal * sig, signalsE)
      if (sig->pv() == pvName)
        sg = sig;
  }
  if (!sg)
    return;

  QPen pen = sg->curve->pen();

  if (pen.color() != QApplication::palette().color(QPalette::Text))
    colorsLeft.push_back(pen.color());
  sg->curve->detach();
  ui->dataTable->removeColumn(column(sg));
  signalsE.removeOne(sg);
  sg->deleteLater();
  sg->rem->deleteLater();
  sg->sig->deleteLater();
  sg->val->deleteLater();
  constructSignalsLayout();

  ui->plot->replot();

  emit configurationChanged();

}

void QChartMX::saveResult(const QString & resFile) {
  if (tableWasSavedTo.isEmpty())
    return;
  QString dataName = resFile.isEmpty() ?
        QFileDialog::getSaveFileName(this, "Save data", saveDir() ) :
        resFile ;
  if (QFile::exists(dataName))
    QFile::remove(dataName);
  QFile::copy(tableWasSavedTo, dataName);
}

void QChartMX::setControlCollapsed(bool val) {
  ui->control->setVisible(!val);
}

void QChartMX::setGridVisible(bool show){
  if ( sender() != ui->showGrid ) {
    ui->showGrid->setChecked(show);
    return;
  }
  if (show)
    grid->attach(ui->plot);
  else
    grid->detach();
  ui->plot->replot();
  emit configurationChanged();
}

void QChartMX::setLogarithmic(bool val) {
  ui->logY->setChecked(val);
  emit configurationChanged();
}

void QChartMX::setNormalized(bool norm){
  if ( sender() != ui->norma ) {
    ui->norma->setChecked(norm);
    return;
  }
  foreach (Signal * sig, signalsE)
    sig->setNormalized(norm);
  ui->plot->enableAxis( QwtPlot::yLeft, ! norm );
  setRanges();
}


void QChartMX::start() {
  if ( ! isRunning() )
    startStop();
}

void QChartMX::stop() {
  if ( isRunning() )
    startStop();
}

void QChartMX::lock(bool val) {
  ui->startStop->setVisible(!val);
}




void QChartMX::browseAutoSave(){
  QString new_dir = QFileDialog::getExistingDirectory(this, "Open directory", saveDir());
  if ( ! new_dir.isEmpty() )
    setSaveDir(new_dir);
}

void QChartMX::printResult(){
  QPrinter printer;
  QPrintDialog dialog(&printer);
  if ( dialog.exec() ) {
    QwtPlotRenderer renderer;
    if ( printer.colorMode() == QPrinter::GrayScale ) {
      renderer.setDiscardFlag(QwtPlotRenderer::DiscardCanvasBackground);
      renderer.setLayoutFlag(QwtPlotRenderer::FrameWithScales);
    }
    renderer.renderTo(ui->plot, printer);
  }
}





void QChartMX::constructSignalsLayout(){
  foreach(Signal * sg, signalsE){
    int position=signalsE.indexOf(sg) + 1;
    ui->signalsL->addWidget(sg->rem,   position, 0);
    ui->signalsL->addWidget(sg->sig,    position, 1);
    ui->signalsL->addWidget(sg->val,   position, 2);
  }
  ui->addSignal->setStyleSheet( signalsE.size() ? goodStyle : badStyle );
}

int QChartMX::column(Signal * sig) const {
  for (int icur = 0 ; icur < ui->dataTable->columnCount() ; icur++ )
    if ( ui->dataTable->horizontalHeaderItem(icur)  == sig->tableItem )
      return icur;
  return -1;
}


void QChartMX::openQti() {
  if (qtiCommand.isEmpty())
    return;

  QString dataN = tableWasSavedTo + "_qti.dat";
  QFile dataFile(dataN);
  dataFile.open(QIODevice::Truncate | QIODevice::WriteOnly);
  QTextStream dataStr(&dataFile);

  dataStr << "X ";
  foreach (Signal * sig, signalsE)
    dataStr
        << sig->pv() << " ";
  dataStr << "\n";

  for ( int y = 0 ; y < ui->dataTable->rowCount(); y++ ) {
    for ( int x = 0 ; x < ui->dataTable->columnCount() ; x++ )
      dataStr << ( ui->dataTable->item(y,x) ?
                   ui->dataTable->item(y,x)->text() : "NaN") << " ";
    dataStr << "\n";
  }
  dataFile.close();

  QProcess::startDetached("qtiplot " + dataN);

}



void QChartMX::logScale() {
  foreach (Signal * sig, signalsE)
    sig->setLogarithmic(isLogarithmic());
  if ( isLogarithmic() )
    ui->plot->setAxisScaleEngine(QwtPlot::yLeft, new QwtLog10ScaleEngine);
  else
    ui->plot->setAxisScaleEngine(QwtPlot::yLeft, new QwtLinearScaleEngine);
  ui->plot->replot();
}

void QChartMX::setRanges() {

  double m=dataMin(), M=dataMax();

  if (isLogarithmic()) {
    if (M<=0.0 || isNormalized()) {
      m = ( m<=0.0 || M<=0.0 ) ? 1.0e-10 : m/M ;
      M = 1.0;
    }
  } else {
    if (isNormalized()) {
      m = 0;
      M = 1.0;
    }
  }
  if ( m == M ) {
    m = (m==0.0 ? -0.1 : m*0.9);
    M = (M==0.0 ?  0.1 : M*1.1);
  }

  if ( ! isAutoMin() )
    m = min();
  else
    ui->min->setValue(m);
  if ( ! isAutoMax() )
    M = max();
  else
    ui->max->setValue(M);

  if ( m != ui->plot->axisScaleDiv(QwtPlot::yLeft)->lowerBound() ||
       M != ui->plot->axisScaleDiv(QwtPlot::yLeft)->upperBound() )
    ui->plot->setAxisScale(QwtPlot::yLeft, m, M);

  ui->plot->replot();

  emit configurationChanged();

}


double QChartMX::dataMin() {
  double m = NAN;
  foreach (Signal * sig, signalsE)
    if ( isnan(m) || sig->min() < m)
      m = sig->min();
  return m;
}

double QChartMX::dataMax() {
  double M = NAN;
  foreach (Signal * sig, signalsE)
    if ( isnan(M) || sig->max() > M)
      M = sig->max();
  return M;
}



void QChartMX::startStop(){

  if ( isRunning() ) {

    timer->stop();
    ui->startStop->setText("Start");
    ui->control->setEnabled(true);
    if (dataFile.isOpen())
      dataFile.close();

  } else {

    preparePlot();
    point = 0;
    ui->startStop->setText("Stop");
    ui->control->setEnabled(false);

    // Data file
    if (isAutoName())
      setAutoName(true); // to update the filename
    tableWasSavedTo = saveDir() + saveName();
    dataFile.setFileName(tableWasSavedTo);
    dataFile.open(QIODevice::Truncate | QIODevice::WriteOnly);
    dataStr.setDevice(&dataFile);

    // buttons
    ui->saveResult->setEnabled(true);
    ui->printResult->setEnabled(true);
    ui->qtiResults->setEnabled(true);
    ui->norma->setEnabled(true);

    ui->dataTable->setRowCount(0);

    dataStr
        << "# Time Scan\n"
        << "#\n"
        << "# Date: " << QDate::currentDate().toString() << "\n"
        << "# Time: " << QTime::currentTime().toString() << "\n"
        << "#\n";

    dataStr
        << "# Number of scan points: " << ( isContinious() ?
                                              "continious scan" :
                                              QString::number(timeData.size()) )
        << "# Interval (sec): " << interval();

    dataStr
        << "# Signals:\n"
        << "#\n";
    foreach (Signal * sig, signalsE)
      dataStr
          << "# PV: \"" << sig->pv() << "\"\n";
    dataStr << "#\n";

    dataStr
        << "# Data columns:\n"
        << "# "
        << "%Point "
        << "%Time ";
    foreach (Signal * sig, signalsE)
      dataStr
          << "%" << sig->pv() << " ";
    dataStr << "\n";

    getData();
    timer->start( (int)(1000*interval()) );

  }

}


void QChartMX::preparePlot() {

  int points = (int) (period() / interval());

  ui->plot->setAxisScaleDraw(QwtPlot::xBottom,
                             new TimeScaleDraw(QTime::currentTime().addMSecs((int)(-period()*1000)),
                                               interval()));
  ui->plot->setAxisScale(QwtPlot::xBottom, 0, points);
  ui->plot->setAxisLabelRotation(QwtPlot::xBottom, -50.0);
  ui->plot->setAxisLabelAlignment(QwtPlot::xBottom, Qt::AlignLeft | Qt::AlignBottom);

  timeData.resize(points);
  for ( int i = 0; i < points; i++ )
      timeData(points - 1 - i) = i;

  foreach(Signal * sig, signalsE)
    sig->resetData();

  ui->plot->replot();

}


void QChartMX::getData() {

  static bool gettingData=false;
  if (gettingData)
    return;
  gettingData = true;

  const int points = timeData.size();

  QDateTime dt = QDateTime::currentDateTime();

  int table_row = point;
  if ( isContinious() && point > points-1 ) {
    ui->dataTable->removeRow(0);
    table_row=points-1;
  }

  ui->dataTable->insertRow(table_row);
  ui->dataTable->setVerticalHeaderItem
      (point, new QTableWidgetItem(QString::number(point+1)));
  if ( ! ui->dataTable->underMouse() )
  ui->dataTable->scrollToBottom();
  ui->dataTable->setItem(table_row, 0,
                         new QTableWidgetItem(dt.time().toString("hh:mm:ss.zzz")));

  dataStr << point+1 << " " << dt.toString("hh:mm:ss.zzz") << " ";

  timeData += 1;
  ui->plot->setAxisScale(QwtPlot::xBottom,
                         timeData(points-1), timeData(0));

  foreach(Signal * sig, signalsE) {
    QString value = sig->get().toString();
    QTableWidgetItem * item = new QTableWidgetItem(value);
    item->setTextColor(sig->curve->pen().color());
    ui->dataTable->setItem(table_row, column(sig), item);
    dataStr << value << " ";
  }
  dataStr <<  "\n";

  setRanges();
  ui->plot->replot();

  if ( ! isContinious() && point >= points-1 )
    startStop();
  point++;

  gettingData = false;

}










QStringList QChartMX::Signal::knownDetectors = QStringList();


QChartMX::Signal::Signal(QChartMX* parent) :
  QObject(parent),
  _min(NAN), _max(NAN),
  _pv(new QEpicsPv(this)),
  xData( & parent->timeData ),
  normalized(false),
  logscaled(false),
  rem(new QPushButton("-", parent)),
  sig(new QComboBox(parent)),
  val(new QLabel(parent)),
  tableItem(new QTableWidgetItem()),
  curve(new QwtPlotCurve)
{

  sig->setEditable(true);
  sig->setDuplicatesEnabled(false);
  sig->addItems(knownDetectors);
  sig->clearEditText();
  sig->setToolTip("PV of the signal.");

  rem->setToolTip("Remove the signal.");
  val->setToolTip("Current value.");

  curve->setStyle(QwtPlotCurve::Lines);
  QwtSymbol * symbol = new QwtSymbol(QwtSymbol::Ellipse);
  symbol->setSize(9);
  curve->setSymbol(symbol);
  curve->setPaintAttribute(QwtPlotCurve::ClipPolygons);
  curve->setPaintAttribute(QwtPlotCurve::CacheSymbols);


  connect(sig, SIGNAL(editTextChanged(QString)), _pv, SLOT(setPV(QString)));
  connect(sig, SIGNAL(editTextChanged(QString)), SLOT(setHeader(QString)));
  connect(_pv, SIGNAL(valueUpdated(QVariant)), SLOT(updateValue(QVariant)));
  connect(_pv, SIGNAL(connectionChanged(bool)), SLOT(setConnected(bool)));

}


QChartMX::Signal::~Signal(){
}


QVariant QChartMX::Signal::get() {

  double value = _pv->isConnected() ? _pv->get().toDouble() : NAN;
  const int points = xData->size();

  double deleted_value = data(points-1);
  for ( int i = points-1 ; i > 0 ; i--)
    data(i) = data(i-1);
  data(0) = value;

  double oldMin = _min , oldMax = _max;
  if ( ! isnan(deleted_value) && deleted_value <= _min )
    _min = blitz::min(data);
  if ( ! isnan(deleted_value) && deleted_value >= _max )
    _max = blitz::max(data);
  if ( ! isnan(value) && ( isnan(_min) || value < _min ) )
    _min = value;
  if ( ! isnan(value) && ( isnan(_max) || value > _max ) )
    _max = value;

  if (normalized) {
    if (_min != oldMin || _max!= oldMax) {
      preparePlot();
    } else {
      for ( int i = points-1 ; i > 0 ; i--)
        normal_data(i) = normal_data(i-1);
      if (logscaled) {
        double norma = qMax(qAbs(_min), qAbs(_max));
        if ( norma == 0.0 )
          normal_data(0) = value;
        else
          normal_data(0) = value / norma;
      } else if (_max == _min) {
        normal_data(0) = ( _max==0.0 ) ? 0.0 : 1.0 ;
      } else {
        normal_data(0) = (value-_min) / (_max-_min);
      }
    }
  }

  int dataSize = blitz::first(blitz_isnan(data));
  if (dataSize < 0)
    dataSize = xData->size();
  if (dataSize != (int) curve->dataSize())
    curve->setRawSamples(xData->data(),
                         normalized ? normal_data.data() : data.data(),
                         dataSize);

  return value;

}



void QChartMX::Signal::resetData() {
  int points = xData->size();
  _min = NAN;
  _max = NAN;
  data.resize(points);
  data = NAN;
  normal_data.resize(points);
  normal_data = NAN;
  preparePlot();
  curve->setRawSamples(xData->data(),
                       normalized ? normal_data.data() : data.data(), 0);
}


void QChartMX::Signal::preparePlot() {
  if (logscaled) {
    double norma = qMax(qAbs(_min), qAbs(_max));
    if ( norma != 0.0 )
      normal_data = data / norma;
  } else if (_max == _min) {
    normal_data = ( _max==0.0 ) ? 0.0 : 1.0 ;
  } else {
    normal_data = (data-_min) / (_max-_min);
  }
  curve->setRawSamples(xData->data(),
                       normalized ? normal_data.data() : data.data(),
                       curve->dataSize());
}

