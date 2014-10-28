QMAKE_CXXFLAGS += -std=c++11 -Wall -Wextra
QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TuxPPServer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    worker.cpp \
    user.cpp \
    mythread.cpp \
    myserver.cpp \
    cryptor.cpp

HEADERS  += mainwindow.h \
    worker.h \
    user.h \
    mythread.h \
    myserver.h \
    cryptor.h

FORMS    += mainwindow.ui
