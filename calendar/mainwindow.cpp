#include "mainwindow.h"
#include "./ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    this->resize(1200, 800);           // 초기 창 크기 지정
    this->setMinimumSize(900, 600);    // 최소 크기 제한
    // 로고 이미지 출력
    QPixmap logopix = QPixmap(":/images/resources/calendar-icons.png");
    ui->logoImg->setPixmap(logopix);
    ui->logoImg->setGeometry(200, 70, 1000, 600);



}

MainWindow::~MainWindow()
{
    delete ui;
}
