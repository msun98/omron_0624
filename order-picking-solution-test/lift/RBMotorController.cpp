#include "RBMotorController.h"

RBMotorController::RBMotorController()
{
    for(int i=0; i<MAX_JOINT; i++){
        MoveJoints[i].MoveFlag = false;
        MoveJoints[i].STP_MaxAcc = 100.0;
    }
}

void RBMotorController::RBJoint_SetMoveJoint(int ch, float angle, float timeMs, int mode){
    if(MoveJoints[ch].MoveFlag == true){
        FILE_LOG(logWARNING) << "It's working now..[BNO: " << BOARD_ID << "]";
        return;
    }
    MoveJoints[ch].Profile = 0;
    MoveJoints[ch].RefAngleInitial = MoveJoints[ch].RefAngleCurrent;
    if(mode == 0){  // abs
        MoveJoints[ch].RefAngleToGo = angle;
        MoveJoints[ch].RefAngleDelta = MoveJoints[ch].RefAngleToGo - MoveJoints[ch].RefAngleInitial;
    }else{          // rel
        MoveJoints[ch].RefAngleToGo = MoveJoints[ch].RefAngleInitial + angle;
        MoveJoints[ch].RefAngleDelta = angle;
    }

    MoveJoints[ch].GoalTimeCount = (ulong)(timeMs/(double)RT_TIMER_PERIOD_MS);
    MoveJoints[ch].CurrentTimeCount = 0;
    MoveJoints[ch].MoveFlag = true;
}
void RBMotorController::RBJoint_SetMoveJointSTrapi(int ch, float angle, float speed, int mode){
    if(MoveJoints[ch].MoveFlag == true){
        FILE_LOG(logWARNING) << "It's working now..[BNO: " << BOARD_ID << "]";
        return;
    }
    MoveJoints[ch].Profile = 1;
    MoveJoints[ch].RefAngleInitial = MoveJoints[ch].RefAngleCurrent;
    if(mode == 0){  // abs
        MoveJoints[ch].RefAngleToGo = angle;
        MoveJoints[ch].RefAngleDelta = MoveJoints[ch].RefAngleToGo - MoveJoints[ch].RefAngleInitial;
    }else{          // rel
        MoveJoints[ch].RefAngleToGo = MoveJoints[ch].RefAngleInitial + angle;
        MoveJoints[ch].RefAngleDelta = angle;
    }


    float acctime;
    float goaltime;
    MoveJoints[ch].STP_Speed = fabs(speed);
    if(MoveJoints[ch].STP_Speed * MoveJoints[ch].STP_Speed / MoveJoints[ch].STP_MaxAcc >= fabs(MoveJoints[ch].RefAngleDelta)){
        MoveJoints[ch].STP_Type = 1;
        MoveJoints[ch].STP_acctime = sqrt(fabs(MoveJoints[ch].RefAngleDelta)/MoveJoints[ch].STP_MaxAcc);
        MoveJoints[ch].STP_goaltime = MoveJoints[ch].STP_acctime * 2;
        if(MoveJoints[ch].STP_acctime < 0.005){
            MoveJoints[ch].STP_AccTimeCnt = 1;
            MoveJoints[ch].GoalTimeCount = 2;
        }else{
            MoveJoints[ch].STP_AccTimeCnt = (uint)(1000.0 * MoveJoints[ch].STP_acctime /(double)RT_TIMER_PERIOD_MS);
            MoveJoints[ch].GoalTimeCount = (uint)(1000.0 * MoveJoints[ch].STP_goaltime /(double)RT_TIMER_PERIOD_MS);
        }
        acctime = MoveJoints[ch].STP_AccTimeCnt*(double)RT_TIMER_PERIOD_MS / 1000.0;
        MoveJoints[ch].STP_acc = fabs(MoveJoints[ch].RefAngleDelta) / (acctime*acctime);
    }else{
        MoveJoints[ch].STP_Type = 0;
        MoveJoints[ch].STP_acctime = MoveJoints[ch].STP_Speed / MoveJoints[ch].STP_MaxAcc;
        MoveJoints[ch].STP_goaltime = MoveJoints[ch].STP_acctime * 2 + (fabs(MoveJoints[ch].RefAngleDelta) - MoveJoints[ch].STP_Speed*MoveJoints[ch].STP_acctime) / MoveJoints[ch].STP_Speed;
        MoveJoints[ch].STP_AccTimeCnt = (uint)(1000.0 * MoveJoints[ch].STP_acctime /(double)RT_TIMER_PERIOD_MS);
        MoveJoints[ch].GoalTimeCount = (uint)(1000.0 * MoveJoints[ch].STP_goaltime /(double)RT_TIMER_PERIOD_MS);
        acctime = MoveJoints[ch].STP_AccTimeCnt*(double)RT_TIMER_PERIOD_MS / 1000.0;
        goaltime = MoveJoints[ch].GoalTimeCount*(double)RT_TIMER_PERIOD_MS / 1000.0;
        if(acctime < 2){
            MoveJoints[ch].STP_acc = MoveJoints[ch].STP_MaxAcc;
        }else{
            MoveJoints[ch].STP_acc = fabs(MoveJoints[ch].RefAngleDelta) / (goaltime*acctime - acctime*acctime);
        }
        //FILE_LOG(logWARNING) << MoveJoints[ch].RefAngleDelta << ", " << goaltime << ", " << acctime;
    }
    MoveJoints[ch].CurrentTimeCount = 0;
    MoveJoints[ch].MoveFlag = true;


}

