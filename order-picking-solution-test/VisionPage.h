#ifndef VISIONPAGE_H
#define VISIONPAGE_H

#include <QWidget>

namespace Ui {
class VisionPage;
}

class VisionPage : public QWidget
{
    Q_OBJECT

public:
    explicit VisionPage(QWidget *parent = nullptr);
    ~VisionPage();

private:
    Ui::VisionPage *ui;
};

#endif // VISIONPAGE_H
