#ifndef _DEFS_H_
#define _DEFS_H_

#include <QFileInfo>

#define ROUTINE_CANNOT_OPEN_FILE(file)                                                                        \
    {                                                                                                         \
        qDebug() << __func__ << ", cannot open " << QFileInfo(file).absoluteFilePath() << file.errorString(); \
        abort();                                                                                              \
    }

#define TICK unsigned long

#endif