float moving_angle = 0.0;
void RBMotorController::RBJoint_MoveJoint(int ch){

    if(MoveJoints[ch].MoveFlag){
        MoveJoints[ch].CurrentTimeCount++;
        if(MoveJoints[ch].GoalTimeCount <= MoveJoints[ch].CurrentTimeCount){
            MoveJoints[ch].GoalTimeCount = MoveJoints[ch].CurrentTimeCount = 0;
            MoveJoints[ch].RefAngleCurrent = MoveJoints[ch].RefAngleToGo;
            MoveJoints[ch].MoveFlag = false;
        }else{
            if(MoveJoints[ch].Profile == 0){
                MoveJoints[ch].RefAngleCurrent = MoveJoints[ch].RefAngleInitial + MoveJoints[ch].RefAngleDelta*0.5*
                    (1.0f-cos(RBCORE_PI/(double)MoveJoints[ch].GoalTimeCount*(double)MoveJoints[ch].CurrentTimeCount));
            }else if(MoveJoints[ch].Profile == 1){
                moving_angle = 0.0;
                if(MoveJoints[ch].STP_Type == 0){
                    if(MoveJoints[ch].CurrentTimeCount < MoveJoints[ch].STP_AccTimeCnt){
                        float cur_t = MoveJoints[ch].CurrentTimeCount * (double)RT_TIMER_PERIOD_MS / 1000.0;
                        moving_angle = 0.5*MoveJoints[ch].STP_acc*cur_t*cur_t;
                    }else if(MoveJoints[ch].CurrentTimeCount < MoveJoints[ch].GoalTimeCount - MoveJoints[ch].STP_AccTimeCnt){
                        float a_t = MoveJoints[ch].STP_acctime;
                        float cur_t = MoveJoints[ch].CurrentTimeCount * (double)RT_TIMER_PERIOD_MS / 1000.0 - a_t;
                        moving_angle = 0.5*MoveJoints[ch].STP_acc*a_t*a_t + MoveJoints[ch].STP_acc*a_t*cur_t;
                    }else if(MoveJoints[ch].CurrentTimeCount < MoveJoints[ch].GoalTimeCount){
                        float a_t = MoveJoints[ch].STP_acctime;
                        float l_t = MoveJoints[ch].GoalTimeCount * (double)RT_TIMER_PERIOD_MS / 1000.0  - a_t*2;
                        float cur_t = MoveJoints[ch].CurrentTimeCount * (double)RT_TIMER_PERIOD_MS / 1000.0 - a_t - l_t;
                        moving_angle = 0.5*MoveJoints[ch].STP_acc*a_t*a_t + MoveJoints[ch].STP_acc*a_t*l_t + MoveJoints[ch].STP_acc*(a_t*cur_t - 0.5*cur_t*cur_t);
                    }
                }else if(MoveJoints[ch].STP_Type == 1){
                    if(MoveJoints[ch].CurrentTimeCount < MoveJoints[ch].STP_AccTimeCnt){
                        float cur_t = MoveJoints[ch].CurrentTimeCount * (double)RT_TIMER_PERIOD_MS / 1000.0;
                        moving_angle = 0.5*MoveJoints[ch].STP_acc*cur_t*cur_t;
                    }else if(MoveJoints[ch].CurrentTimeCount < MoveJoints[ch].GoalTimeCount){
                        float a_t = MoveJoints[ch].STP_acctime;
                        float cur_t = MoveJoints[ch].CurrentTimeCount * (double)RT_TIMER_PERIOD_MS / 1000.0 - a_t;
                        moving_angle = 0.5*MoveJoints[ch].STP_acc*a_t*a_t + MoveJoints[ch].STP_acc*(a_t*cur_t - 0.5*cur_t*cur_t);
                    }
                }

                if(MoveJoints[ch].RefAngleDelta >= 0){
                    MoveJoints[ch].RefAngleCurrent = MoveJoints[ch].RefAngleInitial + moving_angle;
                }else{
                    MoveJoints[ch].RefAngleCurrent = MoveJoints[ch].RefAngleInitial - moving_angle;
                }

            }
        }
    }
}

void RBMotorController::RBBoard_MoveJoint(){
    for(int i=0; i<MOTOR_CHANNEL; i++){
        RBJoint_MoveJoint(i);
    }
}

void RBMotorController::RBMC_AddCANMailBox(){
//    canHandler->RBCAN_AddMailBox(ID_RCV_ENC);
//    canHandler->RBCAN_AddMailBox(ID_RCV_INFO);
//    canHandler->RBCAN_AddMailBox(ID_RCV_PARA);
//    canHandler->RBCAN_AddMailBox(ID_RCV_STAT);
}

void RBMotorController::RBBoard_GetDBData(DB_MC db){
    BOARD_ID        = db.BOARD_ID;
    BOARD_NAME      = db.BOARD_NAME;
    BOARD_TYPE      = db.BOARD_TYPE;
    MOTOR_CHANNEL   = db.MOTOR_CHANNEL;
    CAN_CHANNEL     = db.CAN_CHANNEL;
    ID_SEND_REF     = db.ID_SEND_REF;
    ID_RCV_ENC      = db.ID_RCV_ENC;
    ID_RCV_STAT     = db.ID_RCV_STAT;
    ID_RCV_INFO     = db.ID_RCV_INFO;
    ID_RCV_PARA     = db.ID_RCV_PARA;
    ID_SEND_GENERAL = db.ID_SEND_GENERAL;
    TOTAL_CHANNEL   = BOARD_TYPE;

    for(int i=0; i<MOTOR_CHANNEL; i++){
        Joints[i].PPR = db.JOINTS[i].PPR;
    }
}

