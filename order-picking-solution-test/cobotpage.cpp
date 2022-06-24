#include "cobotpage.h"
#include "ui_cobotpage.h"

int speed_changed = false;

CobotPage::CobotPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CobotPage)
{
    ui->setupUi(this);

    ui->LE_IP->setText("10.0.2.7");
    //ui->LE_IP->setText("127.0.0.1");

    initFlag = false;
    cmdConfirmFlag = false;
    moveCmdFlag = false;
    moveCmdCnt = 0;


    memset(&systemStat, 0, sizeof(systemSTAT));
    systemStat.sdata.program_mode = -1;
    systemStat.sdata.robot_state = -1;

    statusDialog = new StatusDialog(ui->FRAME_STATUS);
    statusDialog->setWindowFlag(Qt::Widget);
    statusDialog->setWindowFlags(Qt::Widget);
    ui->FRAME_STATUS->setFixedSize(statusDialog->size());
    statusDialog->move(0,0);
    statusDialog->SetStatus(&systemStat);

    connect(&timer, SIGNAL(timeout()), this, SLOT(requestDataPacket()));


    connect(&cmdSocket, SIGNAL(connected()), this, SLOT(onCmdConnected()));
    connect(&cmdSocket, SIGNAL(disconnected()), this, SLOT(onCmdDisconnected()));
    connect(&dataSocket, SIGNAL(connected()), this, SLOT(onDataConnected()));
    connect(&dataSocket, SIGNAL(disconnected()), this, SLOT(onDataDisconnected()));


    connect(&systemTimer, SIGNAL(timeout()), this, SLOT(onSystemCheck()));
    connect(&logicTimer, SIGNAL(timeout()), this, SLOT(onLogic()));
    logicTimer.start(10);
    systemTimer.start(100);

}

CobotPage::~CobotPage()
{
    timer.stop();
    cmdSocket.close();
    dataSocket.close();
    delete ui;
}

void CobotPage::onSystemCheck()
{

    // base speed change
    float spd = (float)ui->HS_BASE_SPEED->value()/100.0;
    if(fabs(spd - systemStat.sdata.default_speed) > 0.005)
    {
        if(speed_changed == true)
        {
            // value changed
            BaseSpeedChange(spd);
            speed_changed = false;
        }
        else
        {
            float spd = systemStat.sdata.default_speed;
            ui->HS_BASE_SPEED->setValue(spd*100);
        }
    }


    // check initialize status
    if(initFlag == true)
    {
        int init_info = systemStat.sdata.init_state_info;
        switch(init_info)
        {
        case INIT_STAT_INFO_VOLTAGE_CHECK:
            ui->LE_INIT_POWER->setStyleSheet("QLineEdit{background-color:yellow}");
            ui->LE_INIT_DEVICE->setStyleSheet("QLineEdit{background-color:red}");
            ui->LE_INIT_SYSTEM->setStyleSheet("QLineEdit{background-color:red}");
            ui->LE_INIT_ROBOT->setStyleSheet("QLineEdit{background-color:red}");
            break;
        case INIT_STAT_INFO_DEVICE_CHECK:
            ui->LE_INIT_POWER->setStyleSheet("QLineEdit{background-color:green}");
            ui->LE_INIT_DEVICE->setStyleSheet("QLineEdit{background-color:yellow}");
            ui->LE_INIT_SYSTEM->setStyleSheet("QLineEdit{background-color:red}");
            ui->LE_INIT_ROBOT->setStyleSheet("QLineEdit{background-color:red}");
            break;
        case INIT_STAT_INFO_FIND_HOME:
            ui->LE_INIT_POWER->setStyleSheet("QLineEdit{background-color:green}");
            ui->LE_INIT_DEVICE->setStyleSheet("QLineEdit{background-color:green}");
            ui->LE_INIT_SYSTEM->setStyleSheet("QLineEdit{background-color:yellow}");
            ui->LE_INIT_ROBOT->setStyleSheet("QLineEdit{background-color:red}");
            break;
        case INIT_STAT_INFO_VARIABLE_CHECK:
            ui->LE_INIT_POWER->setStyleSheet("QLineEdit{background-color:green}");
            ui->LE_INIT_DEVICE->setStyleSheet("QLineEdit{background-color:green}");
            ui->LE_INIT_SYSTEM->setStyleSheet("QLineEdit{background-color:yellow}");
            ui->LE_INIT_ROBOT->setStyleSheet("QLineEdit{background-color:red}");
            break;
        case INIT_STAT_INFO_COLLISION_ON:
            ui->LE_INIT_POWER->setStyleSheet("QLineEdit{background-color:green}");
            ui->LE_INIT_DEVICE->setStyleSheet("QLineEdit{background-color:green}");
            ui->LE_INIT_SYSTEM->setStyleSheet("QLineEdit{background-color:green}");
            ui->LE_INIT_ROBOT->setStyleSheet("QLineEdit{background-color:yellow}");
            break;
        case INIT_STAT_INFO_INIT_DONE:
            ui->LE_INIT_POWER->setStyleSheet("QLineEdit{background-color:green}");
            ui->LE_INIT_DEVICE->setStyleSheet("QLineEdit{background-color:green}");
            ui->LE_INIT_SYSTEM->setStyleSheet("QLineEdit{background-color:green}");
            ui->LE_INIT_ROBOT->setStyleSheet("QLineEdit{background-color:green}");
            initFlag = false;
            break;
        case INIT_STAT_INFO_NOACT:
        default:
            break;
        }
    }
}

