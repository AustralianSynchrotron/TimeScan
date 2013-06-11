#include "TimeScanMX_mainwindow.h"
#include "ui_TimeScanMX_mainwindow.h"
#include <QDir>

//QSettings * QChartMX::globalSettings = new QSettings("/etc/timeScanMX", QSettings::IniFormat);


struct clargs {

  std::string command;
  bool start;
  double interval;
  double period;
  bool cont;
  std::string saveDir;
  std::string saveName;
  bool autoName;
  std::vector<std::string> detectors;
  double min;
  bool autoMin;
  double max;
  bool autoMax;
  bool log;
  bool norma;
  bool grid;
  bool collapseControl;

  poptmx::OptionTable table;

  clargs(int argc, char *argv[]);
};


clargs::clargs(int argc, char *argv[]) :
  start(false),
  interval(0.1),
  period(1.0),
  cont(false),
  saveDir(),
  saveName(),
  autoName(false),
  min(0),
  autoMin(false),
  max(0),
  autoMax(false),
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
      .add(poptmx::OPTION,   &autoName, 'N', "autoname",
           "Automatic name for the data file.","")
      .add(poptmx::OPTION,   &min,'m', "min",
           "Minimum on the graph's Y axis. Automatically calculated if not used.", "")
      .add(poptmx::OPTION,   &autoMin, 0, "automin",
           "Automatic minimum.","")
      .add(poptmx::OPTION,   &max,'M', "max",
           "Maximum on the graph's Y axis. Automatically calculated if not used.", "")
      .add(poptmx::OPTION,   &autoMax, 0, "automax",
           "Automatic maximum.","")
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

  if (table.count(&autoName) && table.count(&saveName))
    poptmx::throw_error("Arguments", "Options " + table.desc(&autoName) + " and " +
                        table.desc(&saveName) + " are mutually exclusive.");
  if (table.count(&autoMin) && table.count(&min))
    poptmx::throw_error("Arguments", "Options " + table.desc(&autoMin) + " and " +
                        table.desc(&min) + " are mutually exclusive.");
  if (table.count(&autoMax) && table.count(&max))
    poptmx::throw_error("Arguments", "Options " + table.desc(&autoMax) + " and " +
                        table.desc(&max) + " are mutually exclusive.");

  command = table.name();

}


QSettings MainWindow::localSettings(QDir::homePath() + "/.TimeScanMX", QSettings::IniFormat);


MainWindow::MainWindow(int argc, char *argv[], QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

  clargs args(argc, argv);

  ui->setupUi(this);
  chart = new QChartMX;
  setCentralWidget(chart);

  if ( args.table.count() ) {

    if ( args.table.count(&args.interval) )
      chart->setInterval(args.interval);
    if ( args.table.count(&args.period) )
      chart->setPeriod(args.period);
    if ( args.table.count(&args.cont) )
      chart->setContinious(args.cont);

    if ( args.table.count(&args.saveDir) )
      chart->setSaveDir( QString::fromStdString(args.saveDir) );
    if ( args.table.count(&args.saveName) )
      chart->setSaveName( QString::fromStdString(args.saveName) );
    chart->setAutoName(args.autoName);

    for (unsigned int i = 0; i < args.detectors.size(); ++i)
      chart->addSignal(QString::fromStdString(args.detectors[i]));

    if (args.table.count(&args.min))
      chart->setMin(args.min);
    chart->setAutoMin(args.autoMin);
    if (args.table.count(&args.max))
      chart->setMax(args.max);
    chart->setAutoMax(args.autoMax);

    chart->setLogarithmic(args.log);
    chart->setNormalized(args.norma);
    chart->setGridVisible(args.grid);
    chart->setControlCollapsed(args.collapseControl);

  } else {

    if ( localSettings.contains("interval") )
      chart->setInterval(localSettings.value("interval").toDouble());
    if ( localSettings.contains("period") )
      chart->setPeriod(localSettings.value("period").toDouble());
    if ( localSettings.contains("continue") )
      chart->setContinious(localSettings.value("continue").toBool());

    if ( localSettings.contains("saveDir") )
      chart->setSaveDir(localSettings.value("saveDir").toString());
    if ( localSettings.contains("saveName") )
      chart->setSaveName(localSettings.value("saveName").toString());
    if ( localSettings.contains("autoName") )
      chart->setAutoName(localSettings.value("autoName").toBool());

    int size = localSettings.beginReadArray("detectors");
    for (int i = 0; i < size; ++i) {
      localSettings.setArrayIndex(i);
      chart->addSignal(localSettings.value("detector").toString());
    }
    localSettings.endArray();


    if ( localSettings.contains("min") )
      chart->setMin(localSettings.value("min").toDouble());
    if ( localSettings.contains("autoMin") )
      chart->setAutoMin(localSettings.value("autoMin").toBool());
    if ( localSettings.contains("max") )
      chart->setMax(localSettings.value("max").toDouble());
    if ( localSettings.contains("autoMax") )
      chart->setAutoMax(localSettings.value("autoMax").toBool());

    if ( localSettings.contains("log") )
      chart->setLogarithmic( localSettings.value("log").toBool() );
    if ( localSettings.contains("norma") )
      chart->setNormalized(localSettings.value("norma").toBool() );
    if ( localSettings.contains("grid") )
      chart->setGridVisible( localSettings.value("grid").toBool() );

  }

  connect(chart, SIGNAL(configurationChanged()), SLOT(updateConfiguration()));

  if (args.start)
    QTimer::singleShot(500, chart, SLOT(start()));


}

MainWindow::~MainWindow()
{
  delete chart;
  delete ui;
}

void MainWindow::updateConfiguration() {

  localSettings.clear();

  localSettings.setValue("interval", chart->interval());
  localSettings.setValue("period", chart->period());
  localSettings.setValue("continue", chart->isContinious());
  localSettings.setValue("saveDir", chart->saveDir());
  localSettings.setValue("saveName", chart->saveName());
  localSettings.setValue("autoName", chart->isAutoName());

  QStringList detectors = chart->allSignals();
  localSettings.beginWriteArray("detectors");
  for (int i = 0; i < detectors.size(); ++i) {
    localSettings.setArrayIndex(i);
    localSettings.setValue("detector", detectors[i]);
  }
  localSettings.endArray();

  localSettings.setValue("autoMin", chart->isAutoMin());
  localSettings.setValue("min", chart->min());
  localSettings.setValue("autoMax", chart->isAutoMax());
  localSettings.setValue("max", chart->max());
  localSettings.setValue("log", chart->isLogarithmic());
  localSettings.setValue("norma", chart->isNormalized());
  localSettings.setValue("grid", chart->isGridVisible());

}

