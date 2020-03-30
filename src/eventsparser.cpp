#include "eventsparser.h"
#include "eventsmanager.h"
#include <QDebug>
#include <QFileInfo>
#include <QList>
#include <QMessageBox>

void EventsParser::parseFile(QString path)
{
    qDebug() << __func__ << "()";

    QString result;
    QFile f(path);
    bool isFileEmpty = true;

    if (f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        while (!f.atEnd())
        {
            Event *e = new Event();
            e->parse(f.readLine());
            if (e->isCorrect())
            {
                EVENTSMANAGER.newEventArrived(e);
            }
            isFileEmpty = false;
        }
    }

    // for some tasks only their last running event has been stored
    // but not the corresponding end event (in case of bugs)
    completeSchedulingEvents();

    if (isFileEmpty)
    {
        QMessageBox m;
        m.setText("Error:" + path + " is empty!");
        m.exec();
    }
}

void EventsParser::parseFrequencies()
{
    QString filenameBig = EVENTSMANAGER.getCurrentFolder() + "/freqBIG.txt"; // todo add a setting in settingsdialog
    QString filenameLittle = EVENTSMANAGER.getCurrentFolder() + "/freqLITTLE.txt"; // todo add a setting in settingsdialog

    qDebug() << "Trying to read frequencies over time for "
                "both big and little islands from folder"
             << EVENTSMANAGER.getCurrentFolder();

    // read the frequencies over time of the islands
    QVector<Island_BL *> islands = EVENTSMANAGER.getIslands();
    for (Island_BL *island : islands)
        if (island->isBig())
            island->readFrequenciesOverTime(filenameBig);
        else
            island->readFrequenciesOverTime(filenameLittle);
}

void EventsParser::completeSchedulingEvents()
{
    QMap<Task *, Event *> addedEvents;

    QMapIterator<CPU *, QList<Event *>> i(EVENTSMANAGER.getAllCPUsEvents());
    while (i.hasNext())
    {
        i.next();
        Event *const evt = i.value().last();
        if (evt->getStatus() == "S")
        {
            TICK dur = EVENTSMANAGER.getLastEvent() - evt->getStart() + 1000;
            Event *evtend = new Event(evt->getStart(), dur, evt->getCPU(), evt->getTask(), "RUNNING", RUNNING);
            evtend->setStatus("E");
            evtend->setMagnification(evt->getMagnification());
            evtend->setHasFinished(false);

            addedEvents.insert(evt->getTask(), evtend);
            EVENTSMANAGER.newEventArrived(evtend);
        }
    }

    qDebug() << "Found # not completed tasks: " << addedEvents.size();
    for (const auto &elem : addedEvents.toStdMap())
    {
        qDebug() << "\t" << elem.first->str() << " -> " << elem.second->str();
    }
}

void EventsParser::print() const
{
    qDebug() << "---------------------------------------";
    qDebug() << "Read lines from file: " << QString::number(EVENTSPARSER.getReadLines());

    QMap<Task *, QList<Event *>> tasksEvents = EVENTSMANAGER.getAllTasksEvents();
    qDebug() << "Parser has finished its job. Found "
             << "#tasks: " << EVENTSMANAGER.getTasks().size()
             << "#CPU: " << EVENTSMANAGER.getCPUs().size()
             << "#events: " << EVENTSMANAGER.countEvents();

    qDebug() << "Tasks:";
    for (Task *e : EVENTSMANAGER.getTasks())
        qDebug() << e->str();
    qDebug() << "CPUs:";
    for (CPU *e : EVENTSMANAGER.getCPUs())
        qDebug() << e->str();
    qDebug() << "Events:"; // heavy to print them out. Use for debug
    for (auto &elem : tasksEvents.toStdMap())
    {
        qDebug() << elem.first->str();
        QList<Event *> evts = elem.second;
        for (Event *e : evts)
            qDebug() << "\t" << e->str();
    }
    qDebug() << "---------------------------------------";
}
