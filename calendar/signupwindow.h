#ifndef SIGNUPWINDOW_H
#define SIGNUPWINDOW_H

// 회원가입 화면

#include <QWidget>

namespace Ui {
class signupwindow;
}

class signupwindow : public QWidget
{
    Q_OBJECT

public:
    explicit signupwindow(QWidget *parent = nullptr);
    ~signupwindow();

private:
    Ui::signupwindow *ui;
};

#endif // SIGNUPWINDOW_H
