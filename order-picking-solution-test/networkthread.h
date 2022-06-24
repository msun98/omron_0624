#ifndef NETWORKTHREAD_H
#define NETWORKTHREAD_H

#include <QThread>
#include <iostream>
#include <QtNetwork>
#include <string.h>

#include <QTcpSocket>
#include <QTcpServer>


enum{RBLAN_SUCCESS = 0, RBLAN_FAIL};
enum{RBLAN_CS_CONNECTED  = 0, RBLAN_CS_DISCONNECTED};

using namespace std;

class NetworkThread : public QThread
{
    Q_OBJECT
public:
    explicit NetworkThread(QObject *parent = 0);

    void RunNetwrok();

    bool RBServerOpen(QHostAddress::SpecialAddress _host = QHostAddress::Any, quint16 _port = 5000);
    void RBSendData(QByteArray &data);

    int RBConnectionState = RBLAN_CS_DISCONNECTED;

    QVector<QByteArray> dataReceived;

    QByteArray RBData;

    QString status;
    float baterry, temperature;
    int x,y,heading;

    void InitParameter();

private slots:
    void    RBNewConnection();
    void    RBClientDisconnected();
    void    RBReadData();

protected:
    QTcpServer                      *RBTcpServer;
    QTcpSocket                      *RBTcpClient;
private:
    void run();
    QHostAddress::SpecialAddress    RBHostAddress;
    quint16                         RBPortNumber;

};

#endif // NETWORKTHREAD_H
