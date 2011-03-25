#include <QFileDialog>
#include <QFileInfo>
#include <QDir>
#include <QPrinter>
#include <QDate>
#include <QPrintDialog>
#include <QTime>

#include "qchartmx.h"

#include <qwt_scale_draw.h>
#include <qwt_scale_engine.h>


clargs::clargs(int argc, char *argv[]) :
    start(false),
    interval(0.1),
    period(1.0),
    cont(false),
    saveDir(),
    saveName(),
    log(false),
    norma(false),
    grid(false),
    collapseControl(false),
    table("Program to graphically represent time changes of an EPICS PV.")
{

  table
    .add(poptmx::NOTE,     "ARGUMENTS:")
    .add(poptmx::ARGUMENT, &detectors, "detectors list",
         "List of the PV names to be monitored.", "")

    .add(poptmx::NOTE,     "OPTIONS:")
    .add(poptmx::OPTION,   &start, 's', "start",
         "Starts monitoring automatically.",
         "Equal pushing the \"Start\" button after entering all values.")
    .add(poptmx::OPTION,   &interval, 'i', "interval",
         "Readout interval.", "Time interval between two readouts of the PV. Graph step.")
    .add(poptmx::OPTION,   &period, 'p', "period",
         "Monitored period.", "Period of time to be shown in graph. Graph width.")
    .add(poptmx::OPTION,   &cont, 'c', "continue",
         "Continuous monitoring.",
         "Tells the monitoring to keep going after the one period has expired.")
    .add(poptmx::OPTION, &saveDir,'d', "dir",
         "Directory where the data file is stored.", "")
    .add(poptmx::OPTION,   &saveName, 'f', "file",
         "Name of the data file", "")
    .add(poptmx::OPTION,   &min,'m', "min",
         "Minimum on the graph's Y axis. Automatically calculated if not used.", "")
    .add(poptmx::OPTION,   &max,'M', "max",
         "Maximum on the graph's Y axis. Automatically calculated if not used.", "")
    .add(poptmx::OPTION,   &log, 'l', "log",
         "Sets the logarithmic scale of the Y axis.","")
    .add(poptmx::OPTION,   &norma, 'n', "normalize",
         "Normalize the data on the graph.","")
    .add(poptmx::OPTION,   &grid, 'g', "grid",
         "Shows grid lines on the graph","")
    .add(poptmx::OPTION,   &collapseControl, 0, "collapse",
         "Hide the control panel.",
         "Hide (\"collapse\" in terms of Qt's splitter') the control pannel"
         " leaving only the graph and table pannels visible.")
    .add_standard_options();

  if ( ! table.parse(argc,argv) )
    exit(0);

  command = table.name();

}


class TimeScaleDraw: public QwtScaleDraw {
public:
  TimeScaleDraw(const QTime &base, double interv=1.0):
      baseTime(base), interval(interv) {}
  virtual QwtText label(double v) const {
    QTime utime = baseTime.addMSecs( (int) 1000*v*interval );
    return utime.toString( interval < 1.0 ?
                           "hh:mm:ss.zzz" : "hh:mm:ss" );
  }
private:
  QTime baseTime;
  double interval;
};



QSettings * QChartMX::globalSettings = new QSettings("/etc/timeScanMX", QSettings::IniFormat);
const QString QChartMX::badStyle = "background-color: rgba(255, 0, 0, 64);";
const QString QChartMX::goodStyle = QString();


