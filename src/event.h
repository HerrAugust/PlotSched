#ifndef EVENT_H
#define EVENT_H

#include "task.h"
#include "cpu.h"
#include "config.h"

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QList>

enum EVENT_KIND
{
    ACTIVATION,
    DEAD,
    RUNNING,
    BLOCKED,
    DEADLINE,
    MISS,
    CONFIGURATION,
    FREQUENCY_CHANGE,
    NONE
};

class Event
{    
    static unsigned int _counterEvents;

    unsigned int _id;

    TICK time_start;
    TICK duration;
    CPU *cpu;
    unsigned int row, column; ///row and column of the event
    Task *task;
    QString event;
    EVENT_KIND kind;
    QString status;

    // In case of bugs, tasks are scheduled but they don't end
    bool _hasFinished = true;

    qreal magnification;

    bool correct;
    bool pending;
    bool range;

    bool parseLine(QByteArray b);

public:
    Event();
    Event(const Event &o);
    Event(TICK time_start, TICK duration, CPU *cpu, Task *task, QString event, EVENT_KIND kind)
    {
        this->time_start = time_start;
        this->duration = duration;
        this->cpu = cpu;
        this->task = task;
        this->event = event;
        this->kind = kind;

        _id = _counterEvents;
        _counterEvents++;
    }

    unsigned int getColumn() const { return column; }
    unsigned int getRow() const { return row; }
    void setColumn(unsigned int c) { column = c; }
    void setRow(unsigned int r) { row = r; }
    void setEvent(QString e) { event = e; }
    QString getEvent() const { return event; }
    void setStatus(QString s) { status = s; }
    QString getStatus() const { return status; }
    void setHasFinished(bool f) { _hasFinished = f; }
    bool hasFinished() const { return _hasFinished; }
    void setMagnification(qreal magnification) { this->magnification = magnification; }
    qreal getMagnification() const { return magnification; }
    bool isCorrect() const { return correct; }
    bool isPending() const { return pending; }
    bool isRange() const { return range; }
    TICK getStart() const { return time_start; }
    TICK getDuration() const { return duration; }
    void setStart(TICK t) { time_start = t; }
    void setDuration(TICK t) { duration = t; }
    Task *getTask() const { return task; }
    CPU *getCPU() const { return cpu; }
    EVENT_KIND getKind() const { return kind; }

    /// Fill this event fields given a line (string)
    void parse(QByteArray line);

    Event &operator=(const Event &o);

    inline bool operator==(const Event& other){ return _id == other._id; }

    QString str() const
    {
        QString s = QString("t=%1, %2 %3 for %4 on %5 dur %6").arg(QString::number(time_start), event, status, (task == NULL ? "" : task->str()), (cpu == NULL ? "" : cpu->str()), QString::number(duration));
        return s;
    }
};

#endif // EVENT_H