int test_flag = false;
int test_state = 0;
void CobotPage::onLogic()
{

    // Motion Sequence ===============================================
    if(test_flag == true)
    {
        switch(test_state)
        {
        case 0:
            if(IsMotionIdle())
            {
                print("test_state 0\n");
                MoveJoint(0,0,0,0,0,0);
                test_state = 1;
            }
            break;

        case 1:
            if(IsMotionIdle())
            {
                print("test_state 1\n");
                MoveJoint(50,50,50,50,50,50);
                test_state = 2;
            }
            break;
        case 2:
            if(IsMotionIdle())
            {
                print("test_state 2\n");
                MoveTCP(400,400,400,150,30,-100);
                test_state = 3;
            }
            break;
        case 3:
            if(IsMotionIdle())
            {
                print("test_state 3\n");
                test_flag = false;
                test_state = 0;
            }
            break;
        }
    }
    // ===============================================================

    //TODO
    //User Program

}

int CobotPage::IsMotionIdle()
{
    return ((cmdConfirmFlag == true) && (systemStat.sdata.robot_state == 1));
}
void CobotPage::CobotInit()
{
    QString text;
    text.sprintf("mc jall init");
    cmdConfirmFlag = false;
    cmdSocket.write(text.toStdString().c_str(), text.toStdString().length());
}
void CobotPage::ProgramMode_Real()
{
    QString text;
    text.sprintf("pgmode real");
    cmdConfirmFlag = false;
    cmdSocket.write(text.toStdString().c_str(), text.toStdString().length());
}
void CobotPage::ProgramMode_Simulation()
{
    QString text;
    text.sprintf("pgmode simulation");
    cmdConfirmFlag = false;
    cmdSocket.write(text.toStdString().c_str(), text.toStdString().length());
}
void CobotPage::MoveJoint(float joint1, float joint2, float joint3, float joint4, float joint5, float joint6, float spd, float acc)
{
    QString text;
    text.sprintf("jointall %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f", spd, acc, joint1, joint2, joint3, joint4, joint5, joint6);
    moveCmdFlag = true;
    cmdConfirmFlag = false;
    cmdSocket.write(text.toStdString().c_str(), text.toStdString().length());
    systemStat.sdata.robot_state = 3; //run
}
void CobotPage::MoveTCP(float x, float y, float z, float rx, float ry, float rz, float spd, float acc)
{
    QString text;
    text.sprintf("movetcp %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f", spd, acc, x, y, z, rx, ry, rz);
    moveCmdFlag = true;
    cmdConfirmFlag = false;
    cmdSocket.write(text.toStdString().c_str(), text.toStdString().length());
    systemStat.sdata.robot_state = 3; //run
}
void CobotPage::MoveCircle_ThreePoint(int type, float x1, float y1, float z1, float rx1, float ry1, float rz1, float x2, float y2, float z2, float rx2, float ry2, float rz2, float spd, float acc)
{
    QString text;
    char buf[15];
    if(type == 0)
    {
        sprintf(buf, "intended");
    }
    else if(type == 1)
    {
        sprintf(buf, "constant");
    }
    else if(type == 2)
    {
        sprintf(buf, "radial");
    }
    text.sprintf("movecircle threepoints %s %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f",
                 buf, spd, acc, x1, y1, z1, rx1, ry1, rz1, x2, y2, z2, rx2, ry2, rz2);
    moveCmdFlag = true;
    cmdConfirmFlag =false;
    cmdSocket.write(text.toStdString().c_str(), text.toStdString().length());
    systemStat.sdata.robot_state = 3;
}
void CobotPage::MoveCircle_Axis(int type, float cx, float cy, float cz, float ax, float ay, float az, float rot_angle, float spd, float acc)
{
    QString text;
    char buf[15];
    if(type == 0)
    {
        sprintf(buf, "intended");
    }
    else if(type == 1)
    {
        sprintf(buf, "constant");
    }
    else if(type == 2)
    {
        sprintf(buf, "radial");
    }
    text.sprintf("movecircle axis %s %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f",
                 buf, spd, acc, rot_angle, cx, cy, cz, ax, ay, az);
    moveCmdFlag = true;
    cmdConfirmFlag =false;
    cmdSocket.write(text.toStdString().c_str(), text.toStdString().length());
    systemStat.sdata.robot_state = 3;
}
void CobotPage::MoveJointBlend_Clear()
{
    QString text;
    text.sprintf("blend_jnt clear_pt");
    cmdConfirmFlag = false;
    cmdSocket.write(text.toStdString().c_str(), text.toStdString().length());
}
void CobotPage::MoveJointBlend_AddPoint(float joint1, float joint2, float joint3, float joint4, float joint5, float joint6, float spd, float acc)
{
    QString text;
    text.sprintf("blend_jnt add_pt %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f", spd, acc, joint1, joint2, joint3, joint4, joint5, joint6);
    cmdConfirmFlag = false;
    cmdSocket.write(text.toStdString().c_str(), text.toStdString().length());
    systemStat.sdata.robot_state = 3; //run
}
void CobotPage::MoveJointBlend_MovePoint()
{
    QString text;
    text.sprintf("blend_jnt move_pt");
    moveCmdFlag = true;
    cmdConfirmFlag = false;
    cmdSocket.write(text.toStdString().c_str(), text.toStdString().length());
    systemStat.sdata.robot_state = 3;
}
void CobotPage::MoveTCPBlend_Clear()
{
    QString text;
    text.sprintf("blend_tcp clear_pt");
    cmdConfirmFlag = false;
    cmdSocket.write(text.toStdString().c_str(), text.toStdString().length());
}
void CobotPage::MoveTCPBlend_AddPoint(float radius, float x, float y, float z, float rx, float ry, float rz, float spd, float acc)
{
    QString text;
    text.sprintf("blend_tcp add_pt %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f", spd, acc, radius, x, y, z, rx, ry, rz);
    cmdConfirmFlag = false;
    cmdSocket.write(text.toStdString().c_str(), text.toStdString().length());
    systemStat.sdata.robot_state = 3; //run
}
void CobotPage::MoveTCPBlend_MovePoint()
{
    QString text;
    text.sprintf("blend_tcp move_pt");
    moveCmdFlag = true;
    cmdConfirmFlag = false;
    cmdSocket.write(text.toStdString().c_str(), text.toStdString().length());
    systemStat.sdata.robot_state = 3;
}
void CobotPage::ControlBoxDigitalOut(int d0, int d1, int d2, int d3, int d4, int d5, int d6, int d7, int d8, int d9, int d10, int d11, int d12, int d13, int d14, int d15)
{
    QString text;
    text.sprintf("digital_out %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d", d0, d1, d2, d3, d4, d5, d6, d7, d8, d9, d10, d11, d12, d13, d14, d15);
    cmdConfirmFlag = false;
    cmdSocket.write(text.toStdString().c_str(), text.toStdString().length());
}
void CobotPage::ControlBoxAnalogOut(float a0, float a1, float a2, float a3)
{
    QString text;
    text.sprintf("analog_out %.3f, %.3f, %.3f, %.3f", a0, a1, a2, a3);
    cmdConfirmFlag = false;
    cmdSocket.write(text.toStdString().c_str(), text.toStdString().length());
}
void CobotPage::ToolOut(int volt, int d0, int d1)
{
    int temp_volt = volt;
    if((temp_volt != 12) && (temp_volt != 24))
    {
        temp_volt = 0;
    }

    QString text;
    text.sprintf("tool_out %d, %d, %d", temp_volt, d0, d1);
    cmdConfirmFlag = false;
    cmdSocket.write(text.toStdString().c_str(), text.toStdString().length());
}
void CobotPage::BaseSpeedChange(float spd)
{
    QString text;
    if(spd > 1.0)
    {
        spd = 1.0;
    }
    if(spd < 0.0)
    {
        spd = 0.0;
    }
    text.sprintf("sdw default_speed %.3f", spd);
    cmdConfirmFlag = false;
    cmdSocket.write(text.toStdString().c_str(), text.toStdString().length());
}
void CobotPage::MotionPause()
{
    QString text;
    text.sprintf("task pause");
    cmdConfirmFlag = false;
    cmdSocket.write(text.toStdString().c_str(), text.toStdString().length());
}
void CobotPage::MotionHalt()
{
    QString text;
    text.sprintf("task stop");
    cmdConfirmFlag = false;
    cmdSocket.write(text.toStdString().c_str(), text.toStdString().length());
}
void CobotPage::MotionResume()
{
    QString text;
    text.sprintf("task resume_a");
    cmdConfirmFlag = false;
    cmdSocket.write(text.toStdString().c_str(), text.toStdString().length());
}
void CobotPage::CollisionResume()
{
    QString text;
    text.sprintf("task resume_b");
    cmdConfirmFlag = false;
    cmdSocket.write(text.toStdString().c_str(), text.toStdString().length());
}


