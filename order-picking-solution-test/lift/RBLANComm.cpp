#include <iostream>
#include <string.h>

#include "RBLANComm.h"

#include "RBCAN.h"


extern RBCAN        *canHandler;

extern int     _IS_WORKING;


// define static variables
int RBLANComm::LAN_CLIENT_FD = 0;


using namespace std;

RBLANComm::RBLANComm()
{
    ConnectionStatus = false;
    threadID_connectionThread = pthread_create(&ConnectionThreadHandler, NULL, &RBLANComm_ConnectionThread, this);
    threadID_readThread = pthread_create(&readThreadHandler, NULL, &RBLANComm_ReadThread, this);
}

RBLANComm::~RBLANComm()
{
    ConnectionStatus = false;
    FILE_LOG(logSUCCESS) << "RBLANComm:: Shutdonw Network";
    shutdown(LAN_CLIENT_FD,2);
    //    close(LAN_CLIENT_FD);
}

// This thread only uses for connection management
void *RBLANComm::RBLANComm_ConnectionThread(void *arg)
{
    RBLANComm *lan = (RBLANComm *)arg;

    while(_IS_WORKING)
    {
        if(lan->ConnectionStatus == false)
        {
            // if client is not connected
            if(lan->LAN_CLIENT_FD == 0)
            {
                lan->CreateSocket("192.168.1.3", 1977); //192.168.0.77
                FILE_LOG(logSUCCESS) << "RBLANComm:: succeed to create socket";
            }
            else
            {
                if(lan->Connect2Server())
                {
                    lan->ConnectionStatus = true;
                    canHandler->RBCAN_ClearBuffer();
                    FILE_LOG(logSUCCESS) << "RBLANComm:: succeed to connect server";
                }
            }
        }
        usleep(1000*1000);
    }
}

void *RBLANComm::RBLANComm_ReadThread(void *arg)
{
    RBLANComm *lan = (RBLANComm*)arg;

    char totalLANData[RX_DATA_SIZE];
    char tempPacketData[RX_DATA_SIZE];
    int tcp_size = 0;

    FILE_LOG(logSUCCESS) << "RBLANComm:: RBLANComm_ReadThread";

    while(_IS_WORKING)
    {

        if(lan->ConnectionStatus == true)
        {
            tcp_size = recv(LAN_CLIENT_FD, totalLANData, RX_DATA_SIZE, 0);
            if(tcp_size < 0)
            {
                cout <<  strerror(errno) << endl;
            }

            if(tcp_size == 0)
            {
                lan->LanTCPClientClose();
                lan->LAN_CLIENT_FD = NULL;
                lan->ring_head = lan->ring_tail = 0;
                lan->ConnectionStatus = false;
                FILE_LOG(logERROR) << "RBLANComm:: client is disconnected";
            }
            else if(tcp_size > 0)
            {
                //cout << tcp_size << endl;
                int start_pos = 0;
                int end_pos = 0;

                while(_IS_WORKING)
                {
                    if(start_pos >= tcp_size)
                    {
                        break;
                    }
                    if(totalLANData[start_pos] == 0x24)
                    {
                        if(tcp_size-start_pos >= 4)
                        {
                            int packet_length = (short)(totalLANData[start_pos+1] | (totalLANData[start_pos+2]<<8));
                            if(packet_length < 0 || packet_length > RX_DATA_SIZE)
                            {
                                FILE_LOG(logERROR) << "RBLANComm:: packet length under zero";
                                start_pos++;
                            }
                            else
                            {
                                end_pos = start_pos + packet_length + 3 - 1;

                                if(tcp_size-start_pos >= packet_length + 3)
                                {
                                    memcpy(tempPacketData, &totalLANData[start_pos], packet_length+3);

                                    if(tempPacketData[0] == 0x24 && tempPacketData[packet_length + 3 - 1] == 0x25)
                                    {
                                        int data_type = tempPacketData[5];
                                        switch(data_type)
                                        {
                                        case 0x00:
                                        {
                                            // CAN Data
                                            int num = (short)(tempPacketData[6] | (tempPacketData[7]<<8));
                                            for(int j=0; j<num; j++)
                                            {
                                                unsigned char tempCANData[12];
                                                for(int k=0; k<12; k++)
                                                {
                                                    tempCANData[k] = tempPacketData[8 + 12*j + k];
                                                }
                                                int id = (short)((tempCANData[1]) | tempCANData[2]<<8);
                                                int dlc = tempCANData[3];
                                                unsigned char data[8];
                                                memcpy(data, &tempCANData[4], dlc);

                                                canHandler->RBCAN_MessageDecode(id, data, dlc);
                                            }
                                            break;
                                        }
                                        default:
                                            break;
                                        }
                                    }
                                    else
                                    {
                                        FILE_LOG(logERROR) << "RBLANComm:: header footer not match";
                                    }

                                }
                                else
                                {
                                    FILE_LOG(logERROR) << "RBLANComm:: size not match : " << tcp_size-start_pos-1 << ", " << end_pos;
                                    start_pos++;
                                }
                                start_pos = end_pos+1;
                            }
                        }
                    }
                    else
                    {
                        start_pos++;
                        if(start_pos >= tcp_size)
                        {
                            break;
                        }
                    }
                }

            }
            else
            {

            }
        }
        usleep(100);
    }
}

void RBLANComm::RBLANComm_WriteData(const void *buf, size_t len)
{
    write(LAN_CLIENT_FD, buf, len);
}


// Internal Functions -------------------------------------------------------------------------------------------------



int RBLANComm::CreateSocket(const char *addr, int port)
{
    LAN_CLIENT_FD = socket(AF_INET, SOCK_STREAM, 0);
    if(LAN_CLIENT_FD == -1)
    {
        return false;
    }
    ClientAddr.sin_addr.s_addr = inet_addr(addr);
    ClientAddr.sin_family = AF_INET;
    ClientAddr.sin_port = htons(port);

    int optval = 1;
    if(setsockopt(LAN_CLIENT_FD, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval)) == -1)
    {
        return false;
    }
    if(setsockopt(LAN_CLIENT_FD, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
    {
        return false;
    }
    return true;
}
int RBLANComm::Connect2Server()
{
    if(connect(LAN_CLIENT_FD, (struct sockaddr*)&ClientAddr, sizeof(ClientAddr)) < 0)
    {
        return false;
    }

    return true;
}
int RBLANComm::LanTCPClientClose()
{
    return close(LAN_CLIENT_FD);
}
