#ifndef RBLANCOMM_H
#define RBLANCOMM_H

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>

#include <lift/RBDaemon.h>

#define RX_DATA_SIZE    1000

extern int IS_NEW_CAN_DATA;

class RBLANComm
{
public:
    RBLANComm();
    ~RBLANComm();


    int     ConnectionStatus;

    static void     RBLANComm_WriteData(const void *buf, size_t len);
    void            RBLANComm_ReadData();
    void            RBLANComm_CreateThread();


private:
    int             threadID_connectionThread;

    unsigned long   ConnectionThreadHandler;


    struct sockaddr_in  ClientAddr;
//    static int          LAN_CLIENT_FD;


    int CreateSocket(const char *addr, int port);
    int Connect2Server();
    int LanTCPClientClose();

    static void *RBLANComm_ConnectionThread(void *arg);

};

#endif // RBLANCOMM_H