void RBMotorController::RBBoard_SetCANIDs(int bno, int ch){
    BOARD_ID = bno;
    BOARD_TYPE = ch;
    MOTOR_CHANNEL = ch;
    CAN_CHANNEL = 0;
    ID_SEND_REF = 0x10+bno;
    ID_RCV_ENC = 0x60+bno;
    ID_RCV_STAT = 0x150+bno;
    ID_RCV_INFO = 0x190+bno;
    ID_RCV_PARA = 0x1C0+bno;
    ID_SEND_GENERAL = 0x210+bno;
    TOTAL_CHANNEL = ch;
}




void RBMotorController::RBMC_SetFrictionParam(){
    for(int i=0; i<TOTAL_CHANNEL; i++){
        RBBoard_SetFrictionParameter(i+1, Joints[i].FrictionParam1*Joints[i].PPR/1000.0, Joints[i].FrictionParam2, 2);
    }
}

void RBMotorController::RBBoard_ReferenceOutEnable(bool _refEnable){
    ReferenceOutEnable = _refEnable;
}

int RBMotorController::RBBoard_CANCheck(int _canr){
    RBCAN_MB mb;
    mb.channel = CAN_CHANNEL;
    mb.data[0] = BOARD_ID;
    mb.data[1] = 0x01;
    mb.data[2] = _canr;
    mb.data[5] = 0;
    mb.dlc = 6;
    mb.id = COMMAND_CANID;

    int ret = canHandler->RBCAN_WriteData(mb);

    usleep(30*1000);

    return ret;


}

int RBMotorController::RBBoard_RequestStatus(void){
    RBCAN_MB mb;
    mb.channel = CAN_CHANNEL;
    mb.data[0] = 0x02;
    mb.dlc = 1;
    mb.id = ID_SEND_GENERAL;

    return canHandler->RBCAN_WriteData(mb);
}

int RBMotorController::RBBoard_GetStatus(void){
//    RBCAN_MB mb;
//    unsigned int ret = false;

//    mb.channel = CAN_CHANNEL;
//    mb.id = ID_RCV_STAT;
//    canHandler->RBCAN_ReadData(&mb);
//    if(mb.status != RBCAN_NODATA)    {
//        for(int i=0; i<2; i++){
//            Joints[i].CurrentStatus.B[0] = mb.data[0+i*3];
//            Joints[i].CurrentStatus.B[1] = mb.data[1+i*3];
//            Joints[i].CurrentStatus.B[2] = mb.data[2+i*3];
//        }
//        ret = true;
//        mb.status = RBCAN_NODATA;
//    }
//    return ret;
}

int RBMotorController::RBBoard_LoadDefaultValue(void){
    RBCAN_MB mb;
    mb.channel = CAN_CHANNEL;
    mb.data[0] = 0xFA;
    mb.dlc = 1;
    mb.id = ID_SEND_GENERAL;

    return canHandler->RBCAN_WriteData(mb);
}

int RBMotorController::RBBoard_SetBoardNumber(int _newbno, int _canr){
    RBCAN_MB mb;
    mb.channel = CAN_CHANNEL;
    mb.data[0] = 0xF0;
    mb.data[1] = _newbno;
    mb.data[2] = _canr;
    mb.dlc = 3;
    mb.id = ID_SEND_GENERAL;

    return canHandler->RBCAN_WriteData(mb);
}

int RBMotorController::RBBoard_RequestEncoder(int mode){
    RBCAN_MB mb;
    mb.channel = CAN_CHANNEL;
    mb.data[0] = 0x03;
    mb.data[1] = mode;      // 1-continuous, 0-oneshot
    mb.dlc = 2;
    mb.id = ID_SEND_GENERAL;

    return canHandler->RBCAN_WriteData(mb);
}

int RBMotorController::RBBoard_RequestCurrent(void){
    RBCAN_MB mb;
    mb.channel = CAN_CHANNEL;
    mb.data[0] = 0x04;
    mb.dlc = 1;
    mb.id = ID_SEND_GENERAL;

    return canHandler->RBCAN_WriteData(mb);
}

int RBMotorController::RBJoint_ResetEncoder(int ch){
    RBCAN_MB mb;
    mb.channel = CAN_CHANNEL;
    mb.data[0] = 0x06;
    mb.data[1] = ch;
    mb.dlc = 2;
    mb.id = ID_SEND_GENERAL;

    return canHandler->RBCAN_WriteData(mb);
}

int RBMotorController::RBJoint_EnableFETDriver(int ch, int enable){
    RBCAN_MB mb;
    mb.channel = CAN_CHANNEL;
    mb.data[0] = 0x0B;
    mb.data[1] = ch;
    mb.data[2] = enable;        // 1-enable, 0-disable
    mb.dlc = 3;
    mb.id = ID_SEND_GENERAL;

    return canHandler->RBCAN_WriteData(mb);
}

int RBMotorController::RBJoint_EnableFeedbackControl(int ch, int enable){
    RBCAN_MB mb;
    mb.channel = CAN_CHANNEL;
    mb.data[0] = 0x0E;
    mb.data[1] = ch;
    mb.data[2] = enable;    // 1-enable, 0-disable
    mb.dlc = 3;
    mb.id = ID_SEND_GENERAL;

    return canHandler->RBCAN_WriteData(mb);
}

