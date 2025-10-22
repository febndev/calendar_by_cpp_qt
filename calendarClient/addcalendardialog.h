#ifndef ADDCALENDARDIALOG_H
#define ADDCALENDARDIALOG_H

#pragma once
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

signals:
    void addCalRequested(const QString& name, int is_Public);

private:
    Ui::AddCalendarDialog *ui;

private slots:

    void onCreateButtonClicked();
    void onCancelButtonClicked();

};

#endif // ADDCALENDARDIALOG_H
