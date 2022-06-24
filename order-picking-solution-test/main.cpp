#include "mainwindow.h"

#include <QApplication>

#include "lift/RBDaemon.h"

extern int     _IS_WORKING;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    _IS_WORKING = true;
    RBCore_SMInitialize();
    RBCore_DBInitialize();
    RBCore_CANInitialize();
    RBCore_LANInitialize();
    RBCore_ThreadInitialize();


    return a.exec();
}