int RBMotorController::RBJoint_FindHome(int ch){
    RBCAN_MB mb;
    mb.channel = CAN_CHANNEL;
    mb.data[0] = 0x11;
    mb.data[1] = ch;
    mb.dlc = 2;
    mb.id = ID_SEND_GENERAL;

    return canHandler->RBCAN_WriteData(mb);
}

int RBMotorController::RBJoint_EnableFrictionCompensation(int ch, int enable){
    RBCAN_MB mb;
    mb.channel = CAN_CHANNEL;
    mb.data[0] = 0xB1;
    mb.data[1] = ch;
    mb.data[2] = enable;    // 1-enable, 0-disable
    mb.dlc = 3;
    mb.id = ID_SEND_GENERAL;

    return canHandler->RBCAN_WriteData(mb);
}

int RBMotorController::RBJoint_SetMaxDuty(int ch, int duty){
    RBCAN_MB mb;
    mb.channel = CAN_CHANNEL;
    mb.data[0] = 0xB2;
    mb.data[1] = ch;
    mb.data[2] = duty;  // 1~100 (%)
    mb.dlc = 3;
    mb.id = ID_SEND_GENERAL;

    return canHandler->RBCAN_WriteData(mb);
}

int RBMotorController::RBJoint_SetPositionCommandMode(int _mode, int _ch){
    // MODE ==> 0: absolute position reference
    //      ==> 1: incremental position reference
    RBCAN_MB mb;
    mb.channel = CAN_CHANNEL;
    mb.data[0] = 0x14;							// command
    mb.data[1] = _ch;
    mb.data[2] = _mode;
    mb.dlc = 3;
    mb.id = ID_SEND_GENERAL;

    return canHandler->RBCAN_WriteData(mb);
}

int RBMotorController::RBJoint_ClearErrorFlag(int ch){
    RBCAN_MB mb;
    mb.channel = CAN_CHANNEL;
    mb.data[0] = 0x75;
    mb.data[1] = ch;
    mb.dlc = 2;
    mb.id = ID_SEND_GENERAL;

    return canHandler->RBCAN_WriteData(mb);
}



int RBMotorController::RBBoard_RequestTemperature(){
//    RBCAN_MB mb;
//    mb.channel = CAN_CHANNEL;
//    mb.dlc = 1;
//    mb.data[0] = 0x04;							// command
//    mb.id = ID_SEND_GENERAL;

//    RBCAN_MB mb;
//    mb.channel = CAN_CHANNEL;
//    mb.dlc = 1;
//    mb.data[0] = 0xA1;							// command
//    mb.id = ID_SEND_GENERAL;

//    return canHandler->RBCAN_WriteDataDirectly(mb);
}

int RBMotorController::RBBoard_ReadTemperature(void){
//    RBCAN_MB mb;
//    int tempInt;
//    int mTemp1, mTemp2;

//    mb.channel = CAN_CHANNEL;
//    mb.id = ID_RCV_PARA;
//    canHandler->RBCAN_ReadData(&mb);
//    if(mb.status != RBCAN_NODATA){
//        tempInt = (int)((mb.data[1]<<8) | (mb.data[0]));
//        mTemp1 = (int)((mb.data[3]<<8) | (mb.data[2]));
//        mTemp2 = (int)((mb.data[5]<<8) | (mb.data[4]));

//        BoardTemperature = ((float)tempInt)/10.0f;
//        Joints[0].Temperature = ((float)mTemp1)/10.0f;
//        Joints[1].Temperature = ((float)mTemp2)/10.0f;

//        mb.status = RBCAN_NODATA;
//        return true;
//    }else{
//        return false;
//    }
}

int RBMotorController::RBBoard_PWMCommand2ch(int mode1, short duty1, int mode2, short duty2){
    // mode ================
    // 0: not applied for channel x
    // 1: open-loop PWM in % duty
    // 2: open-loop PWM in 0.1% resolution duty
    // 3: feed-forward open-loop PWM in 0.1% resolution duty
    // 4: feed-forward current-mapped PWM(mA)
    RBCAN_MB mb;
    mb.channel = CAN_CHANNEL;
    mb.data[0] = 0x0D;
    mb.data[1] = mode1;
    mb.data[2] = (unsigned char)(duty1 & 0x00FF);
    mb.data[3] = (unsigned char)((duty1>>8) & 0x00FF);

    mb.data[4] = mode2;
    mb.data[5] = (unsigned char)(duty2 & 0x00FF);
    mb.data[6] = (unsigned char)((duty2>>8) & 0x00FF);
    mb.dlc = 7;
    mb.id = ID_SEND_GENERAL;

    return canHandler->RBCAN_WriteData(mb);
}

int RBMotorController::RBBoard_SetControlMode(int _mode){
    RBCAN_MB mb;
    mb.channel = CAN_CHANNEL;
    mb.data[0] = 0x10;		// command
    mb.data[1] = _mode;	// control mode
    // _mode = 0x00 : position control mode
    // _mode = 0x01 : current control mode
    mb.dlc = 2;
    mb.id = ID_SEND_GENERAL;

    return canHandler->RBCAN_WriteData(mb);
}




