#include "RBCAN.h"
#include "RBLANComm.h"
#include "RBMotorController.h"

extern RBLANComm   *lanHandler;

extern RBMotorController   _DEV_MC[MAX_MC];


using namespace std;
// --------------------------------------------------------------------------------------------- //
void RBCAN::RBCAN_ReadThread(void *_arg)
{
//    RBCAN *rbCAN = (RBCAN *)_arg;
//    TPCANRdMsg m;
//    int index;

//    while(rbCAN->isWorking == true){
//        if(rbCAN->isSuspend == true){
//            usleep(10);
//            continue;
//        }
//        for(int i=0; i<rbCAN->chNum; i++){
//            while(LINUX_CAN_Read_Timeout(rbCAN->canHandler[i], &m, 0) == 0){

//                rbCAN->RBCAN_MessageDecode(m.Msg.ID, m.Msg.DATA, m.Msg.LEN);

////                index = rbCAN->RBCAN_GetMailBoxIndex(m.Msg.ID);

////                if(index < rbCAN->canMBCounter){
////                    rbCAN->canReadMB[index].id = m.Msg.ID;
////                    rbCAN->canReadMB[index].dlc = m.Msg.LEN;
////                    memcpy(rbCAN->canReadMB[index].data, m.Msg.DATA, m.Msg.LEN);
////                    rbCAN->canReadMB[index].channel = i;
////                    if(rbCAN->canReadMB[index].status == RBCAN_NEWDATA)
////                        rbCAN->canReadMB[index].status = RBCAN_OVERWRITE;
////                    else rbCAN->canReadMB[index].status = RBCAN_NEWDATA;
////                }
//            }
//        }
//        usleep(10);
//    }
}

float _pre_mag = 0;
float _raw_mag;
float _mag_alpha = 0.5;//0.95;
int _mag_null = 0;


