#include "calendarwindow.h"
#include "ui_calendarwindow.h"

calendarwindow::calendarwindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::calendarwindow)
{
    ui->setupUi(this);
    this->resize(1200, 800);           // 초기 창 크기 지정
    this->setMinimumSize(900, 600);    // 최소 크기 제한
}

calendarwindow::~calendarwindow()
{
    delete ui;
}