// connect & disconnect ------------------
void CobotPage::onCmdConnected()
{
    ui->BTN_CONNECT_COM->setText("Disconnect");
}
void CobotPage::onCmdDisconnected()
{
    ui->BTN_CONNECT_COM->setText("Connect");
}
void CobotPage::onDataConnected()
{
    ui->BTN_CONNECT_DATA->setText("Disconnect");
}
void CobotPage::onDataDisconnected()
{
    ui->BTN_CONNECT_DATA->setText("Connect");
}
// ---------------------------------------


void CobotPage::onReadyCmdRead()
{
    QByteArray datas = cmdSocket.readAll();
    if(QString(datas.data()).compare("The command was executed\n") == 0)
    {
        //print("Confirm!!\n");
        cmdConfirmFlag = true;
        if(moveCmdFlag == true)
        {
            moveCmdCnt = 5;
            systemStat.sdata.robot_state = 3;
            moveCmdFlag = false;
        }
    }
    else
    {
        print(QString::fromLocal8Bit(datas.data(),datas.length()));
    }
}
void CobotPage::print(QString value)
{
    QString str = ui->textPrint->toPlainText();
    ui->textPrint->setText(str+value);
    QScrollBar *sb = ui->textPrint->verticalScrollBar();
    sb->setValue(sb->maximum());
}
void CobotPage::onReadyDataRead()
{

    QByteArray datas = dataSocket.readAll();
    recvBuf +=datas;

    while(recvBuf.length() > 4 )
    {
        if( recvBuf[0] == '$')
        {
            int size = ((int)((unsigned char)recvBuf[2]<<8)|(int)((unsigned char)recvBuf[1]));
            if(size <= recvBuf.length() )
            {
                int templen = recvBuf.length();
                if(3 == recvBuf[3])
                {
                    if(moveCmdCnt > 0)
                    {
                        moveCmdCnt--;
                    }
                    else
                    {
                        memcpy(&systemStat,recvBuf.data(),sizeof(systemSTAT));
                    }
                    recvBuf.remove(0,sizeof(systemSTAT));

                    //                    float spd = systemStat.sdata.default_speed;
                    //                    ui->HS_BASE_SPEED->setValue(spd*100);
                    //                    ui->LB_BASE_SPEED->setText(QString().sprintf("%.1f%%", spd*100.0));

                    //char buf[256];
                    //sprintf(buf, "stat: %d, %d, %d", size, templen, sizeof(systemSTAT));
                    //sprintf(buf, "pgmode, movecnt: %d, %d\n", systemStat.sdata.program_mode, moveCmdCnt);
                    //print(buf);
                    //   print("systemStat Data Received\n");
                }
                else if(4 == recvBuf[3])
                {
                    memcpy(&systemConfig,recvBuf.data(),sizeof(systemCONFIG));
                    recvBuf.remove(0,sizeof(systemCONFIG));
                    QString str = ui->textPrint->toPlainText();

                    //   print("systemConfig Data Received\n");

                }
                else if(10 == recvBuf[3])
                {
                    memcpy(&systemPopup,recvBuf.data(),sizeof(systemPOPUP));
                    recvBuf.remove(0,sizeof(systemPOPUP));

                    //  print("systemPopup Data Received\n");
                }
                else
                {
                    recvBuf.remove(0,1);
                }

            }

        }
        else
        {
            recvBuf.remove(0,1);
        }
    }


}

