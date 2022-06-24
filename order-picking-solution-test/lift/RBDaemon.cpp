#include "RBDaemon.h"


pRBCORE_SHM sharedData;
#ifdef WITH_XENO
RT_TASK     rtTaskCon;
#else
ulong   rtTaskCon;
#endif
RBCAN      *canHandler;
RBLANComm   *lanHandler;

RBMotorController  _DEV_MC[MAX_MC];

int     _VERSION;
int     _NO_OF_MC;
int     _IS_WORKING = false;
int     _IS_CAN_OK = false;



int _ENCODER_ENABLED = false;
int _SENSOR_ENABLED = false;

void THREAD_ReadHomeError();
long _ThreadCnt = 0;
bool StatusReadFlag[MAX_MC] = {0,};
bool ErrorClearStart = false;


using namespace std;

//--------------------------------------------------------------





// ---------------------------------------------------------



// Initialize Functions -------------------------------
void RBCore_SMInitialize(){
    sharedData = (pRBCORE_SHM)malloc(sizeof(RBCORE_SHM));
}

int RBCore_DBInitialize(){
    _NO_OF_MC = 1;

    _DEV_MC[0].RBBoard_SetCANIDs(1,1);



    // 16384*4/16 pulse per rotation --> 4096
    // 10mm pitch
    // gear ratio 10:1
    // 4096*10 --> 10mm
    _DEV_MC[0].Joints[0].PPR = 2048;    //2560;//2048;     // pulse for 1mm

    _DEV_MC[0].MoveJoints[0].STP_MaxAcc = 30.0;

    _DEV_MC[0].MoveJoints[0].STP_Speed = 20.0;

    return true;
}

int RBCore_CANInitialize(){

    canHandler = new RBCAN();
    _DEV_MC[0].RBMC_AddCANMailBox();
    //    canHandler = new RBCAN(1);

    //    if(canHandler->IsWorking() == false){
    //        _IS_CAN_OK = false;
    //        return false;
    //    }else{
    //        for(int i=0; i<_NO_OF_MC; i++)
    //            _DEV_MC[i].RBMC_AddCANMailBox();
    //        _IS_CAN_OK = true;
    //        return true;
    //    }
}

int RBCore_LANInitialize()
{
    lanHandler = new RBLANComm();
    return true;
    //    if(canHandler->IsWorking() == false){
    //        _IS_CAN_OK = false;
    //        return false;
    //    }else{
    //        for(int i=0; i<_NO_OF_MC; i++)
    //            _DEV_MC[i].RBMC_AddCANMailBox();
    //        _IS_CAN_OK = true;
    //        return true;
    //    }
}

int RBCore_ThreadInitialize(){

    int threadID = pthread_create(&rtTaskCon, NULL, &RBCore_RTThreadCon, NULL);
    if(threadID < 0){
        FILE_LOG(logERROR) << "Fail to create core real-time thread";
        return false;
    }

    return true;
}



inline double diffTimeMs(struct timespec fromT, struct timespec toT){
    return (toT.tv_sec-fromT.tv_sec)*1000.0 + (toT.tv_nsec-fromT.tv_nsec)/1000000;
}