int RBMotorController::RBBoard_SendReference2ch(int ref1, int ref2){
    RBCAN_MB mb;
    mb.channel = CAN_CHANNEL;

    // Ch1
    if(Joints[0].CurrentStatus.b.MOD == 0){
        // position
        mb.data[0] = (unsigned char)(ref1 & 0x000000FF);
        mb.data[1] = (unsigned char)((ref1>>8) & 0x000000FF);
        mb.data[2] = (unsigned char)((ref1>>16) & 0x000000FF);
        mb.data[3] = (unsigned char)((ref1>>24) & 0x000000FF);
    }else{
        // current
        short curref1 = (short)ref1;

        mb.data[0] = (unsigned char)(curref1 & 0x00FF);
        mb.data[1] = (unsigned char)((curref1>>8) & 0x00FF);
        mb.data[2] = 0;
        mb.data[3] = 0;
    }

    // Ch2
    if(Joints[1].CurrentStatus.b.MOD == 0){
        // position
        mb.data[4] = (unsigned char)(ref2 & 0x000000FF);
        mb.data[5] = (unsigned char)((ref2>>8) & 0x000000FF);
        mb.data[6] = (unsigned char)((ref2>>16) & 0x000000FF);
        mb.data[7] = (unsigned char)((ref2>>24) & 0x000000FF);
    }else{
        // current
        short curref2 = (short)ref2;

        mb.data[4] = (unsigned char)(curref2 & 0x00FF);
        mb.data[5] = (unsigned char)((curref2>>8) & 0x00FF);
        mb.data[6] = 0;
        mb.data[7] = 0;
    }
    mb.dlc = 8;
    mb.id = ID_SEND_REF;

     return canHandler->RBCAN_WriteData(mb);
}

int RBMotorController::RBBoard_SendReference(void){
    int i;
    int ref[2];

    if(ReferenceOutEnable == false) return false;

    if(BOARD_TYPE == MOTOR_1CH){
        if(Joints[0].CurrentStatus.b.MOD == 1 || Joints[0].ControlMode == CONTROL_MODE_CUR){
            Joints[0].RefCurrentOld = Joints[0].RefCurrent;
            Joints[0].RefCurrent = Joints[0].Reference;
            ref[0] = (int)Joints[0].RefCurrent;
        }else if(Joints[0].ControlMode == CONTROL_MODE_POS){
            Joints[0].RefPosOld = Joints[0].RefPos;
            Joints[0].RefPos = Joints[0].Reference;
            Joints[0].RefVel = (Joints[0].RefPos-Joints[0].RefPosOld)/(RT_TIMER_PERIOD_MS/1000.0);
            ref[0] = (int)(Joints[0].RefPos*Joints[0].PPR);
        }

        if(Joints[0].ControlMode == CONTROL_MODE_PWM){
            Joints[0].RefPWMOld = Joints[0].RefPWM;
            Joints[0].RefPWM = Joints[0].Reference;
            //ref[0] = (int)Joints[0].RefPWM;

            RBBoard_PWMCommand2ch(4, Joints[0].RefPWM*10,0,0);
        }
        return RBBoard_SendReference2ch(ref[0], ref[0]);

    }else if(BOARD_TYPE == MOTOR_2CH){
        if(Joints[0].CurrentStatus.b.MOD == 1 || Joints[0].ControlMode == CONTROL_MODE_CUR){
            Joints[0].RefCurrentOld = Joints[0].RefCurrent;
            Joints[0].RefCurrent = Joints[0].Reference;
            ref[0] = (int)Joints[0].RefCurrent;
        }else if(Joints[0].ControlMode == CONTROL_MODE_POS){
            Joints[0].RefPosOld = Joints[0].RefPos;
            Joints[0].RefPos = Joints[0].Reference;
            Joints[0].RefVel = (Joints[0].RefPos-Joints[0].RefPosOld)/(RT_TIMER_PERIOD_MS/1000.0);
            ref[0] = (int)(Joints[0].RefPos*Joints[0].PPR);
        }

        if(Joints[1].CurrentStatus.b.MOD == 1 || Joints[1].ControlMode == CONTROL_MODE_CUR){
            Joints[1].RefCurrentOld = Joints[1].RefCurrent;
            Joints[1].RefCurrent = Joints[1].Reference;
            ref[1] = (int)Joints[1].RefCurrent;
        }else if(Joints[1].ControlMode == CONTROL_MODE_POS){
            Joints[1].RefPosOld = Joints[1].RefPos;
            Joints[1].RefPos = Joints[1].Reference;
            Joints[1].RefVel = (Joints[1].RefPos-Joints[1].RefPosOld)/(RT_TIMER_PERIOD_MS/1000.0);
            ref[1] = (int)(Joints[1].RefPos*Joints[1].PPR);
        }

        if(Joints[0].ControlMode == CONTROL_MODE_PWM || Joints[1].ControlMode == CONTROL_MODE_PWM){
            for(i=0; i<2; i++){
                Joints[i].RefPWMOld = Joints[i].RefPWM;
                Joints[i].RefPWM = Joints[i].Reference;
                //ref[i] = (int)Joints[i].RefPWM;
            }
            if(Joints[0].ControlMode == CONTROL_MODE_PWM && Joints[1].ControlMode == CONTROL_MODE_PWM){
                RBBoard_PWMCommand2ch(4, Joints[0].RefPWM*10, 4, Joints[1].RefPWM*10);
            }else if(Joints[0].ControlMode == CONTROL_MODE_PWM){
                RBBoard_PWMCommand2ch(4, Joints[0].RefPWM*10, 0, 0);
            }else if(Joints[1].ControlMode == CONTROL_MODE_PWM){
                RBBoard_PWMCommand2ch(0, 0, 4, Joints[1].RefPWM*10);
            }
        }

        return RBBoard_SendReference2ch(ref[0], ref[1]);
    }
    return false;
}

