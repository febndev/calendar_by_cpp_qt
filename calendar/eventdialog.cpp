#include "eventdialog.h"
#include "ui_eventdialog.h"

eventDialog::eventDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::eventDialog)
{
    ui->setupUi(this);
}

eventDialog::~eventDialog()
{
    delete ui;
}
