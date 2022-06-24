#include "mobilepage.h"
#include "ui_mobilepage.h"
#include "cv_to_qt.h"
#include "qmylabel.h"

extern telnetclient *    telentClinetThread;
extern NetworkThread *networkThread;

MobilePage::MobilePage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MobilePage)
{
    ui->setupUi(this);

    uiTimer = new QTimer(this);
    connect(uiTimer, SIGNAL(timeout()), this, SLOT(UpdateUI()));
    uiTimer->start(100);


    connect(&timer, SIGNAL(timeout()), this, SLOT(timeloop()));

    connect(&timer,SIGNAL(timeout()),this,SLOT(makeRect()));

    connect(ui->txt, SIGNAL(mouse_clicked(double, double)), this, SLOT(mouse_clicked(double, double)));


//    map=cv::Mat::ones(ui->txt->height(),ui->txt->width(),CV_8UC3)*cv::Scalar(255,255,255);
//    cout<<map<<endl;

    //    void mouseReleaseEvent(double x, double y);
//    connect(ui->txt, SIGNAL(mouse_released(double, double)), this, SLOT(mouse_released(double, double)));
    //트리거는 시간, 시간에 맞추어 큐가 진행한다.
//    timer.setInterval(100);//몇 미리세컨드마다 쓸것인가???, 타이머의 멤버 함수
}

MobilePage::~MobilePage()
{
    delete ui;
}

void MobilePage::UpdateUI()
{
    QString text = QString::fromStdString(telentClinetThread->serverMessage);
    //cout << telentClinetThread->serverMessage <<endl;

    if(text.length() < 1)
    {

    }
    else
    {
        ui->te_terminal->append(text);
        // extrect need word like failed or sucess
//        cout<<text.toStdString()<<endl;

        //for c++ example
//        string Text = text.toStdString();
//        istringstream TText(Text);
//        string subs1;
//        vector<string> v;
//        while(getline(TText,subs1,' '))
//        {
//            v.push_back(subs1);
//            cout<<v[0]<<endl;
////            for(int i =0; i<v.size();i++){
////                cout<<v[i]<<endl;
////            }
//        }

        QStringList list = text.split(" ");
        qDebug()<<list[0];// using list[0] compare word failed or success!

        if (list[0]=="Failed")
        {
            cout<<"FFFFF"<<endl;
        }

        else if(list[0]=="Completed")
        {
            cout<<"SSSSS"<<endl;
            flag = true; // 로봇의 출발을 알리는 플래그
        }
    }
    telentClinetThread->serverMessage = "";

    SetRobotInfoText();
}

void MobilePage::SetRobotInfoText()
{

    ui->te_status->setText(networkThread->status);
    ui->le_battery->setText(QString::number(networkThread->baterry));
    ui->le_location->setText(QString::number(networkThread->x) + " , " + QString::number(networkThread->y) + " , " + QString::number(networkThread->heading));
    ui->le_temperature->setText(QString::number(networkThread->temperature));

}

void MobilePage::on_btn_send_clicked()
{
    QString text = ui->te_send->toPlainText();
    telentClinetThread->sendCommand = text.toStdString();
    cout << text.toStdString() << endl;
    telentClinetThread->flag = 1;
}

void MobilePage::on_btn_say_clicked()
{
    QString text = "dotask say ";
    text.append(ui->le_say->text());
    telentClinetThread->sendCommand = text.toStdString();
    cout << text.toStdString() << endl;
    telentClinetThread->flag = 1;
}

void MobilePage::on_btn_dock_clicked()
{
    QString text = "dock";
    telentClinetThread->sendCommand = text.toStdString();
    cout << text.toStdString() << endl;
    telentClinetThread->flag = 1;
}

void MobilePage::on_btn_undock_clicked()
{
    QString text = "undock";
    telentClinetThread->sendCommand = text.toStdString();
    cout << text.toStdString() << endl;
    telentClinetThread->flag = 1;
}

void MobilePage::on_btn_stop_clicked()
{
    QString text = "stop";
    telentClinetThread->sendCommand = text.toStdString();
    cout << text.toStdString() << endl;
    telentClinetThread->flag = 1;
}

void MobilePage::on_btn_movegoal_clicked()
{
    cout<<"hi"<<endl;
}

void MobilePage::on_add_clicked()
{
    QString x1 = ui -> x_get -> text();
    QString y1 = ui -> y_get -> text();
    num();
    QString add = x1+","+y1;

    ui -> listView -> addItem(add);
    ui -> x_get -> setText("");
    ui -> y_get -> setText("");
    add_flag = true;
}

