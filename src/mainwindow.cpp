#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "customtoolbar.h"
#include "eventview.h"
#include "eventsmanager.h"
#include "eventsparser.h"
#include "settingsdialog.h"

#include <QToolBar>
#include <QToolButton>
#include <QIcon>
#include <QFileDialog>
#include <QShortcut>
#include <QProcess>
#include <QDebug>

MainWindow::MainWindow(TICK startingTick, TICK finalTick, QWidget *parent)
    :   QMainWindow(parent),
        ui(new Ui::MainWindow)
{
  qDebug() << "MainWindow()";
  ui->setupUi(this);

  setupShortcut();

  plot = new Plot(this);
  this->setCentralWidget(plot);


  // the menu on the left with filenames
  populate_dock();
  // the menu above with icons
  populate_toolbar();

  EVENTSMANAGER.setMainWindow(this);
  EVENTSPARSER.setStartingTick(startingTick);
  EVENTSPARSER.setFinalTick(finalTick);

  this->_currentView = VIEWS::GANNT;
  loadSettings();

  showMaximized();
}

void MainWindow::loadSettings()
{
  QFile *lastPath = SettingsManager::getFile(SettingsManager::Key::LAST_PST_PATH);
  if (lastPath->exists())
  {
    QFileInfo info(*lastPath);
    onNewTraceChosen(info.absoluteFilePath());
    tfl->update(info.absoluteDir().absolutePath());
  }
  delete lastPath;
}

void MainWindow::reloadTrace()
{
  // todo delete the plotframe if already exists
  this->onNewTraceChosen(curTrace);
}

void MainWindow::setupShortcut()
{
  QShortcut *shortcut = new QShortcut(QKeySequence(Qt::Key_F5), this);
  QObject::connect(shortcut, SIGNAL(activated()), this, SLOT(reloadTrace()));
}

void MainWindow::zoomChanged(qreal start, qreal end, qreal windowWidth)
{
  qreal center = EVENTSMANAGER.magnify(start, end, windowWidth);
  updatePlot(center);
}

// the menu on the left with filenames
void MainWindow::populate_dock()
{
  tfl = new TraceFileLister(this);
  this->addDockWidget(Qt::LeftDockWidgetArea, tfl, Qt::Vertical);
}

// the menu above with icons
void MainWindow::populate_toolbar()
{
  CustomToolBar *ct = new CustomToolBar(this);

  this->addToolBar(ct);

  connect(ct, SIGNAL(openButtonClicked()), this, SLOT(on_actionOpen_Folder_triggered()));
    connect(ct, SIGNAL(refreshButtonClicked()), this, SLOT(on_actionRefresh_triggered()));
  connect(ct, SIGNAL(zoomInClicked()), this, SLOT(on_actionZoomInTriggered()));
  connect(ct, SIGNAL(zoomOutClicked()), this, SLOT(on_actionZoomOutTriggered()));
  //  connect(ct, SIGNAL(zoomFitClicked()), this, SLOT(on_actionZoomFitTriggered()));
//  connect(ct, SIGNAL(changeViewTasksClicked()), this, SLOT(on_actionViewChangedTriggered(VIEWS::TASKS)));
//  connect(ct, SIGNAL(changeViewCPUClicked()), this, SLOT(on_actionViewChangedTriggered(VIEWS::CORES)));
//  connect(ct, SIGNAL(changeViewGanntClicked()), this, SLOT(on_actionViewChangedTriggered(VIEWS::GANNT)));
  connect(plot, SIGNAL(zoomChanged(qreal, qreal, qreal)), this, SLOT(zoomChanged(qreal,qreal,qreal)));
}

MainWindow::~MainWindow()
{
  qDebug() << __func__;
  //  delete _ep;
  delete ui;
}

void MainWindow::on_actionRefresh_triggered()
{
    onNewTraceChosen(SettingsManager::getString(SettingsManager::Key::LAST_PST_PATH));
}

