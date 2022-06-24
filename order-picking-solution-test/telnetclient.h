#ifndef TELNETCLIENT_H
#define TELNETCLIENT_H

#include <QObject>
#include <QThread>
#include <iostream>
#include <stdio.h>
#include "impl/AsioTelnetClient.h"

using namespace std;

class telnetclient : public QThread
{
    Q_OBJECT
public:
   explicit telnetclient(QObject *parent = 0);
   void RunTelnetClient();
   int flag = 0;
   string sendCommand ="";
   string serverMessage = "";

private:
    void run();
};

#endif // TELNETCLIENT_H