void MobilePage::num()
{
//    QString text_1,text_2,text_3,text_4;

    QString x1 = ui -> x_get -> text();
    QString y1 = ui -> y_get -> text();

//    문자열의 길이가 0이라면 무시, 아니면 경로 추가
//  QString으로 받아온 문자를 int 형으로 변환해야 함.

    if (x1.length()!=0 && y1.length()!=0){
        text_1_x=x1.toInt(),text_1_y=y1.toInt();
        q.push(make_pair(text_1_x,text_1_y));//que의 값을 넣음.
    }

}

int trialNum=1;
void MobilePage::timeloop() //queue
{
    // 큐에 값이 있으면 실행
    if (state == 0)
    {
        if (q.empty()) //큐가 비었는지를 확인.
        {
//            cout<<"경로가 비었다."<<endl;
            return;
        }
        QString a = QString::fromStdString(to_string(q.front().first));
        QString b = QString::fromStdString(to_string(q.front().second));

        QString text_1 = "dotask gotopoint ";

        text_1.append(a);
        text_1.append(" ");
        text_1.append(b);
//        cout << a<< endl;
        telentClinetThread->sendCommand = text_1.toStdString();
        telentClinetThread->flag = 1;

        // 로봇에게 명령을 전달하기 위함.
        cout << text_1.toStdString() << endl;
//        qDebug() << text_1;// 어플리케이션 아웃풋에 출력되도록 작업.
//        cout << q.front().first<<","<< q.front().second<< endl;


        QString number = QString::number(trialNum);
        ui -> num -> setText(" TASK "+number);

        ui-> num ->setStyleSheet("color: rgb(0, 0, 0);\nbackground-color: rgb(255, 255, 255);");
        trialNum++;

        if (q.size()==1)
        {
            ui -> num -> setText(" TASK "+number);
            ui -> num->setStyleSheet("color: rgb(255, 255, 255);\nbackground-color: rgb(200, 50, 0);");
//            cout<<"hi"<<endl;
            trialNum=1;
//            ui -> num -> setText("TASK ");
            ui->MovePoint->setStyleSheet("color: rgb(46, 52, 54);\nbackground-color: rgb(211, 215, 207);");
        }
        q.pop();
        ui->listView->takeItem(0);
    //         Queue 의 Pair 출력
//        cout << new_que.first << " , " << new_que.second << endl;
            //값이 빠지지 않게 설정.
        flag = false;
        state = 1;
//        flag=false;
//        ui->listView->takeItem(0);
//        cout << "Is it empty?: " << (q.empty() ? "Yes" : "No") << '\n';
    }
    else if(state == 1) //기다려......
    {
        if (flag == true)
        {
            flag = false;
            state = 0;
        }
//        cout<<"hi"<<endl; //else if 문 1초마다 실행중.
    }

}
void MobilePage::on_MovePoint_clicked(){
    timer.setInterval(100);//몇 미리세컨드마다 쓸것인가???, 타이머의 멤버 함수
    timer.start();
    ui->MovePoint->setStyleSheet("background-color: rgb(238, 238, 200)");
    state = 0;

}

void MobilePage::on_departure_clicked()
{
    flag = true; // 로봇의 출발을 알리는 플래그
//    cout<<"go"<<endl;
}

void MobilePage::on_pushButton_clicked()
{
    QString filename("0617.txt");
    read(filename);
}