void RBCAN::RBCAN_MessageDecode(int id, unsigned char data[], int dlc)
{


//    if(id == _DEV_MC[0].ID_RCV_STAT){
//       // FILE_LOG(logWARNING) << "RCV_STAT 0";
//        _DEV_MC[0].Joints[0].CurrentStatus.B[0] = data[0];
//        _DEV_MC[0].Joints[0].CurrentStatus.B[1] = data[1];
//        _DEV_MC[0].Joints[0].CurrentStatus.B[2] = data[2];
//    }


//    if(id == _DEV_MC[1].ID_RCV_STAT){
//      //  FILE_LOG(logWARNING) << "RCV_STAT 1";
//        _DEV_MC[1].Joints[0].CurrentStatus.B[0] = data[0];
//        _DEV_MC[1].Joints[0].CurrentStatus.B[1] = data[1];
//        _DEV_MC[1].Joints[0].CurrentStatus.B[2] = data[2];
//    }



//    int index = RBCAN_GetMailBoxIndex(id);
////    if(id == _DEV_MC[0].ID_RCV_ENC){
////        FILE_LOG(logERROR) << "ENCENC " << index;
////    }
//    if(index < canMBCounter){
//        canReadMB[index].id = id;
//        canReadMB[index].dlc = dlc;
//        memcpy(canReadMB[index].data, data, dlc);
//        canReadMB[index].channel = 0;
//        if(canReadMB[index].status == RBCAN_NEWDATA)
//            canReadMB[index].status = RBCAN_OVERWRITE;
//        else canReadMB[index].status = RBCAN_NEWDATA;
//    }

//    cout << id << endl;


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

// --------------------------------------------------------------------------------------------- //
void *RBCAN::RBCAN_WriteThread(void *_arg)
{
//    RBCAN *rbCAN = (RBCAN *)_arg;
//    TPCANMsg m;
//    int index = 0;

//    while(rbCAN->isWorking){
//        if(rbCAN->isSuspend == true){
//            usleep(3);
//            continue;
//        }
//        if(rbCAN->canSendSuspend == true){
//            if(rbCAN->canHeadIndex != rbCAN->canTailIndex){
//                index = rbCAN->canTailIndex;
//                m.ID = rbCAN->canWriteMB[index].id;
//                m.LEN = rbCAN->canWriteMB[index].dlc;
//                memcpy(m.DATA, rbCAN->canWriteMB[index].data, rbCAN->canWriteMB[index].dlc);

//                LINUX_CAN_Write_Timeout(rbCAN->canHandler[rbCAN->canWriteMB[index].channel], &m, 0);
//                rbCAN->canTailIndex = (rbCAN->canTailIndex + 1) % RBCAN_MAX_MB;
//            }
//        }
//        usleep(5);
//    }
//    return NULL;
}
// --------------------------------------------------------------------------------------------- //


// --------------------------------------------------------------------------------------------- //
RBCAN::RBCAN(int _ChNum)
{
    int okflag;
#ifndef USE_LAN2CAN
    if(_ChNum > RBCAN_MAX_CAN_CHANNEL){
        FILE_LOG(logERROR) << "Over the maximum CAN channel [" << RBCAN_MAX_CAN_CHANNEL << " ]";
        chNum = 0;
        return;
    }else{
        chNum = _ChNum;
    }

    int oknum = 0;
    okflag = false;
    for(int i=0; i<MAX_SEARCH_CHANNEL; i++){
        char filePath[30];
        sprintf(filePath, "/dev/pcanusb%d", i);
        canHandler[oknum] = NULL;
        canHandler[oknum] = LINUX_CAN_Open(filePath, O_RDWR);

        if(canHandler[oknum] != NULL){
            if(CAN_Init(canHandler[oknum], CAN_BAUD_1M, CAN_INIT_TYPE_ST) !=  CAN_ERR_OK){
                FILE_LOG(logWARNING) << "Fail to setting CAN device (" << i << ")";
            }else{
                oknum++;
                if(oknum >= chNum){
                    okflag = true;
                    break;
                }
            }
        }else{
            FILE_LOG(logWARNING) << "Fail to open CAN device (" << i << ")";
        }
    }
#else
    okflag = true;
#endif

    if(okflag == true)
    {
        isWorking = true;
        canSendSuspend = true;
        isSuspend = false;
        canMBCounter = 0;
        canHeadIndex = 0;
        canTailIndex = 0;

        if(RBCAN_StartThread() == false)
        {
            isWorking = false;
            FILE_LOG(logERROR) << "CAN hardware initialize = FAIL";
        }
        else
        {
            FILE_LOG(logSUCCESS) << "CAN hardware initialize = OK";
        }
    }
    else
    {
        isWorking = false;

        canMBCounter = 0;
        canHeadIndex = 0;
        canTailIndex = 0;
        FILE_LOG(logERROR) << "CAN hardware initialize = FAIL";
    }
}
// --------------------------------------------------------------------------------------------- //
RBCAN::~RBCAN()
{
    isWorking = false;
    usleep(100*1000);

//    for(int i=0; i<chNum; i++)
//        CAN_Close(canHandler[i]);
}
// --------------------------------------------------------------------------------------------- //
void RBCAN::Finish()
{
    isWorking = false;
    usleep(100*1000);

//    for(int i=0; i<chNum; i++)
//        CAN_Close(canHandler[i]);
}


void RBCAN::RBCAN_SendData()
{
    if(lanHandler->ConnectionStatus == true){

        // Send CAN Data ---------------------
        if(canTotalIndex != 0){
            unsigned char send_byte[512];
            send_byte[0] = 0x24;            // HEADER
            unsigned int TotalDataLength = 4 + 2 + 12 * canTotalIndex;
            send_byte[1] = (TotalDataLength)&0xFF;       // Total data length
            send_byte[2] = (TotalDataLength>>8)&0xFF;    // Total data length
            send_byte[3] = 0;               // From (Master : 0, Slave(CAN) : 1, General interface board : 2)
            send_byte[4] = 1;               // To (Master : 0, Slave(CAN) : 1, General interface board : 2)
            send_byte[5] = 0;               // Data type (CAN Data : 0)
            send_byte[6] = (canTotalIndex)&0xFF;
            send_byte[7] = (canTotalIndex>>8)&0xFF;

            for(int idx = 0; idx < canTotalIndex; idx++){
                // data
                send_byte[8 + (12 * idx)] = canTotalMB[idx].channel;// Channel
                unsigned int _CANID = canTotalMB[idx].id;

                send_byte[9 + (12 * idx)] = (_CANID)&0xFF;     // ID
                send_byte[10 + (12 * idx)]= (_CANID>>8)&0xFF;   // ID
                send_byte[11 + (12 * idx)]= canTotalMB[idx].dlc;               // Data Length Code
                for(int i=0; i<8; i++){
                    send_byte[12+i + (12 * idx)] = canTotalMB[idx].data[i];  // Data Fixed size(8bytes)
                }
            }

            send_byte[TotalDataLength+3-1] = 0x25;           // FOOTER
            RBLANComm::RBLANComm_WriteData(send_byte, (TotalDataLength+3));

           // cout << " canTotalIndex :  " << canTotalIndex<< endl;

            canTotalIndex = 0;

        }
    }
}

// --------------------------------------------------------------------------------------------- //
void RBCAN::RBResetCAN()
{

//    isSuspend = true;
//    usleep(10);
//    for(int i=0; i<chNum; i++)
//        CAN_Close(canHandler[i]);

//    int oknum = 0;
//    for(int i=0; i<MAX_SEARCH_CHANNEL; i++){
//        char filePath[30];
//        sprintf(filePath, "/dev/pcanusb%d", i);
//        canHandler[oknum] = NULL;
//        canHandler[oknum] = LINUX_CAN_Open(filePath, O_RDWR);

//        if(canHandler[oknum] != NULL){
//            if(CAN_Init(canHandler[oknum], CAN_BAUD_1M, CAN_INIT_TYPE_ST) !=  CAN_ERR_OK){
//                FILE_LOG(logWARNING) << "Fail to setting CAN device (" << i << ")";
//            }else{
//                oknum++;
//                if(oknum >= chNum){
//                    break;
//                }
//            }
//        }else{
//            FILE_LOG(logWARNING) << "Fail to open CAN device (" << i << ")";
//        }
//    }

//    isSuspend = false;
}

// --------------------------------------------------------------------------------------------- //
bool RBCAN::IsWorking()
{
    return isWorking;
}
// --------------------------------------------------------------------------------------------- //
int RBCAN::RBCAN_StartThread(void)
{
#ifndef USE_LAN2CAN
    if(rt_task_create(&canReadThreadHandler, "RBCAN_READ_TASK", 0, 95, 0) == 0){
        cpu_set_t aCPU;
        CPU_ZERO(&aCPU);
        CPU_SET(1, &aCPU);
        if(rt_task_set_affinity(&canReadThreadHandler, &aCPU) != 0){
            FILE_LOG(logWARNING) << "RBCAN: Read thread set affinity CPU failed..";
        }
        if(rt_task_start(&canReadThreadHandler, &RBCAN_ReadThread, this) == 0){

        }else{
            FILE_LOG(logERROR) << "RBCAN: Read thread Creation Error";
            return false;
        }
    }else{
        FILE_LOG(logERROR) << "RBCAN: Read thread Creation Error";
        return false;
    }

    int threadID = pthread_create(&canWriteThreadHandler, NULL, &RBCAN_WriteThread, this);
    if(threadID < 0){
        FILE_LOG(logERROR) << "RBCAN: Write thread Creation Error";
        return false;
    }
#endif
    return true;
}
// --------------------------------------------------------------------------------------------- //
void RBCAN::RBCAN_ClearBuffer()
{
    canTotalIndex = 0;
}

int RBCAN::RBCAN_WriteData(RBCAN_MB _mb)
{

    canTotalMB[canTotalIndex] = _mb;
    canTotalIndex = canTotalIndex+1;


//    canTotalMB[canHeadIndex] = _mb;
//    canHeadIndex = (canHeadIndex+1)%RBCAN_MAX_NUM;

    return true;
}


// --------------------------------------------------------------------------------------------- //
int RBCAN::RBCAN_WriteDataDirectly(RBCAN_MB _mb)
{
    return RBCAN_WriteData(_mb);
//    TPCANMsg m;
//    m.ID = _mb.id;
//    m.LEN = _mb.dlc;
//    m.MSGTYPE = MSGTYPE_STANDARD;
//    memcpy(m.DATA, _mb.data, m.LEN);

//    if(LINUX_CAN_Write_Timeout(canHandler[_mb.channel], &m, 0)){
//        return false;
//    }
//    return true;
}
// --------------------------------------------------------------------------------------------- //
int RBCAN::RBCAN_ReadData(pRBCAN_MB _mb)
{
    unsigned int index = RBCAN_GetMailBoxIndex(_mb->id);

    _mb->dlc = canReadMB[index].dlc;
    memcpy(_mb->data, canReadMB[index].data, canReadMB[index].dlc);
    _mb->status = canReadMB[index].status;

    canReadMB[index].status = RBCAN_NODATA;
    return true;
}
// --------------------------------------------------------------------------------------------- //
int RBCAN::RBCAN_AddMailBox(unsigned int _id)
{
    unsigned char i;

    if(canMBCounter >= RBCAN_MAX_MB){
        FILE_LOG(logWARNING) << "Over the CAN mail box";
        return false;
    }else{
        if(RBCAN_GetMailBoxIndex(_id) == canMBCounter){
            canReadMB[canMBCounter].id = _id;                           // CAN id assign
            canReadMB[canMBCounter].dlc = 0x00;                         // Data Length Code
            for(i=0; i<8; i++) canReadMB[canMBCounter].data[i] = 0x00;	// Data init.
            canReadMB[canMBCounter].status = RBCAN_NODATA;          // Initial MB status = No Data
            canMBCounter++;
            return true;
        }else{
            if(_id != 0x00){
//                FILE_LOG(logWARNING) << "CAN ID(" << _id << ") is already assigned";
            }
            return false;
        }
    }
}
// --------------------------------------------------------------------------------------------- //
int RBCAN::RBCAN_GetMailBoxIndex(unsigned int _id)
{
    for(int i=0 ; i<canMBCounter; i++) if(canReadMB[i].id == _id) return i;
    return canMBCounter;
}
// --------------------------------------------------------------------------------------------- //
int RBCAN::RBCAN_WriteEnable(int _suspend)
{
    canSendSuspend = _suspend;
    return canSendSuspend;
}
// --------------------------------------------------------------------------------------------- //
