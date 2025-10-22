#include "eventdialog.h"
#include "ui_eventdialog.h"

EventDialog::EventDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::EventDialog)
{
    ui->setupUi(this);
}

EventDialog::~EventDialog()
{
    delete ui;
}


void EventDialog::setCalendars(const QStringList &calendars)
{
    qDebug() << "[EventDialog] setCalendars called with" << calendars
             << "this=" << this << "cmb=" << ui->cmbCalendar;
    qDebug() << "[EventDialog] setCalendars called: "<< calendars;
    ui->cmbCalendar->clear();
    ui->cmbCalendar->addItems(calendars);

    qDebug() << "[EventDialog] cmbCalendar count =" << ui->cmbCalendar->count();

}