void CobotPage::requestDataPacket()
{
    //dataSocket.write("regcfg");
    dataSocket.write("reqdata");
}



void CobotPage::on_BTN_CONNECT_COM_clicked()
{
    if(ui->BTN_CONNECT_COM->text() == "Connect")
    {
        // connect
        cmdSocket.connectToHost(QHostAddress(ui->LE_IP->text()), ui->textCmdPort->text().toInt());
        connect(&cmdSocket, SIGNAL(readyRead()), this, SLOT(onReadyCmdRead()));
    }
    else
    {
        // disconnect
        cmdSocket.close();
    }
}

void CobotPage::on_BTN_CONNECT_DATA_clicked()
{
    if(ui->BTN_CONNECT_DATA->text() == "Connect")
    {
        // connect
        dataSocket.connectToHost(QHostAddress(ui->LE_IP->text()), ui->textDataPort->text().toInt());
        connect(&dataSocket, SIGNAL(readyRead()), this, SLOT(onReadyDataRead()));
        timer.start(20);
    }
    else
    {
        // disconnect
        timer.stop();
        dataSocket.close();
    }
}

void CobotPage::on_BTN_COBOT_INIT_clicked()
{
    CobotInit();
    initFlag = true;
}

void CobotPage::on_BTN_MODE_REAL_clicked()
{
    ProgramMode_Real();
}

