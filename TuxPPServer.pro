QMAKE_CXXFLAGS += -std=c++11 -Wall -Wextra
QT       += core gui network sql
QT += testlib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TuxPPServer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    user.cpp \
    cryptor.cpp \
    dbmanager.cpp \
    tests.cpp \
    server.cpp

HEADERS  += mainwindow.h \
    user.h \
    server.h \
    cryptor.h \
    dbmanager.h \
    tests.h

FORMS    += mainwindow.ui
