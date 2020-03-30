#ifndef EVENTSPARSER_H
#define EVENTSPARSER_H

#include "event.h"

#include <QFile>
#include <QGraphicsItem>
#include <QDebug>

#define EVENTSPARSER EventsParser::getInstance()

class EventsManager;

// Singleton
class EventsParser
{
  EventsManager* _em;

  /// first tick to be parsed
  TICK _startingTick = 0;

  /// last tick to be parsed. If 0, then this parameter is discarded
  TICK _finalTick = std::numeric_limits<TICK>::max();

  /// Number of read lines from file
  unsigned int _readLines = 0;
private:

  // private constructor to prevent instantiations
  EventsParser() {}

  ~EventsParser() {
    qDebug() << __func__;
  }

  void completeSchedulingEvents();

public:

  // --------------------------------- singleton stuff

  static EventsParser& getInstance()
  {
      static EventsParser    instance; // Guaranteed to be destroyed.
                                       // Instantiated on first use.
      return instance;
  }

  EventsParser(EventsParser const&)   { qDebug() << "do not call"; abort(); }   // Don't Implement

  void operator=(EventsParser const&) { qDebug() << "do not call"; abort(); }   // Don't implement


  // ---------------------------------- functions to get private fields

  inline void setStartingTick(TICK st) { _startingTick = st; }
  inline unsigned long getStartingTick() { return _startingTick; }

  inline void setFinalTick(TICK st) { _finalTick = st; }
  inline unsigned long getFinalTick() { return _finalTick; }

  inline void setReadLines(unsigned int rl) { _readLines = rl; }
  inline unsigned int getReadLines() const { return _readLines; }

  // ---------------------------------- other functions

  void parseFile(QString filename);

  void parseFrequencies();

  void print() const;
};

#endif // EVENTSPARSER_H
