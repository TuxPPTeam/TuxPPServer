QMAKE_CXXFLAGS += -std=c++11 -Wall -Wextra
QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TuxPPServer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    user.cpp \
    myserver.cpp \
    cryptor.cpp

HEADERS  += mainwindow.h \
    user.h \
    myserver.h \
    cryptor.h

FORMS    += mainwindow.ui