QChartMX::QChartMX(const clargs & args, QWidget *parent) :
    QMainWindow(parent),
    localSettings(0),
    ui(new Ui::TimeScanMX),
    timer(new QTimer(this)),
    timeData()
{

  colorsLeft.push_back(Qt::red);
  colorsLeft.push_back(Qt::blue);
  colorsLeft.push_back(Qt::green);
  colorsLeft.push_back(Qt::cyan);
  colorsLeft.push_back(Qt::magenta);
  colorsLeft.push_back(Qt::yellow);
  colorsLeft.push_back(Qt::darkRed);
  colorsLeft.push_back(Qt::darkBlue);
  colorsLeft.push_back(Qt::darkGreen);
  colorsLeft.push_back(Qt::darkCyan);
  colorsLeft.push_back(Qt::darkMagenta);
  colorsLeft.push_back(Qt::darkYellow);
  colorsLeft.removeAll(QApplication::palette().color(QPalette::Text));

  ui->setupUi(this);
  ui->splitter->setCollapsible(0, true);

  Signal::xData = &timeData;

  ui->plot->setAutoReplot(false);
  ui->plot->setAxisMaxMinor(QwtPlot::yLeft,  10);
  grid = new QwtPlotGrid;
  grid->enableXMin(true);
  grid->enableYMin(true);
  grid->setMajPen(Qt::DashLine);
  grid->setMinPen(Qt::DotLine);
  showGrid(ui->showGrid->isChecked());

  connect(ui->addSignal, SIGNAL(clicked()), SLOT(addSignal()));
  connect(ui->startStop, SIGNAL(clicked()), SLOT(startStop()));
  connect(ui->browseSaveDir, SIGNAL(clicked()), SLOT(browseAutoSave()));
  connect(ui->printResult, SIGNAL(clicked()), SLOT(printResult()));
  connect(ui->saveResult, SIGNAL(clicked()), SLOT(saveResult()));
  connect(ui->qtiResults, SIGNAL(clicked()), SLOT(openQti()));
  connect(ui->saveDir, SIGNAL(textChanged(QString)), SLOT(prepareAutoSave()));
  connect(ui->saveName, SIGNAL(textChanged(QString)), SLOT(prepareAutoSave()));
  connect(ui->autoName, SIGNAL(toggled(bool)), SLOT(prepareAutoSave()));

  connect(ui->showGrid, SIGNAL(toggled(bool)), SLOT(showGrid(bool)));
  connect(ui->norma, SIGNAL(toggled(bool)), SLOT(normalizePlots(bool)));
  connect(ui->logY, SIGNAL(toggled(bool)), SLOT(logScale()));
  connect(ui->autoMin, SIGNAL(clicked()), SLOT(setRanges()));
  connect(ui->autoMax, SIGNAL(clicked()), SLOT(setRanges()));
  connect(ui->min, SIGNAL(editingFinished()), SLOT(setRanges()));
  connect(ui->max, SIGNAL(editingFinished()), SLOT(setRanges()));

  // Check for qtiplot
  QProcess checkQti;
  checkQti.start("bash -c \"command -v qtiplot\"");
  checkQti.waitForFinished();
  qtiCommand = checkQti.readAll();
  qtiCommand.chop(1);
  ui->qtiResults->setVisible( ! qtiCommand.isEmpty() );


  // Restore global settings
  int size = globalSettings->beginReadArray("detectors");
  for (int i = 0; i < size; ++i) {
    globalSettings->setArrayIndex(i);
    Signal::knownDetectors.append( globalSettings->value("detector").toString() );
  }
  globalSettings->endArray();


  if ( args.table.count() ) {

    if ( args.table.count(&args.interval) )
      ui->interval->setValue(args.interval);
    if ( args.table.count(&args.period) )
      ui->period->setValue(args.period);
    if ( args.table.count(&args.cont) )
      ui->cont->setChecked(args.cont);


    if ( args.table.count(&args.saveDir) )
      ui->saveDir->setText( args.table.count(&args.saveDir) ?
                            QString::fromStdString(args.saveDir)  :  QDir::homePath() );
    ui->autoName->setChecked( ! args.table.count(&args.saveName) );
    if ( args.table.count(&args.saveName) )
      ui->saveName->setText(QString::fromStdString(args.saveName));

    for (unsigned int i = 0; i < args.detectors.size(); ++i)
      addSignal(QString::fromStdString(args.detectors[i]));

    ui->autoMin->setChecked( ! args.table.count(&args.min) );
    if ( args.table.count(&args.min) )
      ui->min->setValue(args.min);
    ui->autoMax->setChecked( ! args.table.count(&args.max) );
    if ( args.table.count(&args.max) )
      ui->max->setValue(args.max);

    ui->logY->setChecked(args.log);
    ui->norma->setChecked(args.norma);
    ui->showGrid->setChecked(args.grid);

    if (args.collapseControl) {
      QList<int> sizes;
      sizes.push_back(0);
      sizes.push_back(1);
      ui->splitter->setSizes(sizes);
    }

  } else {

    // Restore local settings

    localSettings = new QSettings(QDir::homePath() + "/.TimeScanMX",
                                  QSettings::IniFormat);

    if ( localSettings->contains("interval") )
      ui->interval->setValue(localSettings->value("interval").toDouble());
    if ( localSettings->contains("period") )
      ui->period->setValue(localSettings->value("period").toDouble());
    if ( localSettings->contains("continue") )
      ui->cont->setChecked(localSettings->value("continue").toBool());


    if ( localSettings->contains("saveDir") )
      ui->saveDir->setText(localSettings->value("saveDir").toString());
    else
      ui->saveDir->setText(QDir::homePath());
    if ( localSettings->contains("saveName") )
      ui->saveName->setText(localSettings->value("saveName").toString());
    if ( localSettings->contains("autoName") )
      ui->autoName->setChecked( localSettings->value("autoName").toBool() );

    size = localSettings->beginReadArray("detectors");
    for (int i = 0; i < size; ++i) {
      localSettings->setArrayIndex(i);
      addSignal(localSettings->value("detector").toString());
    }
    localSettings->endArray();

    if ( localSettings->contains("autoMin") )
      ui->autoMin->setChecked(localSettings->value("autoMin").toBool());
    if ( localSettings->contains("min") )
      ui->min->setValue(localSettings->value("min").toDouble());
    if ( localSettings->contains("autoMax") )
      ui->autoMax->setChecked(localSettings->value("autoMax").toBool());
    if ( localSettings->contains("max") )
      ui->max->setValue(localSettings->value("max").toDouble());

    if ( localSettings->contains("log") )
      ui->logY->setChecked( localSettings->value("log").toBool() );
    if ( localSettings->contains("norma") )
      ui->norma->setChecked( localSettings->value("norma").toBool() );
    if ( localSettings->contains("grid") )
      ui->showGrid->setChecked( localSettings->value("grid").toBool() );

  }

  connect(ui->interval, SIGNAL(editingFinished()), SLOT(storeSettings()));
  connect(ui->period, SIGNAL(editingFinished()), SLOT(storeSettings()));
  connect(ui->cont, SIGNAL(toggled(bool)), SLOT(storeSettings()));
  connect(ui->saveDir, SIGNAL(editingFinished()), SLOT(storeSettings()));
  connect(ui->saveName, SIGNAL(editingFinished()), SLOT(storeSettings()));
  connect(ui->autoName, SIGNAL(toggled(bool)), SLOT(storeSettings()));
  connect(ui->autoMin, SIGNAL(toggled(bool)), SLOT(storeSettings()));
  connect(ui->min, SIGNAL(editingFinished()), SLOT(storeSettings()));
  connect(ui->autoMax, SIGNAL(toggled(bool)), SLOT(storeSettings()));
  connect(ui->max, SIGNAL(editingFinished()), SLOT(storeSettings()));
  connect(ui->logY, SIGNAL(toggled(bool)), SLOT(storeSettings()));
  connect(ui->norma, SIGNAL(toggled(bool)), SLOT(storeSettings()));
  connect(ui->showGrid, SIGNAL(toggled(bool)), SLOT(storeSettings()));

  connect(ui->interval, SIGNAL(editingFinished()), SLOT(updateTimes()));
  connect(ui->period, SIGNAL(editingFinished()), SLOT(updateTimes()));

  connect(timer, SIGNAL(timeout()), SLOT(getData()));

  preparePlot();

  if (args.start)
    QTimer::singleShot(0.5, this, SLOT(startStop()));

}

