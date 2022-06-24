#include "VisionPage.h"
#include "ui_VisionPage.h"

VisionPage::VisionPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VisionPage)
{
    ui->setupUi(this);
}

VisionPage::~VisionPage()
{
    delete ui;
}