int RBMotorController::RBBoard_ReadEncoderData(void){
//    RBCAN_MB mb;
//    double tempDouble;

//    mb.channel = CAN_CHANNEL;
//    mb.id = ID_RCV_ENC;
//    canHandler->RBCAN_ReadData(&mb);
//    if(mb.status != RBCAN_NODATA){
//        if(TOTAL_CHANNEL == 1){
//            tempDouble = Joints[0].CurrentPosition;
//            Joints[0].EncoderValue = (int)((mb.data[0])|(mb.data[1]<<8)|(mb.data[2]<<16)|(mb.data[3]<<24));
//            Joints[0].CurrentPosition = (double)Joints[0].EncoderValue/Joints[0].PPR;
//            Joints[0].CurrentVelocity = (Joints[0].CurrentPosition - tempDouble)/(double)RT_TIMER_PERIOD_MS * 1000.0;
//        }else if(TOTAL_CHANNEL == 2){
//            Joints[0].EncoderValue = (int)((mb.data[0])|(mb.data[1]<<8)|(mb.data[2]<<16)|(mb.data[3]<<24));
//            Joints[1].EncoderValue = (int)((mb.data[4])|(mb.data[5]<<8)|(mb.data[6]<<16)|(mb.data[7]<<24));

//            tempDouble = Joints[0].CurrentPosition;
//            Joints[0].CurrentPosition = (double)Joints[0].EncoderValue/Joints[0].PPR;
//            Joints[0].CurrentVelocity = (Joints[0].CurrentPosition - tempDouble)/(double)RT_TIMER_PERIOD_MS * 1000.0;

//            tempDouble = Joints[1].CurrentPosition;
//            Joints[1].CurrentPosition = (double)Joints[1].EncoderValue/Joints[1].PPR;
//            Joints[1].CurrentVelocity = (Joints[1].CurrentPosition - tempDouble)/(double)RT_TIMER_PERIOD_MS * 1000.0;
//        }
//        mb.status = RBCAN_NODATA;
//        return true;
//    }
//    return false;
}


int RBMotorController::RBBoard_SetSwitchingMode(char _mode){
    RBCAN_MB mb;
    mb.channel = CAN_CHANNEL;
    mb.data[0] = 0x13;			// command
    mb.data[1] = _mode;		//  (0x00 : complementary, 0x01 : non complementary)
    mb.dlc = 2;
    mb.id = ID_SEND_GENERAL;

    return canHandler->RBCAN_WriteData(mb);
}

int RBMotorController::RBBoard_SetFrictionParameter(int ch, short vel, int amp, int dead){
    // vel  : joint velocity (pulse/0.01sec)
    // amp  : compensating current (mA)
    // dead : deadzone (pulse/msec)
    RBCAN_MB mb;
    mb.channel = CAN_CHANNEL;
    mb.data[0] = 0xB0;
    mb.data[1] = ch;
    mb.data[2] = (unsigned char)(vel & 0x00FF);
    mb.data[3] = (unsigned char)((vel>>8) & 0x00FF);
    mb.data[4] = amp;
    mb.data[5] = dead;
    mb.dlc = 6;
    mb.id = ID_SEND_GENERAL;

    return canHandler->RBCAN_WriteData(mb);
}

int RBMotorController::RBBoard_SetMotorPositionGain0(int _kp, int _ki, int _kd){
    RBCAN_MB mb;
    mb.channel = CAN_CHANNEL;
    mb.data[0] = 0x07;		// command
    mb.data[1] = (unsigned int)(_kp&0xFF);		// position control p-gain
    mb.data[2] = (unsigned int)((_kp>>8)&0xFF);	// position control p-gain
    mb.data[3] = (unsigned int)(_ki&0xFF);		// position control i-gain
    mb.data[4] = (unsigned int)((_ki>>8)&0xFF);	// position control i-gain
    mb.data[5] = (unsigned int)(_kd&0xFF);		// position control d-gain
    mb.data[6] = (unsigned int)((_kd>>8)&0xFF);	// position control d-gain
    mb.dlc = 7;
    mb.id = ID_SEND_GENERAL;

    return canHandler->RBCAN_WriteData(mb);
}

int RBMotorController::RBBoard_SetMotorPositionGain1(int _kp, int _ki, int _kd){
    RBCAN_MB mb;
    mb.channel = CAN_CHANNEL;
    mb.data[0] = 0x08;		// command
    mb.data[1] = (unsigned int)(_kp&0xFF);		// position control p-gain
    mb.data[2] = (unsigned int)((_kp>>8)&0xFF);	// position control p-gain
    mb.data[3] = (unsigned int)(_ki&0xFF);		// position control i-gain
    mb.data[4] = (unsigned int)((_ki>>8)&0xFF);	// position control i-gain
    mb.data[5] = (unsigned int)(_kd&0xFF);		// position control d-gain
    mb.data[6] = (unsigned int)((_kd>>8)&0xFF);	// position control d-gain
    mb.dlc = 7;
    mb.id = ID_SEND_GENERAL;

    return canHandler->RBCAN_WriteData(mb);
}

