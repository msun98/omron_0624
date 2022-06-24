#include "qmylabel.h"

QMyLabel::QMyLabel(QWidget *parent)
    : QLabel(parent)
{

}

void QMyLabel::mousePressEvent(QMouseEvent *ev) //이미지의 배율에 맟추어 클릭 위치가 나옴.
{
//    if(ev->button() == Qt::RightButton)
    if(ev->button() == Qt::LeftButton)
    {
        // calc ratio
        double w = this->width();
        double h = this->height();
//        double cx = w/2;
//        double cy = h/2;
        double x = ev->x();
        double y = ev->y();
//        double ratio_x = (x-cx)/w;
//        double ratio_y = -(y-cy)/h;
        double ratio_x = x/w;
        double ratio_y = y/h;
        emit mouse_clicked(ratio_x, ratio_y);
    }
    
}


void QMyLabel::mouseReleaseEvent(QMouseEvent *ev) //이미지의 배율에 맟추어 클릭 위치가 나옴.
{
//    if(ev->button() == Qt::RightButton)
    if(ev->button() == Qt::LeftButton)
    {
        // calc ratio
        double w = this->width();
        double h = this->height();
//        double cx = w/2;
//        double cy = h/2;
        double x = ev->x();
        double y = ev->y();
        double ratio_x = x/w;
        double ratio_y = y/h;
        emit mouse_released(ratio_x, ratio_y);
    }

}
