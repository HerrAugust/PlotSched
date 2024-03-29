#include "eventsparser.h"
#include "eventsmanager.h"
#include "utils.h"

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
    _completeSchedulingEvents();

    if (isFileEmpty)
    {
        QMessageBox m;
        m.setText("Error:" + path + " is empty!");
        m.exec();
    }
}

void EventsParser::parseFrequencies()
{
    auto tempb = searchFileInAllSubdirs("freqBIG.txt", EVENTSMANAGER.getCurrentFolder());
    auto templ = searchFileInAllSubdirs("freqLITTLE.txt", EVENTSMANAGER.getCurrentFolder());
    if (tempb.size() == 0 || templ.size() == 0)
    {
        qDebug() << "Could not find freqBIG.txt or freqLITTLE.txt in any subdirs of " << EVENTSMANAGER.getCurrentFolder() << ". Skip";
        return;
    }

    QString filenameBig = tempb.at(0);    // todo add a setting in settingsdialog
    QString filenameLittle = templ.at(0); // todo add a setting in settingsdialog

    qDebug() << "Trying to read frequencies over time for "
                "both big and little islands from folder"
             << EVENTSMANAGER.getCurrentFolder();
    qDebug() << "freq big island: " << filenameBig;
    qDebug() << "freq litte island: " << filenameLittle;

    // read the frequencies over time of the islands
    QVector<Island_BL *> islands = EVENTSMANAGER.getIslands();
    for (Island_BL *island : islands)
        if (island->isBig())
            island->readFrequenciesOverTime(filenameBig);
        else
            island->readFrequenciesOverTime(filenameLittle);
}

void EventsParser::_completeSchedulingEvents()
{
    // for each start event, if it does not have a corresponding end event, add it
    QMap<Task *, Event *> addedEvents;

    for (const auto& entry : EVENTSMANAGER.getAllCPUsEvents().values()) {
        for (int i = 0; i < entry.size(); ++i) { // for each event associated with a CPU
            Event* evt = entry.at(i);
            Event* endEvtAfter = NULL;
            if (evt->getStatus() != "S") continue;
            for (int j = i + 1; j < entry.size(); ++j) {
                if (entry.at(j)->getTask() == evt->getTask()) {
                    if (entry.at(j)->getStatus() == "I") // keep staying the same instance
                        j = entry.size();
                    else if (entry.at(j)->getStatus() == "E") {
                        endEvtAfter = entry.at(j);
                        j = entry.size();
                    }
                }
            }
            qDebug() << evt->str();

            if (endEvtAfter == NULL)
            {
                TICK dur = 50;
                Event *evtend = new Event(evt->getStart(), dur, evt->getCPU(), evt->getTask(), "RUNNING", RUNNING);
                evtend->setStatus("E");
                evtend->setMagnification(evt->getMagnification());
                evtend->setHasFinished(false);
                qDebug() << "Completing event " << evt->str() << " with " << evtend->str();

                addedEvents.insert(evt->getTask(), evtend);
                EVENTSMANAGER.newEventArrived(evtend);
            }
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
