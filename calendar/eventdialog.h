#ifndef EVENTDIALOG_H
#define EVENTDIALOG_H

// 일정 추가 수정 팝업 (사이드 뭐시기)

#include <QDialog>

namespace Ui {
class eventDialog;
}

class eventDialog : public QDialog
{
    Q_OBJECT

public:
    explicit eventDialog(QWidget *parent = nullptr);
    ~eventDialog();

private:
    Ui::eventDialog *ui;
};

#endif // EVENTDIALOG_H
