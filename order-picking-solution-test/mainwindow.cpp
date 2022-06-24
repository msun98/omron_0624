#include "mainwindow.h"
#include "ui_mainwindow.h"

int pageIndex = 0;

telnetclient *telentClinetThread;
NetworkThread *networkThread;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    telentClinetThread = new telnetclient(this);
    telentClinetThread->start();

    networkThread = new NetworkThread(this);
    networkThread->start();

    cobotPage = new CobotPage(ui->frame_cobot);
    cobotPage->setWindowFlags(Qt::Widget);
    cobotPage->move(0,0);

    mobilePage = new MobilePage(ui->frame_mobile);
    mobilePage->setWindowFlags(Qt::Widget);
    mobilePage->move(0,0);

    liftPage = new LiftPage(ui->frame_lift);
    liftPage->setWindowFlags(Qt::Widget);
    liftPage->move(0,0);

    liftPage = new LiftPage(ui->frame_lift);
    liftPage->setWindowFlags(Qt::Widget);
    liftPage->move(0,0);

    websocketPage = new WebsocketPage(ui->frame_websocket);
    websocketPage->setWindowFlags(Qt::Widget);
    websocketPage->move(0,0);


    uiTimer = new QTimer(this);
    connect(uiTimer, SIGNAL(timeout()), this, SLOT(UpdateUI()));
    uiTimer->start(100);

    ui->MAIN_TAB->setCurrentIndex(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::UpdateUI()
{

}

