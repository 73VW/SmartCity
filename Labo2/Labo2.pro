#-------------------------------------------------
#
# Project created by QtCreator 2016-09-29T12:02:13
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Labo2
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    GUI/people.cpp \
    GUI/place.cpp \
    paramlist.cpp \
    algothread.cpp \
    GUI/camion.cpp

HEADERS  += mainwindow.h \
    GUI/people.h \
    GUI/place.h \
    paramlist.h \
    algothread.h \
    GUI/camion.h

FORMS    += mainwindow.ui \
    GUI/people.ui \
    GUI/place.ui \
    GUI/camion.ui

RESOURCES += \
    icon.qrc
