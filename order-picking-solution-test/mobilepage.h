#ifndef MOBILEPAGE_H
#define MOBILEPAGE_H

#include <QWidget>

#include <telnetclient.h>
#include <QTimer>
#include <networkthread.h>
#include <queue>
#include <sstream>
#include <vector>

#include "cv_to_qt.h"

namespace Ui {
class MobilePage;
}

using namespace std;

class MobilePage : public QWidget
{
    Q_OBJECT

public:
    explicit MobilePage(QWidget *parent = nullptr);
    ~MobilePage();

    QTimer *uiTimer;
    QTimer timer;
    int x1;
    int y1;
    int text_1_x,text_1_y;
    queue <pair<int, int> > q;
    cv::Point2d pt1;
    cv::Point2d pt2;
    double x_original;
    double y_original;


    void SetRobotInfoText();

private:
    Ui::MobilePage *ui;

    bool flag;//도착 플래그

    int state = 0; //스테이트 머신의 상태
    void read(QString filename);
    QVector <cv::Point2d> point_list;
    QVector<double> vector_x;
    QVector<double> vector_y;
    cv::Mat colored_map;
    cv::Mat Map_original;
    bool add_flag = false;
    int push_num=0;


private slots:
    void UpdateUI();

    void on_btn_send_clicked();
    void on_btn_say_clicked();
    void on_btn_dock_clicked();
    void on_btn_undock_clicked();
    void on_btn_stop_clicked();
    void on_btn_movegoal_clicked();
    void on_add_clicked();
    void num();
    void on_MovePoint_clicked();
    void timeloop();
    void on_departure_clicked();

    void on_pushButton_clicked();
//    void on_MovePoint_clicked();
//    void read();

//private Q_SLOTS:
    void mouse_clicked(double x, double y);
    void on_delete_2_clicked();
    void on_UP_PUSH_clicked();
    void on_UP_DOWN_clicked();
};

#endif // MOBILEPAGE_H
