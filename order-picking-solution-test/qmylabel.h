#ifndef QMYLABEL_H
#define QMYLABEL_H

#include <QObject>
#include <QLabel>
#include <QMouseEvent>

class QMyLabel : public QLabel
{
    Q_OBJECT

public:
    QMyLabel(QWidget *parent);

protected:
    void mousePressEvent(QMouseEvent *ev) override;
    void mouseReleaseEvent(QMouseEvent *ev) override;

Q_SIGNALS:
    void mouse_clicked(double x, double y);
    void mouse_released(double w, double z);
};

#endif // QMYLABEL_H