void MobilePage::read(QString filename) // 데이터 읽음과 동시에 분리.
{
    QFile file(filename);
    if(!file.open(QFile::ReadOnly | QFile::Text))
    {
        qDebug() << " Could not open the file for reading";
        return;
    }

    QTextStream in(&file);
    QString myText = in.readAll();
//    qDebug() << myText;

//    QString str = "DATA";

//////////////////////방법 1//////////////////
    int index = myText.indexOf("DATA", Qt::CaseInsensitive)+5; //data의 첫번째 문자 위치 544250
    QString str2 = myText.mid(index);
//    qDebug() << str2;

////    Eigen::Vector3d target;
////    QStringList list = text.split(" ");
////    vector <cv::Point2d> point_list;
////    point_list.push_back(text.split(" "));
////    cv::Point2d pt1;
    QStringList x = str2.split("\n"); // x
//    qDebug() << x;
    QVector <cv::Point2d> point_list;
////    point_list.push_back(x);
////    qDebug() << point_list;
////    qDebug()<<x.size();
    cv::Mat map(ui->txt->height(),ui->txt->width(),CV_8U,cv::Scalar(255));
    ////min max 찾는 for
    for (int i=0; i<x.size()-1; i++)
    {
       QStringList x_1; //선언을 함.
       x_1 = x[i].split(" ");

       x_original= x_1[0].toDouble();
       y_original= x_1[1].toDouble();

       vector_x.push_back(x_original);
       vector_y.push_back(y_original);
    }

    double max_x = *max_element(vector_x.begin(), vector_x.end());
    double max_y = *max_element(vector_y.begin(), vector_y.end());
    double min_x = *min_element(vector_x.begin(), vector_x.end());
    double min_y = *min_element(vector_y.begin(), vector_y.end());

    for (int i=0; i<x.size()-1; i++)
    {
       QStringList x_1; //선언을 함.
       x_1 = x[i].split(" ");

       double x_original= x_1[0].toDouble();
       double y_original= x_1[1].toDouble();
//       cout<<x_original<<endl;

       int mmperpix_x = round((max_x-min_x)/ui->txt->width());
       int mmperpix_y = round((max_y-min_y)/ui->txt->height());

       double x = (x_original+abs(min_x))/mmperpix_x;
       double y = (y_original+abs(min_y))/mmperpix_y;

//       cout<<x_original<<endl;

       point_list.push_back(cv::Point2d(x,y));
       map.ptr<uchar>(point_list[i].y)[int(point_list[i].x)]=0;

    }
//    cv::imwrite("qwer.bmp", map);
    cv::cvtColor(map, colored_map, cv::COLOR_GRAY2BGR);
//    cv::imwrite("qwer2.bmp", colored_map);



//    cout<<point_list.size()<<endl;


//    cv::Mat map(2000,2000,CV_8U,cv::Scalar(255));
//    for(int i=0; i<x.size()-1; i++){
//        map.ptr<uchar>(point_list[i].y)[int(point_list[i].x)]=0;
//        cv::line(map,(point_list[i]),(point_list[i]),Scalar(20, 0, 255), 5);
//         cv::circle(map, (point_list[i]), 32.0, Scalar(0, 0, 255), 1, 1);
//         cout<<point_list[i]<<endl;
//        ui->pic->setPixmap(QPixmap::fromImage(mat_to_qimage_cpy(map)));
//        cout<<point_list[i].x<<endl;
//    }
//    cv::imshow('map',map);
//    cv::circle(colored_map, cv::Point(10,50), 2, cv::Scalar(0, 50, 255),2,-1);
    ui->txt->setPixmap(QPixmap::fromImage(mat_to_qimage_cpy(colored_map)));
    Map_original = colored_map.clone();
//////////////////////방법 2//////////////////

//    ui -> txt -> setText(str2);

//    qDebug() << c;
//    str2=str1.substr(index);
//    qDebug() << str2;

    // index == 6

    file.close();
}
/////이전 값 기억하고 있을 것///
cv::Point2d pt1_old ;

bool color_flag = 1;

void MobilePage::mouse_clicked(double x, double y)
{
    pt1.x = x * ui->txt->width();
    pt1.y = y * ui->txt->height();
//    cout<<pt1<<endl;
    ui -> x_get -> setText(QString::number(pt1.x));
    ui -> y_get -> setText(QString::number(pt1.y));
    color_flag = 1;

    ///put color circle on the image
//    cout<<colored_map<<endl;
//    cout<<color_flag<<endl;
    if (color_flag)
    {
        cv::circle(colored_map, cv::Point(pt1.x,pt1.y), 2, cv::Scalar(0, 50, 255),2,-1);
        ui->txt->setPixmap(QPixmap::fromImage(mat_to_qimage_cpy(colored_map)));
//        color_flag = false;

//        if (color_flag==false)
//        {
//            ui->txt->setPixmap(QPixmap::fromImage(mat_to_qimage_cpy(Map_original)));
//        }
        if (add_flag)
        {
            ui->txt->setPixmap(QPixmap::fromImage(mat_to_qimage_cpy(colored_map)));
            add_flag = false;
            color_flag = false;
        }

        else
        {
            ui->txt->setPixmap(QPixmap::fromImage(mat_to_qimage_cpy(Map_original)));
        }
    }



    cout<<color_flag<<endl;
//    cv::circle(colored_map, cv::Point(pt1.x,pt1.y), 2, cv::Scalar(0, 50, 255),2,-1);

//    ui->txt->setPixmap(QPixmap::fromImage(mat_to_qimage_cpy(colored_map)));
    state = 0;
}


void MobilePage::on_delete_2_clicked()
{
    q.pop();
//    ui->listView->setCurrentRow(0);
//    ui -> listView -> addItem(push_num);


    ui->listView->takeItem(push_num);//숫자는 빼야하는 순서임.
    push_num = 0;
}

void MobilePage::on_UP_PUSH_clicked()
{
    push_num = push_num + 1;
    ui -> Push_num ->setText(QString::number(push_num));
    cout<<push_num<<endl;
}

void MobilePage::on_UP_DOWN_clicked()
{
    push_num = push_num - 1;
    ui -> Push_num ->setText(QString::number(push_num));
    cout<<push_num<<endl;
}
