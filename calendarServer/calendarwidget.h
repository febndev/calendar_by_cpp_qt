#ifndef CALENDARWIDGET_H
#define CALENDARWIDGET_H

#include <QWidget>

namespace Ui {
class calendarWidget;
}

class calendarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit calendarWidget(QWidget *parent = nullptr);
    ~calendarWidget();

private:
    Ui::calendarWidget *ui;
};

#endif // CALENDARWIDGET_H