#ifdef WITH_XENO
void RBCore_RTThreadCon(void *)
#else
void  *RBCore_RTThreadCon(void *)
#endif
{
#ifdef WITH_XENO
    rt_task_set_periodic(NULL, TM_NOW, (5)*1000000);

    pthread_t hThread;
    int CPU_NUM_RBDaemon_Network_Thread;
    CPU_NUM_RBDaemon_Network_Thread = 1;
    pthread_create_with_affinity(RBDaemon_Network, CPU_NUM_RBDaemon_Network_Thread, "RBDaemonNetwork", hThread, NULL);
#else
    struct timespec th_start, th_stop;
    struct timespec con_check_time, con_cur_time;
#endif

    while(_IS_WORKING){
#ifdef WITH_XENO
        rt_task_wait_period(NULL);
#else
        clock_gettime(CLOCK_REALTIME, &con_check_time);
#endif


//        lanHandler->RBLANComm_ReadData();
        canHandler->RBCAN_SendData();
        //THREAD_ReadHomeError();
        // Read Sensor & Encoder =====================================
        //        if(canHandler->IsWorking()){
        //            //            THREAD_ReadSensor();
        //            //            THREAD_ReadEncoder();
        //            //            THREAD_ReadTemperature();

        //        }
        // ===========================================================


       // cout << "Hi" << endl;


        // Write CAN Reference =======================================
        for(int i=0; i<_NO_OF_MC; i++)
        {
            for(int j=0; j<_DEV_MC[i].MOTOR_CHANNEL; j++)
            {
                _DEV_MC[i].RBJoint_MoveJoint(j);
                sharedData->JointReference[i][j] = _DEV_MC[i].MoveJoints[j].RefAngleCurrent;

                _DEV_MC[i].Joints[j].Reference = sharedData->JointReference[i][j];
                sharedData->ENCODER[i][j].CurrentReference = sharedData->JointReference[i][j];
            }
            _DEV_MC[i].RBBoard_SendReference();
//            cout << "send ref" << endl;
        }
        // ===========================================================


        _ThreadCnt++;
        // Request Sensor & Encoder ==================================
        //        if(canHandler->IsWorking()){
        //            //            THREAD_RequestSensor();
        //            //            THREAD_RequestEncoder();
        //            //            THREAD_RequestTemperature();
        //            // home & error
        //            int mc = _ThreadCnt % _NO_OF_MC;
        //            if(_DEV_MC[mc].ConnectionStatus == true){
        //                StatusReadFlag[mc] = false;
        //                _DEV_MC[mc].RBBoard_RequestStatus();
        //            }
        //        }
        if(_DEV_MC[0].ConnectionStatus == true)
        {
            StatusReadFlag[0] = false;
            _DEV_MC[0].RBBoard_RequestStatus();
        }

        // ===========================================================
#ifdef WITH_XENO
        rt_task_wait_period(NULL);
#else
       clock_gettime(CLOCK_REALTIME, &th_start);
#endif

#ifndef WITH_XENO
        while(1){
            clock_gettime(CLOCK_REALTIME, &con_cur_time);
            if(RT_TIMER_PERIOD_MS-0.05 < diffTimeMs(con_check_time, con_cur_time)){//(double)(con_cur_time.tv_nsec - con_check_time.tv_nsec) / (double)1000000.0){
                break;
            }
            usleep(10);
        }
#endif
    }
}



void THREAD_ReadHomeError(){
    //    static unsigned int StatusErrorCnt[MAX_MC] = {0,};

    //    int mc = _ThreadCnt%_NO_OF_MC;

    //    if(_DEV_MC[mc].ConnectionStatus == true){
    //        if(_DEV_MC[mc].RBBoard_GetStatus() == true){
    //            StatusErrorCnt[mc] = 0;
    //            StatusReadFlag[mc] = true;

    //            for(int j=0; j<_DEV_MC[mc].MOTOR_CHANNEL; j++){
    //                sharedData->MCStatus[mc][j] = _DEV_MC[mc].CurrentStatus;
    //                sharedData->MCStatus[mc][j].b.CUR = 0;
    //                if(sharedData->MCStatus[mc][j].b.BIG == 1 || sharedData->MCStatus[mc][j].b.INP == 1 ||
    //                        sharedData->MCStatus[mc][j].b.ENC == 1 || sharedData->MCStatus[mc][j].b.JAM == 1 )
    //                {
    //                    if(ErrorClearStart == false){
    //                    }
    //                }
    //            }
    //        }else if(StatusReadFlag[mc] == false){
    //            StatusErrorCnt[mc]++;
    //            if(StatusErrorCnt[mc] > 3){
    //                if(ErrorClearStart == false){
    //                    for(int j=0; j<_DEV_MC[mc].MOTOR_CHANNEL; j++){
    //                        sharedData->MCStatus[mc][j].b.CUR = 1;
    //                    }
    //                }
    //            }

    //            if(StatusErrorCnt[mc]%3 == 0){
    //                cout << "Status Get Error from Board " << mc << " Cnt : " << StatusErrorCnt[mc] << endl;
    //            }
    //        }
    //    }
}
