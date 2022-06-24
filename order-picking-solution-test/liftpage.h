#ifndef RIFTPAGE_H
#define RIFTPAGE_H

#include <QWidget>
#include <QTimer>
#include <iostream>
#include "lift/RBSharedMemory.h"
#include "lift/RBMotorController.h"

namespace Ui {
class LiftPage;
}

using namespace std;

class LiftPage : public QWidget
{
    Q_OBJECT

public:
    explicit LiftPage(QWidget *parent = nullptr);
    ~LiftPage();


    bool refFlag = false;

private:
    Ui::LiftPage *ui;
    QTimer *uiTimer;

private slots:
    void UpdateUI();
    void on_BTN_CANCHECK_clicked();
    void on_BTN_FETON_clicked();
    void on_BTN_FETOFF_clicked();
    void on_BTN_CTRLON_clicked();
    void on_BTN_CTRLOFF_clicked();
    void on_BTN_FINDHOME_clicked();
    void on_BTN_REFON_clicked();
    void on_BTN_REFOFF_clicked();
    void on_BTN_RIFT_UP_clicked();
    void on_BTN_DOWN_clicked();
    void on_BTN_STOP_clicked();
};

#endif // RIFTPAGE_H
