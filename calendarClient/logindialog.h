#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QDebug>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

private slots:
    void onLoginButtonClicked(); // 로그인 버튼 클릭 시 실행할 슬롯
    void onSignupButtonClicked(); // 회원가입 버튼 클릭시 실행할 슬롯

signals:
    // 다른 객체(TcpClient 등)로 ID/PW를 전달하는 시그널
    void loginRequested(const QString &id, const QString &pw);
    // 회원가입 요청 들어왔다고 signup으로 보냄.
    void signupRequested();

private:
    Ui::LoginDialog *ui;
};

#endif // LOGINDIALOG_H
