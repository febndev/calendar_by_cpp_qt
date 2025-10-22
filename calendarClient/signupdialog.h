#ifndef SIGNUPDIALOG_H
#define SIGNUPDIALOG_H

#include <QDialog>

namespace Ui {
class SignUpDialog;
}

class SignUpDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SignUpDialog(QWidget *parent = nullptr);
    ~SignUpDialog();

private slots:
    void onSignupButtonClicked(); // 가입 진행/ 검증 후 accept()
    void onCancelButtonClicked(); // 취소시 reject()

signals:
    //회원가입 요청 시그널 , tcpClient에게 보내는 시그널
    void sendSignupRequested(const QString &id,
                         const QString &pw,
                         const QString &username,
                         const QString &useremail,
                         const QString &level);


private:
    Ui::SignUpDialog *ui;
};

#endif // SIGNUPDIALOG_H
