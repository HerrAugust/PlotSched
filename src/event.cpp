#include "event.h"
#include "eventsparser.h"

#include <QTextStream>
#include <QMap>
#include <eventsmanager.h>

#include <QDebug>

unsigned Event::_counterEvents = 0;

QMap<QString, QMap<EVENT_KIND, Event *>> pending_events;

bool Event::parseLine(QByteArray line)
{
  QString taskname, cpuname;
  correct = false;

  QTextStream ss(line);


  ss >> time_start;
  ss >> taskname;
  ss >> cpuname;
  ss >> event;
  ss >> status;

  EVENTSPARSER.setReadLines(EVENTSPARSER.getReadLines() + 1);

  // warning, should you delete the task or cpu of an event you delete the task itself
  task = EVENTSMANAGER.getTaskByName(taskname);
  cpu  = EVENTSMANAGER.getCPUByName(cpuname); // tasks won't have sequential ID, they're shared with CPU's
  if (task == NULL)
      task = new Task(taskname);
  if (cpu == NULL)
      cpu  = new CPU(cpuname);

  if (time_start < EVENTSPARSER.getStartingTick() || time_start > EVENTSPARSER.getFinalTick())
      return false;

  if (event == "RUNNING") {
    kind = RUNNING;
  } else if (event == "DEAD") {
    kind = DEAD;
  }  else if (event == "BLOCKED") {
    kind = BLOCKED;
  } else if (event == "ACTIVATION" || event == "CREATION") {
    kind = ACTIVATION;
  } else if (event == "CONFIGURATION") {
    kind = CONFIGURATION;
  } else if (event == "DEADLINE") {
    kind = DEADLINE;
  } else if (event == "MISS") {
    kind = MISS;
  } else if (event == "FREQ_CHANGE") { // todo maybe useless
    kind = FREQUENCY_CHANGE;
  }

  if (status == "I") {
    correct = true;
    pending = false;
  } else if (status == "E") {
    pending = false;
    // this is an end event for a job. find its corresponding scheduling event to compute the duration
    if (pending_events[task->name].find(kind) != pending_events[task->name].end()) {
      duration = time_start - pending_events[task->name].find(kind).value()->getStart();
      this->time_start = pending_events[task->name].find(kind).value()->getStart();
      correct = true;

      Event * ev = pending_events[task->name].find(kind).value();
      pending_events[task->name].remove(kind);
      delete ev;
    }
  }else if (status == "S") {
    correct = true;
    pending = true;
    Event * ev = new Event(*this);
    pending_events[task->name].insert(kind, ev);
  }

  return true;
}


bool correctLine(QByteArray line)
{
  if (line.size() < 2)
    return false;
  return true;
}


Event::Event()
{
  magnification = 1;
  duration = 0;
  correct = false;
}


void Event::parse(QByteArray line)
{
  if (correctLine(line))
    parseLine(line);
}


Event::Event(const Event &o)
{
  time_start = o.time_start;
  duration = o.duration;
  cpu = o.cpu;
  row = o.row;
  task = o.task;
  event = o.event;
  kind = o.kind;

  magnification = o.magnification;

  correct = o.correct;
  pending = o.pending;
  range = o.range;

  _id = o._id;
}


Event& Event::operator=(const Event &o)
{
  time_start = o.time_start;
  duration = o.duration;
  cpu = o.cpu;
  row = o.row;
  task= o.task;
  event = o.event;
  kind = o.kind;

  magnification = o.magnification;

  correct = o.correct;
  pending = o.pending;
  range = o.range;

  _id = o._id;

  return *this;
}
