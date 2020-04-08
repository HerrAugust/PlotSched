#ifndef _TASK_H_
#define _TASK_H_

#include "config.h"

#include <QString>

static unsigned int _currentidTask = 0;

class Node;

class Task
{
    /// the node associated with the DAG
    Node* _node = NULL;

public:
    /// task name
    QString name;

    /// task unique identifier
    unsigned int id;

    /// CBS budget
    // TICK Q = 0;

    /// task WCET and period
    TICK WCET = 0, period = 0, _relDl = 0;


    Task(QString name = "",
         /*unsigned int q = 0,*/
         unsigned int wcet = 0,
         unsigned int period = 0,
         unsigned int relDl = 0)
        : name(name)
        /*, Q(q) */,
          WCET(wcet),
          period(period),
          _relDl(relDl)
    {
        id = _currentidTask;
        _currentidTask++;
    }

    inline TICK getRelDl() const { return _relDl; }

    inline TICK getWCET() const { return WCET; }

    inline QString getName() const { return name; }

    inline Node* getNode() const { return _node; }

    inline void setNode(Node* node) { _node = node; }

    QString toString() const
    {
        return name + ", id: " + QString::number(id) + " (" + QString::number(WCET) + ", " + QString::number(period) + ")";
    }

    QString str() const { return toString(); }


    bool operator==(const Task &other) { return id == other.id; }

    bool operator<(const Task &other) { return id < other.id; }
};

#endif
