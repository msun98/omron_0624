#include "GripperPage.h"
#include "ui_GripperPage.h"

GripperPage::GripperPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GripperPage)
{
    ui->setupUi(this);
}

GripperPage::~GripperPage()
{
    delete ui;
}
