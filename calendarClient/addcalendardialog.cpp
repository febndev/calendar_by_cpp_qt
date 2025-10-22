#include "addcalendardialog.h"
#include "ui_addcalendardialog.h"

AddCalendarDialog::AddCalendarDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddCalendarDialog)
{
    ui->setupUi(this);
}

AddCalendarDialog::~AddCalendarDialog()
{
    delete ui;
}
