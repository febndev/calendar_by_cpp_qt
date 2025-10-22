#include "signupdialog.h"
#include "ui_signupdialog.h"

#include <QPushButton>
#include <QMessageBox>

SignUpDialog::SignUpDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SignUpDialog)
{
    ui->setupUi(this);

    // 회원가입 버튼 눌렀을 때 client - > server -> db 로 보낼 수 있게끔 하는 함수
    connect(ui->okButton, &QPushButton::clicked,
            this, &SignUpDialog::onSignupButtonClicked);

    // 돌아가기 버튼 눌렀을 때 다시 login 창으로 가기
    connect(ui->cancelClicked, &QPushButton::clicked,
            this, &SignUpDialog::onCancelButtonClicked);

}

SignUpDialog::~SignUpDialog()
{
    delete ui;
}

void SignUpDialog::onSignupButtonClicked()
{
    QString id = ui-> signIDLineEdit->text().trimmed();
    QString pw = ui -> signpwLineEdit ->text().trimmed();
    QString pw2 = ui ->signpw2LineEdit->text().trimmed();
    QString username = ui->usernameLineEdit->text().trimmed();
    QString useremail = ui->userEmailLineEdit->text().trimmed();
    QString level = ui->levelComboBox->currentText().trimmed();

    if(id.isEmpty() || pw.isEmpty() || username.isEmpty() || useremail.isEmpty()){
        QMessageBox::warning(this, "항목 미입력","모든 항목을 입력해주세요.");
        qDebug() << "항목 미입력";
        return;
    }

    if (pw != pw2){
        QMessageBox::warning(this, "비밀번호 검증 오류","같은 비밀번호를 입력하세요.");
        qDebug() << "비밀번호 동일하지 않음.";
        return;
    }
    QString role;
    // role 값으로 받은 것을 정수로 대입
    if (level == "사원"){
        role = QStringLiteral("D");
    } else if (level == "대리"){
        role = QStringLiteral("C");
    } else if (level == "과장"){
        role = QStringLiteral("B");
    } else if (level == "부장"){
        role = QStringLiteral("A");
    }

    // 시그널 발송
    emit sendSignupRequested(id, pw, username, useremail, role);
}

void SignUpDialog::onCancelButtonClicked()
{
    reject();
}
