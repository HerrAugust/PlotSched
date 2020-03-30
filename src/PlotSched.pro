#-------------------------------------------------
#
# Project created by QtCreator 2015-03-04T16:49:08
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = plotsched
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp\
    customtoolbar.cpp \
    tracefilelister.cpp \
    eventsparser.cpp \
    event.cpp \
    eventview.cpp \
    plot.cpp \
    eventsmanager.cpp \
    customscene.cpp \
    rangeselector.cpp \
    plotframe.cpp \
    settingsdialog.cpp \
    cpu.cpp \
    dag.cpp

HEADERS  += mainwindow.h\
    customtoolbar.h \
    tracefilelister.h \
    eventsparser.h \
    event.h \
    eventview.h \
    plot.h \
    eventsmanager.h \
    customscene.h \
    rangeselector.h \
    plotframe.h \
    settingsdialog.h \
    cpu.h \
    dag.h \
    task.h

FORMS    += mainwindow.ui \
    settingsdialog.ui

QMAKE_CXXFLAGS += -std=c++11

RESOURCES += \
    resources.qrc
