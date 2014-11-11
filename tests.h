#ifndef TESTS_H
#define TESTS_H

#include <QtTest/QtTest>
#include "dbmanager.h"

class Tests: public QObject
{
    Q_OBJECT
private slots:
    void testConnection();
};

#endif // TESTS_H