QChartMX::~QChartMX() {
  delete ui;
}

void QChartMX::storeSettings() {

  if (!localSettings)
    return;

  localSettings->setValue("interval", ui->interval->value());
  localSettings->setValue("period", ui->period->value());
  localSettings->setValue("continue", ui->cont->isChecked());
  localSettings->setValue("saveDir", ui->saveDir->text());
  localSettings->setValue("saveName", ui->saveName->text());
  localSettings->setValue("autoName", ui->autoName->isChecked());

  localSettings->beginWriteArray("detectors");
  for (int i = 0; i < signalsE.size(); ++i) {
    localSettings->setArrayIndex(i);
    localSettings->setValue("detector", signalsE[i]->sig->currentText());
  }
  localSettings->endArray();

  localSettings->setValue("autoMin", ui->autoMin->isChecked());
  localSettings->setValue("min", ui->min->value());
  localSettings->setValue("autoMax", ui->autoMax->isChecked());
  localSettings->setValue("max", ui->max->value());
  localSettings->setValue("log", ui->logY->isChecked());
  localSettings->setValue("norma", ui->norma->isChecked());
  localSettings->setValue("grid", ui->showGrid->isChecked());

}


void QChartMX::browseAutoSave(){
  QString new_dir = QFileDialog::getExistingDirectory(this, "Open directory", ui->saveDir->text());
  if ( ! new_dir.isEmpty() )
    ui->saveDir->setText(new_dir);
}