void MainWindow::on_actionZoomInTriggered()
{
  for (const auto &e : EVENTSMANAGER.getAllTasksEvents().values().at(0))
  {
    e->setMagnification(e->getMagnification() + 0.5);
  }
}

void MainWindow::on_actionZoomOutTriggered()
{
  for (const auto &e : EVENTSMANAGER.getAllTasksEvents().values().at(0))
  {
    e->setMagnification(e->getMagnification() - 0.5);
  }
}

void MainWindow::on_actionOpen_triggered()
{
  QString tmpfilename = QFileDialog::getOpenFileName(
      this,
      tr("Open File"),
      "./",
      "Plot Sched Trace (*.pst)");

  filename = tmpfilename;
}

void MainWindow::on_actionOpen_Settings_triggered()
{
  SettingsDialog *w = new SettingsDialog(this);
  w->show();
}

void MainWindow::on_actionOpen_Folder_triggered()
{
  QString tmpfilename = QFileDialog::getExistingDirectory(
      this,
      tr("Open Directory"),
      "./",
      QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

  filename = tmpfilename;

  tfl->update(filename);
}

/// user chooses a .pst file
void MainWindow::onNewTraceChosen(QString path)
{
  qDebug() << "Chosen new trace : " << path;

  QFileInfo f(path);
  if (f.isFile())
  {
    SettingsManager::saveFile(SettingsManager::Key::LAST_PST_PATH, path);
    this->curTrace = path;
    setWindowTitle("PlotSched - " + path);

    EVENTSMANAGER.setCurrentFolder(QFileInfo(path).dir());

    EVENTSMANAGER.clear();
    EVENTSMANAGER.readCPUs();
    EVENTSMANAGER.readTasks();
    EVENTSPARSER.parseFile(path);

//    // debug stuff
//    for (const auto& cpu : EVENTSMANAGER.getAllCPUsEvents().keys()) {
//        qDebug() << cpu->str();
//        for (const auto& e : EVENTSMANAGER.getAllCPUsEvents()[cpu])
//            qDebug() << "\t" << e->str();
//    }

    // stuff of big-LITTLE
    EVENTSPARSER.parseFrequencies();
    EVENTSMANAGER.addFrequencyChangeEvents();

    // stuff of DAG
    EVENTSMANAGER.addDAGs();

    updatePlot();
  }
}

void MainWindow::on_actionViewChangedTriggered(VIEWS newView)
{
  this->_currentView = newView;
  updatePlot();
}

/// keyboard arrow left/right do not call this method.
/// keyboard HOME/END call this method.
void MainWindow::updatePlot(qreal center)
{
  unsigned long row = 0;
  EventView* lastEventViewShown = NULL;

  qDebug() << "View updated: " + VIEWS_STR[_currentView];
//  EVENTSPARSER.print();

  plot->clear();

  if (_plotFrames[_currentView] != NULL)
  {
    PlotFrame *plotFrame = new PlotFrame();
    _plotFrames[_currentView] = plotFrame;

    if (_currentView == VIEWS::GANNT) // default
    {
      // CPU #0 |_____t1______t2_____...
      QVector<QPair<CPU *, QList<Event *>>> msorted = EVENTSMANAGER.getAggregatedEventsByCPUs();

      qDebug() << "-----";
      qDebug() << "Showing the following to screen:";
      for (const auto &pair : msorted.toStdVector())
      {
        qDebug() << pair.first->str() << " -> {";
        for (const auto &e : pair.second)
          qDebug() << "\t" << e->str();
        qDebug() << "}";
      }
      qDebug() << "-----";

      for (const auto &elem : msorted)
      {
        // add CPUs onto screen plot
        CPU_BL* cc = dynamic_cast<CPU_BL*>(elem.first);
        if (cc != NULL) {
            QString prefix = (cc->isBig() ? "B" : "L");
            plotFrame->addRow(prefix + elem.first->getName());
        }
        else
            plotFrame->addRow(elem.first->getName());

        // add task events onto screen plot
        QList<Event *> l = elem.second;
        for (Event *e : l)
        {
            qDebug() << "dealing with " << e->str();
            e->setRow(row);

            // get event graphical representation (e.g., a rectangle or an arrow)
            EventView *ev = new EventView(e);
            if (e->getKind() == EVENT_KIND::RUNNING) {
              ev->setFgTextType(EventView::FG_FIELD::TASKANME);
              if (lastEventViewShown == NULL || e->getStart() > lastEventViewShown->getEvent()->getStart())
              lastEventViewShown = ev;
            }

            // show the event graphical representation onto screen plot
            plot->addNewItem(ev);
        }
        ++row;
      }
    }
    else if (_currentView == VIEWS::TASKS)
    {
      QMap<Task *, QList<Event *>> m = EVENTSMANAGER.getAllTasksEvents();
      for (QList<Event *> l : m.values())
      {
        plotFrame->addRow(l.first()->getTask()->name);

        // add events onto screen plot
        for (Event *e : l)
        {
          e->setRow(row);
          // qDebug() << "dealing with " << e->print();

          // get event graphical representation (e.g., a rectangle or an arrow)
          EventView *ev = new EventView(e);
          if (e->getKind() != EVENT_KIND::ACTIVATION)
            ev->setFgTextType(EventView::FG_FIELD::CPUNAME);
          plot->addNewItem(ev);

          if (e->getKind() == EVENT_KIND::RUNNING)
              if (lastEventViewShown == NULL || e->getStart() > lastEventViewShown->getEvent()->getStart())
                  lastEventViewShown = ev;
        }
        ++row;
      }
    }
    else
    { // CORES
//        |    |         | t2 |
//        | t3 |         | t1 |
//        |____|         |____|
//        L0 800MHz      B0 1700MHz
        QVector<QPair<CPU *, QList<Event *>>> msorted = EVENTSMANAGER.getAggregatedEventsByCPUs();

        qDebug() << "-----";
        qDebug() << "Showing the following to screen:";
        for (const auto &pair : msorted.toStdVector())
        {
          qDebug() << pair.first->str() << " -> {";
          for (const auto &e : pair.second)
            qDebug() << "\t" << e->str();
          qDebug() << "}";
        }
        qDebug() << "-----";

        for (const auto &elem : msorted)
        {
          // add CPUs onto screen plot
          CPU_BL* cc = dynamic_cast<CPU_BL*>(elem.first);
          if (cc != NULL) {
              QString prefix = (cc->isBig() ? "B" : "L");
              plotFrame->addCPU(prefix + elem.first->getName());
          }
          else
              plotFrame->addCPU(elem.first->getName());

          // add task events onto screen plot
          QList<Event *> l = elem.second;
          for (Event *e : l)
          {
              qDebug() << "dealing with " << e->str();
              e->setRow(row);

              // get event graphical representation (e.g., a rectangle or an arrow)
              EventView *ev = new EventView(e, 50, NULL, true);
              if (e->getKind() == EVENT_KIND::RUNNING) {
                ev->setFgTextType(EventView::FG_FIELD::TASKANME);
                if (lastEventViewShown == NULL || e->getStart() > lastEventViewShown->getEvent()->getStart())
                    lastEventViewShown = ev;
              }

              // show the event graphical representation onto screen plot
              plot->addNewItem(ev);
          }
          ++row;
        }
    }
  } // if _plotframe[current view] == NULL


//  qreal rightmost = plot->updateSceneView(center);
  if (_currentView == VIEWS::CORES)
      _plotFrames[_currentView]->setCPUHeight(300);
  else
  {
      Event* temp = lastEventViewShown->getEvent();
      qreal linesWidth = temp->getStart() * temp->getMagnification();
      qreal someMore = 500;
      _plotFrames[_currentView]->setLinesWidth(linesWidth + someMore);
  }
  plot->addNewItem(_plotFrames[_currentView]);
  plot->updateSceneView(center);
  plot->getScene()->setLastEventView(lastEventViewShown);

  qDebug() << "end of MainWindow::updatePlot()";
}
