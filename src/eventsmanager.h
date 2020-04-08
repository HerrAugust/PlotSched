#ifndef EVENTSMANAGER_H
#define EVENTSMANAGER_H

#include <QObject>
#include <QMap>
#include <QList>
#include <QString>
#include <QDebug>
#include <QDir>

#include "event.h"

#define EVENTSMANAGER EventsManager::getInstance()

class MainWindow;

/**
 * Singleton
 *
 * This class contains and remembers all tasks, CPUs and
 * events to be plotted on screen.
 */
class EventsManager : public QObject
{
  Q_OBJECT

  /// List of events for each task (task -> events)
  QMap<Task*, QList<Event*>> _tasksEvents;

  /// List of events for each CPU (cpu -> events). Same data as _tasksEvents, but shaped differently for efficiency
  QMap<CPU*,  QList<Event*>> _cpusEvents;

  /// List of all tasks encountered
  QVector<Task*> _tasks;

  /// List of all CPUs encountered
  QVector<CPU*>  _cpus;

  /// Islands of big-little architecture, if any
  QVector<Island_BL*> _islands;

  /// Folder of the current trace file
  QString _currentFolder;

  TICK last_event_tick;

  qreal last_magnification;

  /// reference to the main window, so to notify when all events have been added and plot them
  MainWindow* mainWindow;

private:
  // private constructor to prevent instantiation
  EventsManager(QObject *parent = 0);

  ~EventsManager() {
      qDebug() << __func__;
  }

public:
  // --------------------------- singleton stuff
  static EventsManager& getInstance()
  {
      static EventsManager    instance; // Guaranteed to be destroyed.
                                        // Instantiated on first use.
      return instance;
  }

  EventsManager(EventsManager const&, QObject *parent = 0) : QObject(parent)  { qDebug() << "do not call"; abort(); } // Don't Implement

  void operator=(EventsManager const&) { qDebug() << "do not call"; abort(); } // Don't implement

    // ------------------------- functions

  /// empties events (EventsManager reset)
  void clear();

  /// returns events number - O(#tasks)
  unsigned long countEvents() const {
    unsigned long res = 0;
    for (QList<Event*> elem : _tasksEvents) {
        res += elem.size();
    }
    return res;
  }


  /// Returns the list of all events associated with all tasks
  inline QMap<Task *, QList<Event *>> getAllTasksEvents() const { return _tasksEvents; }

  /// returns the list of all events associated with all CPUs
  inline QMap <CPU*, QList<Event*>> getAllCPUsEvents()  const { return _cpusEvents;   }

  /// groups events by CPUs
  QVector<QPair<CPU *, QList<Event *>>> getAggregatedEventsByCPUs() const;

  /// set main window
  void setMainWindow(MainWindow* mw) { this->mainWindow = mw; }

  /// returns the number of tasks
  unsigned long countTasks();

  /// returns the folder where taking plot data
    inline QString getCurrentFolder() const { return _currentFolder; }

    /// set the folder where the plot takes its data from
    inline void setCurrentFolder(QDir f) { _currentFolder = f.absolutePath(); if (!_currentFolder.endsWith("/")) _currentFolder+= "/"; }

    QList<Event> * getCallerEventsList(unsigned long caller);

    /// returns the tick of the last registered event
    inline TICK getLastEventTick() const { return last_event_tick; }

    /// for big-LITTLE. returns the 2 islands
    inline QVector<Island_BL*> getIslands() { return _islands; }

    /// returns all tasks
    inline QVector<Task*> getTasks() { return _tasks; }

    /// returns all CPUs
    inline QVector<CPU*> getCPUs() { return _cpus; }

    /// same as getCPUs(), but returns a list
    QList<QString> getCPUList();

    QMap<QString, QList<QString> > *getTasks(QString core, unsigned int time);

    /// read tasks data
    void readTasks();

    /// read CPUs data
    void readCPUs();

    /// adds a frequency change event (big-LITTLE)
    void addFrequencyChangeEvents();

    /// adds stuff related to DAGs
    void addDAGs();

    /// Returns the minimum scheduling tick in the .pst file (only performed once)
    unsigned long getMinimumSchedulingTick(bool reset = false) const;

    /// makes ticks begin from t=0 instead of sometime in the future
    void moveBackTicks();

    /// remove event from the fields
    void deleteEvent(Event* e);

    /// Returns the task given its name
    Task* getTaskByName(QString &name) {
        for (Task* t : _tasks)
            if (t->name == name)
                return t;
        return NULL;
    }

    /// Returns the CPU given its name
    CPU* getCPUByName(QString &name) {
        for (CPU* t : _cpus)
            if (t->getName() == name)
                return t;
        return NULL;
    }

public slots:
    /// called when a new event is registered
    void newEventArrived(Event* e);

    qreal magnify(qreal start, qreal end, qreal width);
};

#endif // EVENTSMANAGER_H
