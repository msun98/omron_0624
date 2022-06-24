#ifndef GRIPPERPAGE_H
#define GRIPPERPAGE_H

#include <QWidget>

namespace Ui {
class GripperPage;
}

class GripperPage : public QWidget
{
    Q_OBJECT

public:
    explicit GripperPage(QWidget *parent = nullptr);
    ~GripperPage();

private:
    Ui::GripperPage *ui;
};

#endif // GRIPPERPAGE_H