QString QChartMX::prepareAutoSave() {

  if( ! ui->autoName->isChecked() && ui->saveName->text().isEmpty() )
    return "/dev/null";

  QString dn = ui->saveDir->text();
  if ( ! dn.endsWith('/') )
    dn += "/";
  QFileInfo dirInfo(dn);
  bool dirOK = dirInfo.exists() && dirInfo.isWritable();
  ui->saveDir->setStyleSheet( dirOK  ?  goodStyle  :  badStyle );

  QString tryName;

  if ( ui->autoName->isChecked() ) {

    const QString fn = "time_scan_" +
                       QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss");
    const QString ext = ".dat";

    int count = 1;
    tryName = fn + ext;
    while ( QFile::exists( dn + tryName ) )
      tryName = fn + "_(" + QString::number(count++) + ")" + ext;
    ui->saveName->setText(tryName);

  }

  tryName = ui->saveName->text();
  QFileInfo fileInfo( dn + ui->saveName->text() );
  bool fileOK = ! fileInfo.exists() ||
                ( ! fileInfo.isDir() && fileInfo.isWritable() );
  ui->saveName->setStyleSheet( fileOK  ?  goodStyle  :  badStyle );

  return  ( dirOK && fileOK )  ?  dn + tryName  :  "" ;

}

void QChartMX::saveResult() {
  if (tableWasSavedTo.isEmpty())
    return;
  QString dataName =
      QFileDialog::getSaveFileName(this, "Save data", ui->saveDir->text() ) ;
  if (QFile::exists(dataName))
    QFile::remove(dataName);
  QFile::copy(tableWasSavedTo, dataName);
}

void QChartMX::printResult(){
  QPrinter printer;
  QPrintDialog dialog(&printer);
  if ( dialog.exec() )
    ui->plot->print(printer);
}



void QChartMX::addSignal(const QString & pvName){

  Signal * sg = new Signal(this);
  sg->sig->addItem(pvName);
  sg->sig->setCurrentIndex( sg->sig->findText(pvName) );

  sg->pen.setColor( colorsLeft.size()
                     ?  colorsLeft.takeFirst() :
                     QApplication::palette().color(QPalette::Text));
  sg->pen.setWidth(2);
  sg->curve->setPen(sg->pen);

  connect(sg, SIGNAL(remove()), this, SLOT(removeSignal()));
  //connect(sg, SIGNAL(connectionChanged(bool)), SLOT(checkReady()));

  sg->tableItem = new QTableWidgetItem(pvName);
  signalsE.append(sg);
  int tablePos = ui->dataTable->columnCount();
  ui->dataTable->insertColumn(tablePos);
  sg->tableItem->setTextColor(sg->pen.color());
  ui->dataTable->setHorizontalHeaderItem(tablePos, sg->tableItem);

  sg->sig->setStyleSheet("color: " + sg->pen.color().name() + ";");

  constructSignalsLayout();

  preparePlot();
  sg->curve->attach(ui->plot);

}


void QChartMX::removeSignal(){
  Signal * sg = (Signal *) sender();
  if (!sg)
    return;
  if (sg->pen.color() != QApplication::palette().color(QPalette::Text))
    colorsLeft.push_back(sg->pen.color());
  sg->curve->detach();
  ui->dataTable->removeColumn(column(sg));
  signalsE.removeOne(sg);
  sg->deleteLater();
  sg->rem->deleteLater();
  sg->sig->deleteLater();
  sg->val->deleteLater();
  constructSignalsLayout();
}

void QChartMX::constructSignalsLayout(){
  foreach(Signal * sg, signalsE){
    int position=signalsE.indexOf(sg) + 1;
    ui->signalsL->addWidget(sg->rem,   position, 0);
    ui->signalsL->addWidget(sg->sig,    position, 1);
    ui->signalsL->addWidget(sg->val,   position, 2);
  }
  ui->addSignal->setStyleSheet( signalsE.size() ? goodStyle : badStyle );
  storeSettings();
}

int QChartMX::column(Signal * sig) const {
  for (int icur = 0 ; icur < ui->dataTable->columnCount() ; icur++ )
    if ( ui->dataTable->horizontalHeaderItem(icur)  == sig->tableItem )
      return icur;
  return -1;
}



