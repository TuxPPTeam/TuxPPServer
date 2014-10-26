#-------------------------------------------------
#
# Project created by QtCreator 2014-10-26T21:02:14
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TuxPPServer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    worker.cpp \
    user.cpp \
    mythread.cpp \
    myserver.cpp

HEADERS  += mainwindow.h \
    worker.h \
    user.h \
    mythread.h \
    myserver.h

FORMS    += mainwindow.ui
