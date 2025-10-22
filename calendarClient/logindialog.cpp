#include "logindialog.h"
#include "ui_logindialog.h"

#include <QPushButton>
#include <QMessageBox>


LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    // login 버튼 눌렀을 때 onLoginButtonClicked() 실행
    connect(ui->loginButton, &QPushButton::clicked,
           this, &LoginDialog::onLoginButtonClicked);

    // signupButton 눌렀을 때 onsignupButtonClicked() 실행
    connect(ui->signupButton, &QPushButton::clicked,
            this, &LoginDialog::onSignupButtonClicked);

}

LoginDialog::~LoginDialog()
{
    delete ui;
}

// 로그인 버튼 클릭 시 실행할 슬롯
void LoginDialog::onLoginButtonClicked()
{
    QString id = ui->idLineEdit->text().trimmed();
    QString pw = ui->pwLineEdit->text().trimmed();

    if (id.isEmpty() || pw.isEmpty()){
        QMessageBox::warning(this, "로그인 실패", "아이디와 비밀번호를 입력하세요.");
        qDebug() << "아이디나 비밀번호가 비어있습니다.";
        return;
    }

    qDebug() << "로그인 요청:" << id << pw;

    //시그널을 발행해서 TcpClient로 전달
    emit loginRequested(id, pw);
}

void LoginDialog::onSignupButtonClicked()
{   // 여기서는 회원가입 요청이 들어왔다고
    qDebug() << "회원가입 요청";
    emit signupRequested();
}
