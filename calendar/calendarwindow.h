#ifndef CALENDARWINDOW_H
#define CALENDARWINDOW_H

#include <QMainWindow>
// 메인달력 (화면 3)
namespace Ui {
class calendarwindow;
}

class calendarwindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit calendarwindow(QWidget *parent = nullptr);
    ~calendarwindow();

private:
    Ui::calendarwindow *ui;
};

#endif // CALENDARWINDOW_H
