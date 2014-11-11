#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

#ifdef TEST
    QTest::qExec(&t);
    Tests t;
#else
    MainWindow w;
    w.show();
#endif


    return a.exec();
}
