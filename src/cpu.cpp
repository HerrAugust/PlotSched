// -------------------------------- Island BL, CPU BL

#include "cpu.h"
#include "eventsmanager.h"

#include <QFile>
#include <QDebug>
#include <sstream>

QMap<double, double> Island_BL::_speeds_big, Island_BL::_speeds_little;
unsigned int CPU::_currentidCPU = 0;

void Island_BL::readFrequencySpeed(QString filenameSpeeds, QString island_name)
{
    // f1 speed bzip2
    // ...
    // fn speed bzip2

    qDebug() << "Trying to read frequency -> speed from " << filenameSpeeds;
    QFile file(filenameSpeeds);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "error while reading from " << filenameSpeeds << ": " << file.errorString();
    }

    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine();
        QStringList fields = line.split(" ");
        double f0 = QString(fields.at(0)).toDouble();
        double f1 = QString(fields.at(1)).toDouble();
        if (island_name == "big")
            Island_BL::_speeds_big.insert(f0, f1);
        else
            Island_BL::_speeds_little.insert(f0, f1);
    }
    file.close();
}

void Island_BL::readFrequenciesOverTime(QString filenameFrequenciesOverTime)
{
    QFile file(filenameFrequenciesOverTime);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "error while reading from " << filenameFrequenciesOverTime << ": " << file.errorString();
    }

    TICK minTick = EVENTSMANAGER.getMinimumSchedulingTick();
    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine();
        QStringList fields = line.split(" ");
        TICK f0 = QString(fields.at(0)).toUInt();
        double f1 = QString(fields.at(1)).toDouble();
        if (f0 >= minTick)
            this->_frequencies.insert(f0, f1);
    }
    file.close();

    qDebug() << "Read # frequencies over time: " << _frequencies.size();
}

void Island_BL::moveBackTicks(unsigned long minTick)
{
    for (const auto &elem : _frequencies.toStdMap())
    {
        if (elem.first >= minTick)
        {
            TICK t = elem.first - minTick;
            double f = elem.second;
            _frequencies.remove(elem.first);
            _frequencies[t] = f;
        }
    }
}

QVector<QPair<TICK, double>> Island_BL::getFrequenciesOverTimeInRange(TICK t1, TICK t2)
{
    QVector<QPair<TICK, double>> res;
    QPair<TICK, double> last_freq;

    for (const auto &elem : _frequencies.toStdMap())
    {
        if (elem.first >= t1 && elem.first <= t2)
        {
            res.push_back(QPair<TICK, double>(elem.first, elem.second));
        }
        else if (elem.first < t1)
            last_freq = QPair<TICK, double>(t1, elem.second);
    }

    if (res.size() == 0)
    {
        // try with the speed right before t1
        res.push_back(last_freq);
    }

    return res;
}

bool CPU_BL::isBig() const
{
    return getIsland()->isBig();
}

QString CPU_BL::str() const
{
    std::stringstream ss;
    ss << CPU::str().toStdString();
    ss << " " << (_island->isBig() ? "big" : "little");
    return QString::fromStdString(ss.str());
}

void CPU::readUtilizationsOverTime(QString filename)
{
    // t1 cpuid util util_active
    // ...
    // tn cpuid util util_active

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "error while reading from " << filename;
    }

    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine();
        QStringList fields = line.split(" ");
        TICK f0 = QString(fields.at(0)).toUInt();
        unsigned int f1 = QString(fields.at(1)).toUInt();
        double f2 = QString(fields.at(2)).toDouble();
        // double f3 = QString(fields.at(3)).toDouble();

        if (f1 == this->_id)
            this->_utils.insert(f0, QPair<double, double>(f1, f2));
    }
    file.close();
}
