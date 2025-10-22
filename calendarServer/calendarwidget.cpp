#include "calendarwidget.h"
#include "ui_calendarwidget.h"

calendarWidget::calendarWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::calendarWidget)
{
    ui->setupUi(this);
}

calendarWidget::~calendarWidget()
{
    delete ui;
}
