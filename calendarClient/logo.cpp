#include "logo.h"
#include "ui_logo.h"

Logo::Logo(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Logo)
{
    ui->setupUi(this);


    this->resize(1200, 800);           // 초기 창 크기 지정
    this->setMinimumSize(900, 600);    // 최소 크기 제한
    // 로고 이미지 출력
    QPixmap logopix = QPixmap(":/resources/calendar-icons.png");
    ui->logoLabel->setPixmap(logopix);
    ui->logoLabel->setGeometry(380, 30, 1000, 600);
}

Logo::~Logo()
{
    delete ui;
}
