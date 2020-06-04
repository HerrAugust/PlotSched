#ifndef _CPU_H_
#define _CPU_H_

#include "task.h"
#include "config.h"

#include <QMap>
#include <QVector>

class CPU
{
private:
    static unsigned int _currentidCPU;

protected:
    /// CPU name
    QString _name;

    /// CPU unique identifier
    unsigned int _id;

    /// tasks that the CPU holds at a given time, for each time (i.e., tick)
    QMap<TICK, QVector<Task *>> tasksOverTime;

    /// CPU utilization and active utilizations over time. Active utilization defaults to 0.0
    QMap<TICK, QPair<double, double>> _utils;

public:

    CPU(QString name) : _name(name)
    {
        _id = _currentidCPU;
        _currentidCPU++;
    }

    virtual ~CPU() = default;

    inline QString getName() const { return _name; }

    inline unsigned int getID() const { return _id; }

    /// List of tasks at given tick t
    QVector<Task *> getTasksAtTime(TICK tick)
    {
        return tasksOverTime.lowerBound(tick).value();
    }

    /// The utilization and active utilization (default 0.0) at tick t or the closest tick from the past
    QPair<double, double> getUtilizationAt(TICK t)
    {
        if (_utils.isEmpty())
            return QPair<double, double>(0.0, 0.0);
        else
            return _utils.lowerBound(t).value();
    }

    /// reads both utilizations and active utilizations over time
    void readUtilizationsOverTime(QString filename);

    /// resets the CPU IDs
    static void resetIDs() { CPU::_currentidCPU = 0; }

    virtual QString str() const
    {
        return "CPU " + _name + ", id: " + QString::number(_id);
    }

    bool operator==(const CPU &other) { return _id == other._id; }

    bool operator<(const CPU &other) { return _id < other._id; }
};

class Island_BL;
/// CPU big-little
class CPU_BL : public CPU
{
protected:
    /// Island this core belongs to
    Island_BL *_island;

public:
    CPU_BL(QString name, Island_BL *island)
        : CPU(name), _island(island) {}

    Island_BL *getIsland() const { return _island; }

    void setIsland(Island_BL *i) { _island = i; }

    bool isBig() const;

    virtual QString str() const;

    //    QVector<QPair<TICK, double>> getFrequenciesOverTimeInRange(TICK t1, TICK t2) { return _island->getFrequenciesOverTimeInRange(t1, t2); }

    //    double getSpeed(double freq) const { return _island->getSpeed(freq); }
};

/// Island big-little
class Island_BL
{
protected:
    /// Frequency over time for the island
    QMap<TICK, double> _frequencies;

    /// 200 MHz -> 0.00021, 1200 MHz -> 0.7777, etc.
    static QMap<double, double> _speeds_big, _speeds_little;

    /// CPUs composing this island
    QVector<CPU_BL *> _cpus;

    bool _isBig = true;

public:
    Island_BL() {}

    void setBig(bool isbig) { _isBig = isbig; }

    void setCPUs(QVector<CPU_BL *> cpus) { _cpus = cpus; }

    static void readFrequencySpeed(QString filenameSpeeds, QString island_name);

    void readFrequenciesOverTime(QString filenameFrequenciesOverTime);

    double getFrequencyAt(TICK t) const
    {
        return _frequencies.lowerBound(t).value();
    }

    QMap<TICK, double> getFrequencies() const { return _frequencies; }

    QVector<CPU_BL *> getProcessors() const { return _cpus; }

    bool isBig() const { return _isBig; }

    double getSpeed(double freq) const
    {
        Q_ASSERT(_speeds_big.size() > 0 && _speeds_little.size() > 0);
        double f = _speeds_big.toStdMap().at(freq);
        if (!isBig())
            f = _speeds_little.toStdMap().at(freq);
        return f;
    }

    void moveBackTicks(TICK minTick);

    QVector<QPair<TICK, double>> getFrequenciesOverTimeInRange(TICK t1, TICK t2);
};

#endif
