#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <iostream>
#include <QPixmap>

#include <cobotpage.h>
#include <mobilepage.h>
#include <liftpage.h>
#include <websocketpage.h>

#include <telnetclient.h>
#include <networkthread.h>

//#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include "cv_to_qt.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


using namespace std;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    CobotPage *cobotPage;
    MobilePage *mobilePage;
    LiftPage *liftPage;
    WebsocketPage *websocketPage;

    QTimer *uiTimer;
    void mouse_clicked(double x, double y);
    void mouse_released(double w, double z);



private slots:
    void UpdateUI();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
