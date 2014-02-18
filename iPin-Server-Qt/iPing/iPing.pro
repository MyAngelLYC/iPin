#-------------------------------------------------
#
# Project created by QtCreator 2013-05-31T15:54:35
#
#-------------------------------------------------

QT       += core gui
QT       += network
QT       += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = iPing
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    mythread.cpp \
    msgthread.cpp \
    subthread.cpp \
    timethread.cpp

HEADERS  += mainwindow.h \
    mythread.h \
    msgthread.h \
    subthread.h \
    timethread.h

FORMS    += mainwindow.ui
