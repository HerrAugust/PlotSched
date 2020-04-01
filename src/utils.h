#include <QFile>
#include <QDir>
#include <QVector>

/**
 * Looks for a file in all subdirectories of rootDir.
 */
static QVector<QString> searchFileInAllSubdirs(QString filename, QString rootDir, QVector<QString>* _partialResult = new QVector<QString>())
{
    QDir dir(rootDir);
    QDir::Filters recursiveFilter = QDir::Files | QDir::NoDotAndDotDot | QDir::Dirs;
    QStringList matches = dir.entryList(recursiveFilter, QDir::Name | QDir::DirsFirst);
    foreach (const QString &entry, matches)
    {
        QFileInfo fInfo(dir, entry);
        if (fInfo.isDir())
            searchFileInAllSubdirs(filename, QDir(fInfo.absoluteFilePath()).absolutePath(), _partialResult);
        else if (fInfo.absoluteFilePath().endsWith(filename))
            if (!_partialResult->contains(fInfo.absoluteFilePath()))
                _partialResult->push_back(fInfo.absoluteFilePath());
    }

    return *_partialResult;
}
