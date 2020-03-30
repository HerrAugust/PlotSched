#include "eventsmanager.h"
#include "mainwindow.h"
#include "eventview.h"
#include "dag.h"

#include <limits>
#include <QDebug>

EventsManager::EventsManager(QObject *parent) : QObject(parent)
{
    last_event = 0;
    last_magnification = 1;
}

void EventsManager::clear()
{
    //  events_container.clear();
    for (const auto& list : getAllTasksEvents().values())
        for (const auto& e : list)
            delete e;
    for (const auto& t : _tasks)
        delete t;
    for (const auto& c : _cpus)
        delete c;
    _tasksEvents.clear();
    _cpusEvents.clear();
    _tasks.clear();
    _cpus.clear();
    last_magnification = 1;
    last_event = 0;
    CPU::resetIDs();
}

void EventsManager::addDAGs()
{
    QFile file(_currentFolder + QString::fromStdString("graphs.txt"));
    QFileInfo fileinfo(file);
    if (fileinfo.exists())
    {
        if (!file.open(QIODevice::ReadOnly))
        {
            qDebug() << "error while reading from " << fileinfo.absoluteFilePath() << ": " << file.errorString();
        }

        QTextStream in(&file);
        while (!in.atEnd())
        {
            QString line = in.readLine();
            QStringList fields = line.split(" ");
            QString taskname  = fields.at(0);
            QString pathgraph = _currentFolder + QString(fields.at(1));
            unsigned int row  = QString(fields.at(2)).toUInt();

            QStringList nameFilter("*adj_mx.txt");
            QDir directory(pathgraph);

            QString adj_mx_path = pathgraph + directory.entryList(nameFilter).at(0);
            DAG* dag = new DAG();
            dag->fromFile(adj_mx_path);

            // assiate tasks with nodes. O(n^2)
            for (const auto& task : getTasks())
                for (const auto& node : dag->getNodes())
                    if (task->getName() == node->getName())
                        task->setNode(node);
        }
        file.close();
    }
}

void EventsManager::addFrequencyChangeEvents()
{
    for (const Island_BL *isl : _islands)
    {
        for (const auto &fc : isl->getFrequencies().toStdMap())
        {
            //            qDebug() << __func__ << " " << (isl->isBig() ? "big " : "little ") << fc.first << " " << fc.second << endl;
            for (CPU_BL *cpu : isl->getProcessors())
            {
                Event *e = new Event(fc.first, 0, cpu, NULL, "FREQ_CHG", FREQUENCY_CHANGE);
                _cpusEvents[cpu].push_back(e);
            }
        }
    }
}

void EventsManager::moveBackTicks()
{
    qDebug() << __func__ << "()";

    TICK minTick = getMinimumSchedulingTick();

    for (CPU *cpu : _cpusEvents.keys())
    {
        QList<Event *> le = _cpusEvents[cpu];
        for (Event *evt : le)
        {
            Q_ASSERT(evt->getStart() >= minTick);
            evt->setStart(evt->getStart() - minTick);
        }
    }

    for (Island_BL *isl : getIslands())
    {
        isl->moveBackTicks(minTick);
    }
}

TICK EventsManager::getMinimumSchedulingTick(bool reset) const
{
    // first tick is taken from the .pst file (i.e., only scheduling evts)
    static TICK minTick = std::numeric_limits<TICK>::max();

    // user chooses another trace, for example
    if (reset)
        minTick = std::numeric_limits<TICK>::max();

    // if we have already computed the value (when the ser chooses a new .pst file), we return it
    if (minTick < std::numeric_limits<TICK>::max())
        return minTick;

    for (auto &elem : _cpusEvents.toStdMap())
    {
        QList<Event *> le = elem.second;
        for (Event *evt : le)
        {
            if (evt->getKind() == FREQUENCY_CHANGE)
                continue;

            TICK tick = evt->getStart();
            if (tick < minTick)
                minTick = tick;
        }
    }

    return minTick;
}

void EventsManager::readTasks()
{
    QString filename = _currentFolder + "/tasks.txt";
    qDebug() << "Trying to read tasks generalities from " + filename;
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "error while reading from " << filename << ": " << file.errorString();
    }

    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine();
        QStringList fields = line.split(" ");
        QString name = fields.at(0);
        unsigned int q = QString(fields.at(1)).toUInt(); // useless
        unsigned int wcet = QString(fields.at(2)).toUInt();
        unsigned int period = QString(fields.at(3)).toUInt();
        _tasks.push_back(new Task(name, wcet, period));
    }
    file.close();
}

void EventsManager::readCPUs()
{
    QString filename = _currentFolder + "/cpus.txt";
    qDebug() << "Trying to read CPUs generalities from " + filename;
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "error while reading from " << filename << ": " << file.errorString();
    }

    Island_BL *lastIsland;
    QVector<CPU_BL *> cpus;

    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine();
        QStringList fields = line.split(" ");
        if (line.startsWith("BEGIN ISLAND"))
        {
            bool isbig = true;
            if (QString(fields.at(2)).trimmed() == "LITTLE")
                isbig = false;
            lastIsland = new Island_BL();
            lastIsland->setBig(isbig);
            _islands.push_back(lastIsland);
        }
        else if (line.startsWith("END ISLAND"))
        {
            lastIsland->setCPUs(cpus);
            cpus.clear();
        }
        else
        { // cpu
            QString name = QString(fields.at(0)).trimmed();
            CPU_BL *c = new CPU_BL(name, lastIsland);
            _cpus.push_back(c);
            cpus.push_back(c);
        }
    }
    file.close();
}

void EventsManager::newEventArrived(Event *e)
{
    Q_ASSERT(e->getTask() != NULL);
    Q_ASSERT(e->getCPU() != NULL);

    // is it the last event I found until now?
    if (e->getStart() > last_event)
        last_event = e->getStart();

    // store tasks and CPU
    if (_tasks.contains(e->getTask()) == false)
        _tasks.append(e->getTask());
    if (_cpus.contains(e->getCPU()) == false)
        _cpus.append(e->getCPU());

    // add event to queue. task -> list<events>
    _tasksEvents[e->getTask()].push_back(e); // todo can be optimized by using id instead of task
    _cpusEvents[e->getCPU()].push_back(e);
}

QMap<Task *, QList<Event *>> EventsManager::getAllTasksEvents() const
{
    return _tasksEvents;
    //     if (!_tasksEvents.empty())
    //         return _tasksEvents;

    //     for (const auto &listevt : _cpusEvents.values())
    //         for (Event *e : listevt)
    //             _tasksEvents[e->getTask()].push_back(e);
}

qreal EventsManager::magnify(qreal start, qreal end, qreal width)
{
    qreal new_center;
    qreal fraction;
    qreal size = end - start;
    qreal magnification = width / size;

    new_center = (start + end) / 2 / last_magnification;

    if (size > 0)
        fraction = magnification;
    else
        fraction = -last_magnification / magnification;

    last_magnification = fraction;

    for (QList<Event *> e : _tasksEvents.values())
        for (Event *ee : e)
            ee->setMagnification(fraction);

    qDebug() << "The magnification is : " << fraction;
    if (size > 0)
        qDebug() << "The new margins are : " << start * fraction << " " << end * fraction;

    new_center *= last_magnification;

    return new_center;
}