int RBMotorController::RBBoard_SetMotorCurrentGain0(int _kp, int _ki, int _kd){
    RBCAN_MB mb;
    mb.channel = CAN_CHANNEL;
    mb.data[0] = 0x09;		// command
    mb.data[1] = (unsigned int)(_kp&0xFF);		// current control p-gain
    mb.data[2] = (unsigned int)((_kp>>8)&0xFF);	// current control p-gain
    mb.data[3] = (unsigned int)(_ki&0xFF);		// current control i-gain
    mb.data[4] = (unsigned int)((_ki>>8)&0xFF);	// current control i-gain
    mb.data[5] = (unsigned int)(_kd&0xFF);		// current control d-gain
    mb.data[6] = (unsigned int)((_kd>>8)&0xFF);	// current control d-gain
    mb.dlc = 7;
    mb.id = ID_SEND_GENERAL;

    return canHandler->RBCAN_WriteData(mb);
}

int RBMotorController::RBBoard_SetMotorCurrentGain1(int _kp, int _ki, int _kd){
    RBCAN_MB mb;
    mb.channel = CAN_CHANNEL;
    mb.data[0] = 0x0A;		// command
    mb.data[1] = (unsigned int)(_kp&0xFF);		// current control p-gain
    mb.data[2] = (unsigned int)((_kp>>8)&0xFF);	// current control p-gain
    mb.data[3] = (unsigned int)(_ki&0xFF);		// current control i-gain
    mb.data[4] = (unsigned int)((_ki>>8)&0xFF);	// current control i-gain
    mb.data[5] = (unsigned int)(_kd&0xFF);		// current control d-gain
    mb.data[6] = (unsigned int)((_kd>>8)&0xFF);	// current control d-gain
    mb.dlc = 7;
    mb.id = ID_SEND_GENERAL;

    return canHandler->RBCAN_WriteData(mb);
}

int RBMotorController::RBBoard_SetJamPwmSat(int _jam, int _sat, int _jamduty, int _satduty){
    RBCAN_MB mb;
    mb.channel = CAN_CHANNEL;
    mb.data[0] = 0xF2;					// command
    mb.data[1] = (_jam & 0xFF);		// duration in msec for JAM detection
    mb.data[2] = ((_jam>>8) & (0xFF));	// duration in msec for JAM detection
    mb.data[3] = (_sat & 0xFF);		// duration in msec for PWM saturation detection
    mb.data[4] = ((_sat>>8) & (0xFF));	// duration in msec for PWM saturation detection
    mb.data[5] = _satduty;				// PWM duty for saturation detection
    mb.data[6] = _jamduty;				// PWM duty for JAM detection
    mb.dlc = 7;
    mb.id = ID_SEND_GENERAL;

    return canHandler->RBCAN_WriteData(mb);
}

int RBMotorController::RBBoard_SetErrorBound(int _ierror, int _berror, int _teeror){
    RBCAN_MB mb;
    mb.channel = CAN_CHANNEL;
    mb.data[0] = 0xF3;						// command
    mb.data[1] = (_ierror & 0xFF);			// maximum input difference error (new ref - old ref)
    mb.data[2] = ((_ierror>>8) & (0xFF));	// maximum input difference error (new ref - old ref)
    mb.data[3] = (_berror & 0xFF);			// maximum error (ref - encoder pos)
    mb.data[4] = ((_berror>>8) & (0xFF));	// maximum error (ref - encoder pos)
    mb.data[5] = (_teeror & 0xFF);			// maximum temerature warning
    mb.data[6] = ((_teeror>>8) & (0xFF));	// maximum temerature warning
    mb.dlc = 7;
    mb.id = ID_SEND_GENERAL;

    return canHandler->RBCAN_WriteData(mb);
}

int RBMotorController::RBBoard_RequestParameter(int _para){
    RBCAN_MB mb;
    mb.channel = CAN_CHANNEL;
    mb.data[0] = 0x24;			// command
    mb.data[1] = _para;		// parameter request
    // OF = 0 : channel 0, 1 and 2
    // OF = 1 : channel 3 and 4
    // _para = channel*6 + OF + 1 : Motor position control gains and encoder resolution
    // _para = channel*6 + OF + 2 : Deadzone, home search direction, home search mode and home search limit
    // _para = channel*6 + OF + 3 : Home offset and lower position limit
    // _para = channel*6 + OF + 4 : Upper limit position, maximum acceleration, maximum velocity and maximum PWM
    // _para = channel*6 + OF + 5 : Current limit and motor current control gains
    // _para = channel*6 + OF + 6 : motor current control gains
    // _para = 20 : Board number, CAN comunication rate, board type and maximun acceleration for home limit search
    // _para = 21 : Maximun velocity to limit switch, maximun velocity to offset position, duration for JAM detection and duration for saturation detection
    // _para = 22 : PWM duty for saturation, PWM duty for JAM and error bound values
    mb.dlc = 2;
    mb.id = ID_SEND_GENERAL;

    return canHandler->RBCAN_WriteData(mb);
}



int RBMotorController::RBJoint_SetDeadzone(int _ch, int _deadzone){
    RBCAN_MB mb;
    mb.channel = CAN_CHANNEL;
    mb.data[0] = 0x20+_ch;		// command
    mb.data[1] = _deadzone;	// deadzone
    mb.dlc = 2;
    mb.id = ID_SEND_GENERAL;

    return canHandler->RBCAN_WriteData(mb);
}

int RBMotorController::RBJoint_SetHomeSearchParameter(int _ch, int _rotlimit, int _dir, int _offset){
    RBCAN_MB mb;
    mb.channel = CAN_CHANNEL;
    mb.data[0] = 0x30+_ch;					// command
    mb.data[1] = _rotlimit;				// maximum number of turns to find limit switch
    mb.data[2] = _dir;						// search direction
    mb.data[3] = (_offset & 0xFF);			// offset
    mb.data[4] = ((_offset>>8) & (0xFF));	// offset
    mb.data[5] = ((_offset>>16) & (0xFF));	// offset
    mb.data[6] = ((_offset>>24) & (0xFF)); // offset
    mb.dlc = 7;
    mb.id = ID_SEND_GENERAL;

    return canHandler->RBCAN_WriteData(mb);
}


