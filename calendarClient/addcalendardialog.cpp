#include "addcalendardialog.h"
#include "ui_addcalendardialog.h"

//0819 [캘린더 추가]
#include <QVBoxLayout>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>

AddCalendarDialog::AddCalendarDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddCalendarDialog)
{
    ui->setupUi(this);

    //0819 [캘린더 추가]
    setWindowTitle("캘린더 추가");

    // 추가하기 버튼 클릭시 연결
    connect(ui->btnCreate, &QPushButton::clicked,
            this, &AddCalendarDialog::onCreateButtonClicked);
}

// 소멸자
AddCalendarDialog::~AddCalendarDialog()
{
    delete ui;
}

// 추가하기 버튼 클릭시 실행할 슬롯

void AddCalendarDialog::onCreateButtonClicked(){

    QString name = ui->leName->text().trimmed();
    bool is_Public = ui->cbPublic->isChecked();
    // true = 1 , 1이면 공유캘린더

    if (name.isEmpty()){
        QMessageBox::warning(this, "제목 필수 입력", "캘린더 제목을 입력해주세요.");
        qDebug() << "캘린더 제목 미입력";
        return;
    }
    qDebug() << "캘린더 추가: " << name <<is_Public;

    emit addCalRequested(name, is_Public);
}

void AddCalendarDialog::onCancelButtonClicked() {
    reject();
}
