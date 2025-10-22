/********************************************************************************
** Form generated from reading UI file 'logindialog.ui'
**
** Created by: Qt User Interface Compiler version 6.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGINDIALOG_H
#define UI_LOGINDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LoginDialog
{
public:
    QWidget *layoutWidget;
    QGridLayout *gridLayout;
    QPushButton *signupButton;
    QSpacerItem *horizontalSpacer_2;
    QLabel *pwLabel;
    QPushButton *loginButton;
    QLineEdit *idLineEdit;
    QSpacerItem *horizontalSpacer;
    QLabel *idLabel;
    QLineEdit *pwLineEdit;

    void setupUi(QDialog *LoginDialog)
    {
        if (LoginDialog->objectName().isEmpty())
            LoginDialog->setObjectName("LoginDialog");
        LoginDialog->resize(400, 600);
        layoutWidget = new QWidget(LoginDialog);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(20, 200, 351, 311));
        gridLayout = new QGridLayout(layoutWidget);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(0, 0, 0, 0);
        signupButton = new QPushButton(layoutWidget);
        signupButton->setObjectName("signupButton");
        signupButton->setAutoDefault(false);

        gridLayout->addWidget(signupButton, 7, 0, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout->addItem(horizontalSpacer_2, 4, 0, 1, 1);

        pwLabel = new QLabel(layoutWidget);
        pwLabel->setObjectName("pwLabel");

        gridLayout->addWidget(pwLabel, 2, 0, 1, 1);

        loginButton = new QPushButton(layoutWidget);
        loginButton->setObjectName("loginButton");
        loginButton->setAutoDefault(false);

        gridLayout->addWidget(loginButton, 6, 0, 1, 1);

        idLineEdit = new QLineEdit(layoutWidget);
        idLineEdit->setObjectName("idLineEdit");

        gridLayout->addWidget(idLineEdit, 1, 0, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout->addItem(horizontalSpacer, 5, 0, 1, 1);

        idLabel = new QLabel(layoutWidget);
        idLabel->setObjectName("idLabel");

        gridLayout->addWidget(idLabel, 0, 0, 1, 1);

        pwLineEdit = new QLineEdit(layoutWidget);
        pwLineEdit->setObjectName("pwLineEdit");

        gridLayout->addWidget(pwLineEdit, 3, 0, 1, 1);


        retranslateUi(LoginDialog);

        loginButton->setDefault(true);


        QMetaObject::connectSlotsByName(LoginDialog);
    } // setupUi

    void retranslateUi(QDialog *LoginDialog)
    {
        LoginDialog->setWindowTitle(QCoreApplication::translate("LoginDialog", "Dialog", nullptr));
        signupButton->setText(QCoreApplication::translate("LoginDialog", "\355\232\214\354\233\220\352\260\200\354\236\205", nullptr));
        pwLabel->setText(QCoreApplication::translate("LoginDialog", "PW", nullptr));
        loginButton->setText(QCoreApplication::translate("LoginDialog", "\353\241\234\352\267\270\354\235\270", nullptr));
        idLabel->setText(QCoreApplication::translate("LoginDialog", "ID", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LoginDialog: public Ui_LoginDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGINDIALOG_H