void QChartMX::updateTimes() {
  int points = ui->period->value() / ui->interval->value();
  if (points < 2) {
    if (sender() == ui->interval)
      ui->period->setValue(2*ui->interval->value());
    else
      ui->interval->setValue(ui->period->value()/2.0);
  }
  preparePlot();
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
        << sig->pv->pv() << " ";
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

void QChartMX::showGrid(bool show){
  if (show)
    grid->attach(ui->plot);
  else
    grid->detach();
  ui->plot->replot();
}


void QChartMX::normalizePlots(bool norm){
  foreach (Signal * sig, signalsE)
    sig->preparePlot(norm, isLog());
  ui->plot->enableAxis( QwtPlot::yLeft, ! norm );
}


void QChartMX::logScale() {
  foreach (Signal * sig, signalsE)
    sig->preparePlot(isNorm(), isLog());
  if ( isLog() )
    ui->plot->setAxisScaleEngine(QwtPlot::yLeft, new QwtLog10ScaleEngine);
  else
    ui->plot->setAxisScaleEngine(QwtPlot::yLeft, new QwtLinearScaleEngine);
}

void QChartMX::setRanges() {

  double m=min(), M=max();

  if (isLog()) {
    if (M<=0.0 || isNorm()) {
      m = ( m<=0.0 || M<=0.0 ) ? 1.0e-10 : m/M ;
      M = 1.0;
    }
  } else {
    if (isNorm()) {
      m = 0;
      M = 1.0;
    }
  }
  if ( m == M ) {
    m = (m==0.0 ? -0.1 : m*0.9);
    M = (M==0.0 ?  0.1 : M*1.1);
  }

  if ( ! ui->autoMin->isChecked() )
    m = ui->min->value();
  else
    ui->min->setValue(m);
  if ( ! ui->autoMax->isChecked() )
    M = ui->max->value();
  else
    ui->max->setValue(M);

  if ( m != ui->plot->axisScaleDiv(QwtPlot::yLeft)->lowerBound() ||
       M != ui->plot->axisScaleDiv(QwtPlot::yLeft)->upperBound() )
    ui->plot->setAxisScale(QwtPlot::yLeft, m, M);

}


double QChartMX::min() {
  double m = NAN;
  foreach (Signal * sig, signalsE)
    if ( isnan(m) || sig->min < m)
      m = sig->min;
  return m;
}

double QChartMX::max() {
  double M = NAN;
  foreach (Signal * sig, signalsE)
    if ( isnan(M) || sig->max > M)
      M = sig->max;
  return M;
}



void QChartMX::startStop(){

  if ( timer->isActive() ) {

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
    tableWasSavedTo = prepareAutoSave();
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
        << "# Number of scan points: " << ( ui->cont->isChecked() ?
                                            "continious scan" :
                                            QString::number(timeData.size()) )
        << "# Interval (sec): " << ui->interval->value();

    dataStr
        << "# Signals:\n"
        << "#\n";
    foreach (Signal * sig, signalsE)
      dataStr
          << "# PV: \"" << sig->pv->pv() << "\"\n";
    dataStr << "#\n";

    dataStr
        << "# Data columns:\n"
        << "# "
        << "%Point "
        << "%Time ";
    foreach (Signal * sig, signalsE)
      dataStr
          << "%" << sig->pv->pv() << " ";
    dataStr << "\n";

    getData();
    timer->start((int) 1000*ui->interval->value());

  }

}


void QChartMX::preparePlot() {

  int points = ui->period->value() / ui->interval->value();

  ui->plot->setAxisScaleDraw(QwtPlot::xBottom,
                             new TimeScaleDraw(QTime::currentTime(),
                             ui->interval->value()));
  ui->plot->setAxisScale(QwtPlot::xBottom, 0, points);
  ui->plot->setAxisLabelRotation(QwtPlot::xBottom, -50.0);
  ui->plot->setAxisLabelAlignment(QwtPlot::xBottom, Qt::AlignLeft | Qt::AlignBottom);

  timeData.resize(points);
  for ( int i = 0; i < points; i++ )
      timeData(points - 1 - i) = i;

  foreach(Signal * sig, signalsE)
    sig->resetData(isNorm(), isLog());

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
  if ( ui->cont->isChecked() && point > points-1 ) {
    ui->dataTable->removeRow(0);
    table_row=points-1;
  }

  ui->dataTable->insertRow(table_row);
  ui->dataTable->setVerticalHeaderItem
      (point, new QTableWidgetItem(QString::number(point+1)));
  if ( ! ui->dataTable->underMouse() )
  ui->dataTable->scrollToBottom();
  ui->dataTable->setItem(table_row, 0,
                         new QTableWidgetItem(dt.time().toString()));

  dataStr << point+1 << " " << dt.toString() << " ";

  timeData += 1;
  ui->plot->setAxisScale(QwtPlot::xBottom,
                         timeData(points-1), timeData(0));

  foreach(Signal * sig, signalsE) {
    double value = sig->get();
    QTableWidgetItem * item = new QTableWidgetItem(QString::number(value));
    item->setTextColor(sig->pen.color());
    ui->dataTable->setItem(table_row, column(sig), item);
    dataStr << QString::number(value, 'e') << " ";
  }
  dataStr <<  "\n";

  ui->plot->replot();
  setRanges();

  if ( ! ui->cont->isChecked() && point >= points-1 )
    startStop();
  point++;

  gettingData = false;

}










QStringList QChartMX::Signal::knownDetectors = QStringList();
Line * QChartMX::Signal::xData = 0;


QChartMX::Signal::Signal(QWidget* parent) :
    QObject(parent),
    rem(new QPushButton("-", parent)),
    sig(new QComboBox(parent)),
    val(new QLabel(parent)),
    tableItem(new QTableWidgetItem()),
    pv(new QEpicsPV(this)),
    curve(new QwtPlotCurve),
    pen(),
    min(NAN), max(NAN),
    normalized(false),
    logscaled(false)
{

  sig->setEditable(true);
  sig->setDuplicatesEnabled(false);
  sig->addItems(knownDetectors);
  sig->clearEditText();
  sig->setToolTip("PV of the signal.");

  rem->setToolTip("Remove the signal.");
  val->setToolTip("Current value.");

  curve->setStyle(QwtPlotCurve::Dots);
  curve->setPaintAttribute(QwtPlotCurve::PaintFiltered);
  //curve->setPaintAttribute(QwtPlotCurve::ClipPolygons);

  connect(sig, SIGNAL(editTextChanged(QString)), pv, SLOT(setPV(QString)));
  connect(sig, SIGNAL(editTextChanged(QString)), SLOT(setHeader(QString)));
  connect(pv, SIGNAL(valueUpdated(QVariant)), SLOT(updateValue(QVariant)));
  connect(pv, SIGNAL(connectionChanged(bool)), SLOT(setConnected(bool)));
  connect(rem, SIGNAL(clicked()), SIGNAL(remove()));

}


QChartMX::Signal::~Signal(){
}


double QChartMX::Signal::get() {

  double value = pv->get().toDouble();
  const int points = xData->size();

  double deleted_value = data(points-1);
  for ( int i = points-1 ; i > 0 ; i--)
    data(i) = data(i-1);
  data(0) = value;

  double oldMin = min , oldMax = max;

  if ( ! isnan(deleted_value) && deleted_value <= min )
    min = blitz::min(data);
  if ( ! isnan(deleted_value) && deleted_value >= max )
    max = blitz::max(data);
  if ( isnan(min) || value < min )
    min = value;
  if ( isnan(max) || value > max )
    max = value;

  if (normalized) {
    if (min != oldMin || max!= oldMax) {
      prep();
    } else {
      for ( int i = points-1 ; i > 0 ; i--)
        normal_data(i) = normal_data(i-1);
      normal_data(0) = prep(value);
    }
  }

  return value;

}


void QChartMX::Signal::preparePlot(bool norma, bool log) {
  normalized = norma;
  logscaled = log;
  min = blitz::min(data);
  max = blitz::max(data);
  prep();
  curve->setRawData(xData->data(),
                    norma ? normal_data.data() : data.data(),
                    xData->size());
}


void QChartMX::Signal::resetData(bool norm, bool log) {
  int points = xData->size();
  min = NAN;
  max = NAN;
  data.resize(points);
  data = NAN;
  normal_data.resize(points);
  normal_data = NAN;
  preparePlot(norm, log);
}


double QChartMX::Signal::prep(double value) {
  if (logscaled) {
    double norma = qMax(qAbs(min), qAbs(max));
    if ( norma == 0.0 )
      return value;
    else
      return value / norma;
  } else if (max == min) {
    return ( max==0.0 ) ? 0.0 : 1.0 ;
  } else {
    return (value-min) / (max-min);
  }
}


void QChartMX::Signal::prep() {
  if (logscaled) {
    double norma = qMax(qAbs(min), qAbs(max));
    if ( norma != 0.0 )
      normal_data = data / norma;
  } else if (max == min) {
    normal_data = ( max==0.0 ) ? 0.0 : 1.0 ;
  } else {
    normal_data = (data-min) / (max-min);
  }
}

