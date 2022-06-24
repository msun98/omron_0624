#include "networkthread.h"


NetworkThread::NetworkThread(QObject *parent) : QThread(parent)
{
    cout << "Hi Mobile Network" << endl;
    if (RBServerOpen(QHostAddress::AnyIPv4, 7179))
    {
        cout << "Mobile Server Open" << endl;
    }

}


void NetworkThread::run()
{
    //RBPilot_ReadThread(this);
    RunNetwrok();
}


void NetworkThread::InitParameter()
{
    status="";
    x = 0;
    y = 0;
    heading = 0;
    temperature = 0;
    baterry = 0;
}

void NetworkThread::RunNetwrok()
{
    int a = 0;
    //서버 열기
    while(1)
    {
        usleep(10*1000);

        if(RBConnectionState == RBLAN_CS_DISCONNECTED)
        {

        }
        else
        {
            RBReadData();

            if (dataReceived.size() > 0)
            {

                QByteArray data = dataReceived[0];

                dataReceived.pop_front();


                int data_length = data.length();
                QString str = QString::fromStdString(data.toStdString());

                if(data_length > 0)
                {

                    InitParameter();

                    int flag = -1;
                    int counter = 0;

                    QStringList strlist = str.split(" ");

                    for(int i=0;i<strlist.size();i++)
                    {
                        if(strlist[i] == "Status:")
                        {
                            flag = 0;
                        }
                        else if(strlist[i] == "StateOfCharge:")
                        {
                            flag = 1;
                        }
                        else if(strlist[i] == "Location:")
                        {
                            flag = 2;
                        }
                        else if(strlist[i] == "Temperature:")
                        {
                            flag = 3;
                        }
                        else
                        {
                            if(flag == 0)
                            {
                                status += strlist[i];
                                status += " ";
                            }
                            else if(flag == 1)
                            {
                                baterry = stof(strlist[i].toStdString());
                            }
                            else if(flag == 2)
                            {
                                if(counter == 0)
                                {
                                    x = stoi(strlist[i].toStdString());
                                }
                                else if(counter == 1)
                                {
                                    y = stoi(strlist[i].toStdString());
                                }
                                else if(counter == 2)
                                {
                                    heading = stoi(strlist[i].toStdString());
                                }
                                counter++;
                            }
                            else if(flag == 3)
                            {
                                temperature = stof(strlist[i].toStdString());
                            }
                        }
                    }
                }
            }
        }

    }
}

bool NetworkThread::RBServerOpen(QHostAddress::SpecialAddress _host, quint16 _port)
{
    RBHostAddress = _host;
    RBPortNumber = _port;

    RBTcpServer = new QTcpServer(this);
    connect(RBTcpServer, SIGNAL(newConnection()), this, SLOT(RBNewConnection()));

    return(RBTcpServer->listen(RBHostAddress, RBPortNumber));

}

void NetworkThread::RBNewConnection()
{
    if(RBConnectionState == RBLAN_CS_DISCONNECTED)
    {
        RBTcpClient = RBTcpServer->nextPendingConnection();
        connect(RBTcpClient, SIGNAL(disconnected()), this, SLOT(RBClientDisconnected()));
        connect(RBTcpClient, SIGNAL(readyRead()), this, SLOT(RBReadData()));
        RBConnectionState = RBLAN_CS_CONNECTED;
        cout << "New client is connected" << endl;;
    }
    else
    {
        QTcpSocket *dummySocket = RBTcpServer->nextPendingConnection();
        dummySocket->close();
        cout << "Client is already connected" << endl;
    }
}

void NetworkThread::RBClientDisconnected()
{
    RBTcpClient->deleteLater();
    RBConnectionState = RBLAN_CS_DISCONNECTED;
    cout <<  "Client is disconnected" << endl;
}

void NetworkThread::RBSendData(QByteArray &data)
{
    RBTcpClient->write(data);
}

void NetworkThread::RBReadData()
{
    int size = RBTcpClient->bytesAvailable();

    if(size > 0)
    {
        RBData = RBTcpClient->readAll();
        dataReceived.push_back(RBData);
    }
}



