#ifndef ORDERPICKINGPAGE_H
#define ORDERPICKINGPAGE_H

#include <QWidget>

namespace Ui {
class OrderPickingPage;
}

class OrderPickingPage : public QWidget
{
    Q_OBJECT

public:
    explicit OrderPickingPage(QWidget *parent = nullptr);
    ~OrderPickingPage();

private:
    Ui::OrderPickingPage *ui;
};

#endif // ORDERPICKINGPAGE_H
