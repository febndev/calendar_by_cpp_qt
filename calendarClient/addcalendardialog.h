#ifndef ADDCALENDARDIALOG_H
#define ADDCALENDARDIALOG_H

#include <QDialog>

namespace Ui {
class AddCalendarDialog;
}

class AddCalendarDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddCalendarDialog(QWidget *parent = nullptr);
    ~AddCalendarDialog();

private:
    Ui::AddCalendarDialog *ui;
};

#endif // ADDCALENDARDIALOG_H
