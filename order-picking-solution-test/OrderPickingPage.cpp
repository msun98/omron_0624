#include "OrderPickingPage.h"
#include "ui_OrderPickingPage.h"

OrderPickingPage::OrderPickingPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OrderPickingPage)
{
    ui->setupUi(this);
}

OrderPickingPage::~OrderPickingPage()
{
    delete ui;
}
