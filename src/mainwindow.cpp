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

#include <QDebug>

MainWindow::MainWindow(TICK startingTick, TICK finalTick, QWidget *parent) : QMainWindow(parent),
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
    delete lastPath;
  }
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
  //  connect(ct, SIGNAL(refreshButtonClicked()), this, SLOT(on_actionRefresh_Folder_triggered()));
  connect(ct, SIGNAL(zoomInClicked()), this, SLOT(on_actionZoomInTriggered()));
  connect(ct, SIGNAL(zoomOutClicked()), this, SLOT(on_actionZoomOutTriggered()));
  //  connect(ct, SIGNAL(zoomFitClicked()), this, SLOT(on_actionZoomFitTriggered()));
  connect(ct, SIGNAL(changeViewTasksClicked()), this, SLOT(on_actionViewChangedTasksTriggered()));
  connect(ct, SIGNAL(changeViewCPUClicked()), this, SLOT(on_actionViewChangedCPUTriggered()));
  connect(ct, SIGNAL(changeViewGanntClicked()), this, SLOT(on_actionViewChangedGanntTriggered()));
}

MainWindow::~MainWindow()
{
  qDebug() << __func__;
  //  delete _ep;
  delete ui;
}

void MainWindow::updateTitle()
{
  QString t = "PlotSched";
  if (filename.length() > 0)
  {
    t.append(" - ");
    t.append(filename);
  }
  this->setWindowTitle(t);
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
  updateTitle();
}

void MainWindow::on_actionQuit_triggered()
{
  close();
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
  updateTitle();

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

    EVENTSMANAGER.setCurrentFolder(QFileInfo(path).dir());

    EVENTSMANAGER.clear();
    EVENTSMANAGER.readCPUs();
    EVENTSMANAGER.readTasks();
    EVENTSPARSER.parseFile(path);
//    for (const auto& cpu : EVENTSMANAGER.getAllCPUsEvents().keys()) {
//        qDebug() << cpu->str();
//        for (const auto& e : EVENTSMANAGER.getAllCPUsEvents()[cpu])
//            qDebug() << "\t" << e->str();
//    }
    EVENTSPARSER.parseFrequencies();
    EVENTSMANAGER.addFrequencyChangeEvents();

    updatePlot();
  }
}

void MainWindow::on_actionViewChangedTriggered(VIEWS newView)
{
  this->_currentView = newView;
  updatePlot();
}

void MainWindow::updatePlot(qreal center)
{
  unsigned long row = 0;

  qDebug() << "View updated: " + VIEWS_STR[_currentView];
  EVENTSPARSER.print();
  plot->clear();

  if (_plotFrames[_currentView] != NULL)
  {
    if (_currentView == VIEWS::GANNT)
    { // default
      // CPU #0 |_____t1______t2_____...
      PlotFrame *plotFrame = new PlotFrame;
      _plotFrames[_currentView] = plotFrame;

      QMap<CPU *, QList<Event *>> m = EVENTSMANAGER.getAllCPUsEvents();
      QVector<QPair<CPU *, QList<Event *>>> msorted = QVector<QPair<CPU *, QList<Event *>>>(EVENTSMANAGER.getCPUs().size());
      for (const auto &elem : m.toStdMap())
      {
        QPair<CPU *, QList<Event *>> pair = QPair<CPU *, QList<Event *>>(elem.first, elem.second);
        msorted[elem.first->getID()] = pair;
      }
      for (int i = 0; i < msorted.size(); i++) {
          if (msorted[i].first == NULL) {
              msorted.removeAt(i);
              i--;
          }
      }
      Q_ASSERT(msorted.size() == m.keys().size());

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
        CPU_BL* cc = dynamic_cast<CPU_BL*>(elem.first);
        if (cc != NULL) {
            QString prefix = (cc->isBig() ? "B" : "L");
            plotFrame->addRow(prefix + elem.first->getName());
        }
        else
            plotFrame->addRow(elem.first->getName());

        QList<Event *> l = elem.second;
        for (Event *e : l)
        {
          e->setRow(row);
          //                  if (e->getKind() != FREQUENCY_CHANGE)
          qDebug() << "dealing with " << e->str();
          EventView *ev = new EventView(e);
          //                  if (e->getKind() != EVENT_KIND::ACTIVATION && e->getKind() != EVENT_KIND::DEAD)
          if (e->getKind() == EVENT_KIND::RUNNING)
            ev->setFgTextType(EventView::FG_FIELD::TASKANME);
          plot->addNewItem(ev);
        }
        ++row;
      }
    }
    else if (_currentView == VIEWS::TASKS)
    {
      PlotFrame *plotFrame = new PlotFrame;
      _plotFrames[_currentView] = plotFrame;
      QMap<Task *, QList<Event *>> m = EVENTSMANAGER.getAllTasksEvents();
      for (QList<Event *> l : m.values())
      {
        plotFrame->addRow(l.first()->getTask()->name);

        for (Event *e : l)
        {
          e->setRow(row);
          //                  qDebug() << "dealing with " << e->print();
          EventView *ev = new EventView(e);
          if (e->getKind() != EVENT_KIND::ACTIVATION)
            ev->setFgTextType(EventView::FG_FIELD::CPUNAME);
          plot->addNewItem(ev);
        }
        ++row;
      }
    }
    else
    { // CORES
    }
  } // if _plotframe[current view] == NULL

  qreal rightmost = plot->updateSceneView(center);

  _plotFrames[_currentView]->setWidth(rightmost);
  plot->addNewItem(_plotFrames[_currentView]);

  plot->updateSceneView(center);

  qDebug() << "MainWindow::updatePlot()";
}
