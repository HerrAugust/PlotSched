#ifndef _TASK_H_
#define _TASK_H_

#include "config.h"

#include <QObject>

static unsigned int currentid = 0;

class Task : public QObject
{
    Q_OBJECT

public:
    /// task name
    QString name;

    /// task unique identifier
    unsigned int id;

    /// CBS budget
    // TICK Q = 0;

    /// task WCET and period
    TICK WCET = 0, period = 0;

    Task(QString name = "", /*unsigned int q = 0,*/ unsigned int wcet = 0, unsigned int period = 0) : name(name) /*, Q(q) */, WCET(wcet), period(period)
    {
        id = currentid;
        currentid++;
    }

    QString getName() const { return name; }

    QString toString() const
    {
        return name + ", id: " + QString::number(id) + " (" + QString::number(WCET) + ", " + QString::number(period) + ")";
    }

    QString str() const { return toString(); }

    TICK getWCET() const { return WCET; }

    bool operator==(const Task &other) { return id == other.id; }

    bool operator<(const Task &other) { return id < other.id; }
};

#endif
