#include "liftpage.h"
#include "ui_liftpage.h"

//extern pRBCORE_SHM          sharedData;
extern int                  _NO_OF_MC;
extern RBMotorController    _DEV_MC[MAX_MC];


LiftPage::LiftPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LiftPage)
{
    ui->setupUi(this);

    uiTimer = new QTimer(this);
    connect(uiTimer, SIGNAL(timeout()), this, SLOT(UpdateUI()));
    uiTimer->start(100);
}

LiftPage::~LiftPage()
{
    delete ui;
}

void LiftPage::UpdateUI()
{
    int mcId, mcCh;
    mcId = 6;
    mcCh = 1;
    QString str;
    _mSTAT stat;// = sharedData->MCStatus[mcId][mcCh];

    stat = _DEV_MC[0].Joints[0].CurrentStatus;
    str = "";
    //if(sharedData->ENCODER[mcId][mcCh].BoardConnection)
    if(_DEV_MC[0].ConnectionStatus)
    {
        str += "C  ";
    }
    else
    {
        str += "N  ";
    }
    if(stat.b.HIP == 1)
    {
        str += "H/";
    }
    else
    {
        str += "-/";
    }
    if(stat.b.RUN == 1)
    {
        str += "R/";
    }
    else
    {
        str += "-/";
    }

    str += QString().sprintf("%d", stat.b.HME);

    if(stat.b.JAM == 1) str += "JAM ";
    if(stat.b.PWM == 1) str += "PWM ";
    if(stat.b.BIG == 1) str += "BIG ";
    if(stat.b.INP == 1) str += "INP ";
    if(stat.b.FLT == 1) str += "FLT ";
    if(stat.b.ENC == 1) str += "ENC ";
    if(stat.b.CUR == 1) str += "CUR ";
    if(stat.b.TMP == 1) str += "TMP ";
    if(stat.b.PS1 == 1) str += "PS1 ";
    if(stat.b.PS2 == 1) str += "PS2 ";

    ui->LE_STATUS->setText(str);
    if(stat.b.RUN == 1 && stat.b.HME == 6)// green    Home(6) Run(on)
    {
        ui->LE_STATUS->setStyleSheet("background-color:green");
    }
    else if(stat.b.HME != 0)// red      Home(x) Run(x)
    {
        ui->LE_STATUS->setStyleSheet("background-color:red");
    }
    else// yellow   Home(0)
    {
        ui->LE_STATUS->setStyleSheet("background-color:yellow");
    }

    ui->LE_ENCODER->setText(QString().sprintf("%d", _DEV_MC[0].Joints[0].EncoderValue));
    ui->LE_REF->setText(QString().sprintf("%.3f",_DEV_MC[0].Joints[0].Reference));
    ui->LABEL_PULSE->setText(QString().sprintf("%d", int(_DEV_MC[0].Joints[0].Reference*_DEV_MC[0].Joints[0].PPR)));

    if(refFlag == true)
    {
        ui->LE_REF_STATUS->setStyleSheet("background-color:green");
    }
    else
    {
        ui->LE_REF_STATUS->setStyleSheet("background-color:red");
    }
}

void LiftPage::on_BTN_CANCHECK_clicked()
{
    _DEV_MC[0].RBBoard_CANCheck(RT_TIMER_PERIOD_MS);
}

void LiftPage::on_BTN_FETON_clicked()
{
    _DEV_MC[0].RBJoint_EnableFETDriver(1,1);
}

void LiftPage::on_BTN_FETOFF_clicked()
{
    _DEV_MC[0].RBJoint_EnableFETDriver(1,0);
}

void LiftPage::on_BTN_CTRLON_clicked()
{
    _DEV_MC[0].RBJoint_EnableFeedbackControl(1,1);
}

void LiftPage::on_BTN_CTRLOFF_clicked()
{
    _DEV_MC[0].RBJoint_EnableFeedbackControl(1,0);
}

void LiftPage::on_BTN_FINDHOME_clicked()
{
    _DEV_MC[0].RBJoint_ResetEncoder(1);
    usleep(20*1000);
    _DEV_MC[0].RBJoint_EnableFETDriver(1,1);
    usleep(20*1000);
    _DEV_MC[0].RBJoint_EnableFeedbackControl(1,1);
    usleep(20*1000);
    _DEV_MC[0].RBJoint_FindHome(1);
    _DEV_MC[0].RBJoint_SetLowerPosLimit(1, -600000, 3);
    usleep(20*1000);
    _DEV_MC[0].RBJoint_SetUpperPosLimit(1, 600000, 3);
    usleep(20*1000);
    _DEV_MC[0].RBJoint_SetPositionCommandMode(1, 0);
    usleep(20*1000);
    _DEV_MC[0].RBBoard_RequestEncoder(1);
}

void LiftPage::on_BTN_REFON_clicked()
{
    _DEV_MC[0].MoveJoints[0].RefAngleCurrent = float(_DEV_MC[0].Joints[0].EncoderValue/_DEV_MC[0].Joints[0].PPR);
    _DEV_MC[0].RBBoard_ReferenceOutEnable(true);
    refFlag = true;
}

void LiftPage::on_BTN_REFOFF_clicked()
{
    _DEV_MC[0].RBBoard_ReferenceOutEnable(false);
    refFlag = false;
}

void LiftPage::on_BTN_RIFT_UP_clicked()
{
    int goal = ui->LE_GOAL->text().toUInt();
    float angle = goal;
    float speed = 50;
    _DEV_MC[0].RBJoint_SetMoveJointSTrapi(0,angle,speed,5);
}

void LiftPage::on_BTN_DOWN_clicked()
{
    int goal = ui->LE_GOAL->text().toUInt();
    float angle = goal * -1.0;
    float speed = 50;
    _DEV_MC[0].RBJoint_SetMoveJointSTrapi(0,angle,speed,5);
}

void LiftPage::on_BTN_STOP_clicked()
{

}
