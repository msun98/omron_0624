#include <iostream>
#include <string.h>

//#include "RBCommon.h"
#include "RBCAN.h"
//#include "CommonHeader.h"
#include "RBMotorController.h"
//#include "RBMobileMotion.h"


extern RBMotorController    _DEV_MC[MAX_MC];
extern int voltageFromCAN;

extern RBLANComm   *lanHandler;

int can_timeout_err[10];

using namespace std;

//int CAN_COUNT_ERR_index = 0;
RBCAN::RBCAN(){
    canTotalIndex = 0;
    canHeadIndex = 0;
    canTailIndex = 0;
}


int RBCAN::RBCAN_WriteData(RBCAN_MB &mb){
    //    canTotalMB[canTotalIndex] = mb;
    //    canTotalIndex = canTotalIndex+1;

    canTotalMB[canHeadIndex] = mb;
    canHeadIndex = (canHeadIndex+1)%RBCAN_MAX_NUM;

    return mb.dlc;
}

void RBCAN::RBCAN_ClearBuffer(){
    canTotalIndex = 0;
}


#define HAND_SHAKE_LAN_CNT 100

void RBCAN::RBCAN_SendData(){

    static int cnt = HAND_SHAKE_LAN_CNT;
    static unsigned char send_byte[1024];

    if(lanHandler->ConnectionStatus == true)
    {
        cnt--;

        // Send CAN Data ---------------------
        //if(canTotalIndex != 0){
        if(canHeadIndex != canTailIndex)
        {

            cnt = HAND_SHAKE_LAN_CNT;
            canTotalIndex = canHeadIndex - canTailIndex;
            if(canTotalIndex < 0)
            {
                canTotalIndex += RBCAN_MAX_NUM;
            }

            if(canTotalIndex > 20)
            {
                printf("canTotalIndex > 20 : %d\n", canTotalIndex);
            }

            send_byte[0] = 0x24;            // HEADER
            unsigned int TotalDataLength = 4 + 2 + 12 * canTotalIndex;


            send_byte[1] = (TotalDataLength)&0xFF;       // Total data length
            send_byte[2] = (TotalDataLength>>8)&0xFF;    // Total data length
            send_byte[3] = 0;               // From (Master : 0, Slave(CAN) : 1, General interface board : 2)
            send_byte[4] = 1;               // To (Master : 0, Slave(CAN) : 1, General interface board : 2)
            send_byte[5] = 0;               // Data type (CAN Data : 0)
            send_byte[6] = (canTotalIndex)&0xFF;
            send_byte[7] = (canTotalIndex>>8)&0xFF;


            for(int idx = 0; idx < canTotalIndex; idx++)
            {
                // data
                int temp_idx = canTailIndex;

                send_byte[8 + (12 * idx)] = canTotalMB[temp_idx].channel;// Channel
                unsigned int _CANID = canTotalMB[temp_idx].id;


                send_byte[9 + (12 * idx)] = (_CANID)&0xFF;     // ID
                send_byte[10 + (12 * idx)]= (_CANID>>8)&0xFF;   // ID
                send_byte[11 + (12 * idx)]= canTotalMB[temp_idx].dlc;               // Data Length Code
                for(int i=0; i<8; i++)
                {
                    send_byte[12+i + (12 * idx)] = canTotalMB[temp_idx].data[i];  // Data Fixed size(8bytes)
                }
                canTailIndex = (canTailIndex+1)%RBCAN_MAX_NUM;

            }

            send_byte[TotalDataLength+3-1] = 0x25;           // FOOTER

            int now_tx_size = TotalDataLength + 3;



            //RBLANComm::RBLANComm_WriteData(send_byte, now_tx_size);
            lanHandler->RBLANComm_WriteData(send_byte, now_tx_size);
//            cout << "RBCAN::Send" << endl;
        }
        else
        {
            //if(canTotalIndex == 0 && cnt <=0)
            if(cnt <= 0)
            {
                send_byte[0] = 0x24;           // HEADER
                send_byte[1] = 0x01;           // FOOTER
                send_byte[2] = 0x00;           // FOOTER
                send_byte[3] = 0x25;           // FOOTER
                //RBLANComm::RBLANComm_WriteData(send_byte, 4);
                 lanHandler->RBLANComm_WriteData(send_byte, 4);

                //printf("RBLANComm: dummy %d %d %d %d \n",send_byte[0] ,send_byte[1] ,send_byte[2] ,send_byte[3] );
                cnt =HAND_SHAKE_LAN_CNT;

            }
        }
    }
    else
    {
        //canTotalIndex = 0;
        canHeadIndex = canTailIndex = 0;
    }
}


void RBCAN::RBCAN_MessageDecode(int id, unsigned char *data, int dlc)
{
//    RB_UNUSED(dlc);

//   cout << "message decode: " << std::hex << id << endl;
//    cout << "message decode: " << id << endl;

    if(id == 0x355)
    {
     //  voltageFromCAN = data[0];
    }
    else if(id == 0x61) //encoder
    {
        int mc = id-0x61;

        int temp_enc = (int)((data[0]) | (data[1]<<8) | (data[2]<<16) | (data[3]<<24));

        //cout << "encoder : " << temp_enc << endl;
        _DEV_MC[mc].Joints[0].EncoderValue = temp_enc;
        _DEV_MC[mc].Joints[0].CurrentPosition = (double)temp_enc/_DEV_MC[mc].Joints[0].PPR;
    }
    else if( id == 0x151) //status
    {
        int mc = id-0x151;
        //cout << "status : " << mc << endl;
        _DEV_MC[mc].Joints[0].CurrentStatus.B[0] = data[0];
        _DEV_MC[mc].Joints[0].CurrentStatus.B[1] = data[1];
        _DEV_MC[mc].Joints[0].CurrentStatus.B[2] = data[2];
    }
    else if(id == 0x191) //ID
    {
        int mc = id-0x191;

//        cout << "id : " << mc << endl;
        // can check return
        _DEV_MC[mc].ConnectionStatus = true;
        _DEV_MC[mc].Joints[0].CurrentStatus.b.CAN = _DEV_MC[mc].ConnectionStatus;

    }


}