void CobotPage::on_BTN_MODE_SIMULATION_clicked()
{
    ProgramMode_Simulation();
}

void CobotPage::on_BTN_CLEAR_clicked()
{
    ui->textPrint->clear();
}

void CobotPage::on_HS_BASE_SPEED_valueChanged(int value)
{
    float spd = (float)value/100.0;
    ui->LB_BASE_SPEED->setText(QString().sprintf("%.1f%%", spd*100.0));
    speed_changed = true;
}

void CobotPage::on_BTN_GET_JOINT_AND_TCP_clicked()
{
    ui->LE_JOINT_LIST->setText(QString().sprintf("%.3f, %.3f, %.3f, %.3f, %.3f, %.3f",
                                                 systemStat.sdata.jnt_ref[0], systemStat.sdata.jnt_ref[1], systemStat.sdata.jnt_ref[2],
            systemStat.sdata.jnt_ref[3], systemStat.sdata.jnt_ref[4], systemStat.sdata.jnt_ref[5]));

    ui->LE_TCP_LIST->setText(QString().sprintf("%.3f, %.3f, %.3f, %.3f, %.3f, %.3f",
                                               systemStat.sdata.tcp_ref[0], systemStat.sdata.tcp_ref[1], systemStat.sdata.tcp_ref[2],
            systemStat.sdata.tcp_ref[3], systemStat.sdata.tcp_ref[4], systemStat.sdata.tcp_ref[5]));
}

void CobotPage::on_BTN_MOTION_PAUSE_clicked()
{
    MotionPause();
}

void CobotPage::on_BTN_MOTION_RESUME_clicked()
{
    MotionResume();
}

void CobotPage::on_BTN_MOTION_HALT_clicked()
{
    MotionHalt();
    // escape the motion sequence
    test_state = 0;
    test_flag = false;
}

void CobotPage::on_BTN_COLLISION_RESUME_clicked()
{
    CollisionResume();
}
