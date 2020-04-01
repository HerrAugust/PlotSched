#include "mainwindow.h"
#include "event.h"
#include "settingsdialog.h"
#include "cpu.h"

#include <limits>
#include <QApplication>
#include <QDebug>

// default paths for speeds
#define FN_SPEEDS_ODROID_XU3_BZIP2_LITTLE QCoreApplication::applicationDirPath().append("/../src/assets/speedsLittle_Odroid_bzip2.txt")
#define FN_SPEEDS_ODROID_XU3_BZIP2_BIG QCoreApplication::applicationDirPath().append("/../src/assets/speedsBig_Odroid_bzip2.txt")

void onFirstStart()
{
    int res = SettingsManager::getInt(SettingsManager::Key::IS_FIRST_START, 0);
    if (res == 0)
    {
        SettingsManager::save(SettingsManager::Key::SPEEDS_BL_PATH_LITTLE, FN_SPEEDS_ODROID_XU3_BZIP2_LITTLE);
        SettingsManager::save(SettingsManager::Key::SPEEDS_BL_PATH_BIG, FN_SPEEDS_ODROID_XU3_BZIP2_BIG);
        SettingsManager::saveInt(SettingsManager::Key::IS_FIRST_START, 0);
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow *w;

    qDebug() << "Welcome in " << QString(argv[0]) << ". Should anything go wrong, try:";
    qDebug() << "\t" << QString(argv[0]) << " --reset";

    onFirstStart();

    TICK startingTick = 0, finalTick = std::numeric_limits<unsigned int>::max();
    QString filename_frequencies_big = "",
            filename_frequencies_little = "",
            filename_tasks = "", filename_cpus = "",
            folderSpeedsLittle = SettingsManager::getString(SettingsManager::Key::SPEEDS_BL_PATH_LITTLE),
            folderSpeedsBig = SettingsManager::getString(SettingsManager::Key::SPEEDS_BL_PATH_BIG);

    while (argc > 0)
    {
        if (QString(argv[0]) == "--delete-last-trace")
        {
            qDebug() << "deleting last selected trace";
            SettingsManager::removeKey(SettingsManager::Key::LAST_PST_PATH);
        }
        else if (QString(argv[0]) == "-i")
        { // input trace file filename
            argv++;
            QString lastPath = QString(argv[0]);
            SettingsManager::saveFile(SettingsManager::Key::LAST_PST_PATH, lastPath);
        }
        else if (QString(argv[0]) == "--filename-speed-big")
        {
            argv++;
            folderSpeedsBig = QString(argv[0]);
            SettingsManager::saveFile(SettingsManager::Key::SPEEDS_BL_PATH_BIG, folderSpeedsBig);
        }
        else if (QString(argv[0]) == "--filename-speed-little")
        {
            argv++;
            folderSpeedsLittle = QString(argv[0]);
            SettingsManager::saveFile(SettingsManager::Key::SPEEDS_BL_PATH_LITTLE, folderSpeedsLittle);
        }
        else if (QString(argv[0]) == "-s")
        { // starting tick to be considered in plot
            argv++;
            startingTick = QString(argv[0]).toLong();
        }
        else if (QString(argv[0]) == "-f")
        { // final tick to be considered in plot
            argv++;
            finalTick = QString(argv[0]).toLong();
        }
        else if (QString(argv[0]) == "--reset")
        {
            SettingsManager::removeAllKeys();
        }
        argc--;
        argv++;
    }

    qRegisterMetaType<Event>("Event");

    Island_BL::readFrequencySpeed(folderSpeedsBig, QString("big"));
    Island_BL::readFrequencySpeed(folderSpeedsLittle, QString("little"));

    w = new MainWindow(startingTick, finalTick);
    w->show();

    // at this point no trace file (pst) has been opened

    return a.exec();
}