int RBMotorController::RBJoint_SetEncoderResolution(int _ch, int _res){
    RBCAN_MB mb;
    mb.channel = CAN_CHANNEL;
    mb.data[0] = 0x38+_ch;				// command
    mb.data[1] = (_res & 0xFF);         // encoder res.
    mb.data[2] = ((_res>>8) & (0xFF));	// encoder res.
    mb.dlc = 3;
    mb.id = ID_SEND_GENERAL;

    return canHandler->RBCAN_WriteData(mb);
}

int RBMotorController::RBJoint_SetMaxAccVel(int _ch, int _acc, int _vel){
    RBCAN_MB mb;
    mb.channel = CAN_CHANNEL;
    mb.data[0] = 0x40+_ch;				// command
    mb.data[1] = (_acc & 0xFF);		// maximum acceleration
    mb.data[2] = ((_acc>>8) & (0xFF));	// maximum acceleration
    mb.data[3] = (_vel & 0xFF);		// maximum velocity
    mb.data[4] = ((_vel>>8) & (0xFF));	// maximum velocity
    mb.dlc = 5;
    mb.id = ID_SEND_GENERAL;

    return canHandler->RBCAN_WriteData(mb);
}

int RBMotorController::RBJoint_SetLowerPosLimit(int _ch, int _limit, int _mode){
    RBCAN_MB mb;
    mb.channel = CAN_CHANNEL;
    mb.data[0] = 0x50;					// command
    mb.data[1] = _ch;
    // _mode = 0x00 : ignore new limit value and disable limit
    // _mode = 0x01 : ignore new limit value and enable limit
    // _mode = 0x02 : update new limit value and disable limit
    // _mode = 0x03 : update new limit value and enable limit
    mb.data[2] = _mode;
    mb.data[3] = (_limit & 0xFF);			// limit position
    mb.data[4] = ((_limit>>8) & (0xFF));	// limit position
    mb.data[5] = ((_limit>>16) & (0xFF));	// limit position
    mb.data[6] = ((_limit>>24) & (0xFF));	// limit position
    mb.dlc = 7;
    mb.id = ID_SEND_GENERAL;

    return canHandler->RBCAN_WriteData(mb);
}

int RBMotorController::RBJoint_SetUpperPosLimit(int _ch, int _limit, int _mode){
    RBCAN_MB mb;
    mb.channel = CAN_CHANNEL;
    mb.data[0] = 0x56;					// command
    mb.data[1] = _ch;
    // _mode = 0x00 : ignore new limit value and disable limit
    // _mode = 0x01 : ignore new limit value and enable limit
    // _mode = 0x02 : update new limit value and disable limit
    // _mode = 0x03 : update new limit value and enable limit
    mb.data[2] = _mode;
    mb.data[3] = (_limit & 0xFF);			// limit position
    mb.data[4] = ((_limit>>8) & (0xFF));	// limit position
    mb.data[5] = ((_limit>>16) & (0xFF));	// limit position
    mb.data[6] = ((_limit>>24) & (0xFF));	// limit position
    mb.dlc = 7;
    mb.id = ID_SEND_GENERAL;

    return canHandler->RBCAN_WriteData(mb);
}

int RBMotorController::RBJoint_SetMaxAccVelForHomeSearch(int _ch, int _acc, int _vel1, int _vel2, int _mode, int _duty){
    RBCAN_MB mb;
    mb.channel = CAN_CHANNEL;
    mb.data[0] = 0x60+_ch;		// command
    mb.data[1] = _acc;			// acceleration
    mb.data[2] = _vel1;		// maximun velocity to reach the limit switch
    mb.data[3] = _vel2;		// maximum velocity to reach the offset position
    mb.data[4] = _mode;		// search mode
    // _mode = 0x00 : limit switch and index(z-phase)
    // _mode = 0x01 : limit switch only
    // _mode = 0x02 : no limit switch. mechanical limit
    mb.data[5] = _duty;		// PWM duty(%) for mechanical limt search mode
    mb.dlc = 6;
    mb.id = ID_SEND_GENERAL;

    return canHandler->RBCAN_WriteData(mb);
}

int RBMotorController::RBJoint_GainOverride(int ch, int logscale, short msec){
    RBCAN_MB mb;
    mb.channel = CAN_CHANNEL;
    mb.data[0] = 0x6F;
    mb.data[1] = ch;
    mb.data[2] = logscale;          // 1~100 -- 0.9^logscale
    mb.data[3] = (unsigned char)(msec & 0x00FF);
    mb.data[4] = (unsigned char)((msec>>8) & 0x00FF);
    mb.dlc = 5;
    mb.id = ID_SEND_GENERAL;

    return canHandler->RBCAN_WriteData(mb);
}

int RBMotorController::RBWinding_Solenoid(int ch, int state){
    RBCAN_MB mb;
    mb.channel = CAN_CHANNEL;
    mb.data[0] = 0xC0;
    mb.data[1] = ch+1;
    mb.data[2] = state;
    mb.dlc = 3;
    mb.id = ID_SEND_GENERAL;

    return canHandler->RBCAN_WriteData(mb);
}

