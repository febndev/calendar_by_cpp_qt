/********************************************************************************
** Form generated from reading UI file 'signupdialog.ui'
**
** Created by: Qt User Interface Compiler version 6.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SIGNUPDIALOG_H
#define UI_SIGNUPDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SignUpDialog
{
public:
    QWidget *layoutWidget;
    QGridLayout *gridLayout;
    QLabel *signIDLabel;
    QLineEdit *userEmailLineEdit;
    QLabel *useremail;
    QLineEdit *signpw2LineEdit;
    QLabel *signpwLabel;
    QLineEdit *usernameLineEdit;
    QLineEdit *signpwLineEdit;
    QLabel *username;
    QLineEdit *signIDLineEdit;
    QLabel *signpw2Label;
    QLabel *levelLabel;
    QComboBox *levelComboBox;
    QPushButton *cancelClicked;
    QPushButton *okButton;

    void setupUi(QDialog *SignUpDialog)
    {
        if (SignUpDialog->objectName().isEmpty())
            SignUpDialog->setObjectName("SignUpDialog");
        SignUpDialog->resize(400, 600);
        layoutWidget = new QWidget(SignUpDialog);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(40, 150, 321, 411));
        gridLayout = new QGridLayout(layoutWidget);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(0, 0, 0, 0);
        signIDLabel = new QLabel(layoutWidget);
        signIDLabel->setObjectName("signIDLabel");

        gridLayout->addWidget(signIDLabel, 0, 0, 1, 1);

        userEmailLineEdit = new QLineEdit(layoutWidget);
        userEmailLineEdit->setObjectName("userEmailLineEdit");

        gridLayout->addWidget(userEmailLineEdit, 4, 1, 1, 2);

        useremail = new QLabel(layoutWidget);
        useremail->setObjectName("useremail");

        gridLayout->addWidget(useremail, 4, 0, 1, 1);

        signpw2LineEdit = new QLineEdit(layoutWidget);
        signpw2LineEdit->setObjectName("signpw2LineEdit");

        gridLayout->addWidget(signpw2LineEdit, 2, 1, 1, 2);

        signpwLabel = new QLabel(layoutWidget);
        signpwLabel->setObjectName("signpwLabel");

        gridLayout->addWidget(signpwLabel, 1, 0, 1, 1);

        usernameLineEdit = new QLineEdit(layoutWidget);
        usernameLineEdit->setObjectName("usernameLineEdit");

        gridLayout->addWidget(usernameLineEdit, 3, 1, 1, 2);

        signpwLineEdit = new QLineEdit(layoutWidget);
        signpwLineEdit->setObjectName("signpwLineEdit");

        gridLayout->addWidget(signpwLineEdit, 1, 1, 1, 2);

        username = new QLabel(layoutWidget);
        username->setObjectName("username");

        gridLayout->addWidget(username, 3, 0, 1, 1);

        signIDLineEdit = new QLineEdit(layoutWidget);
        signIDLineEdit->setObjectName("signIDLineEdit");

        gridLayout->addWidget(signIDLineEdit, 0, 1, 1, 2);

        signpw2Label = new QLabel(layoutWidget);
        signpw2Label->setObjectName("signpw2Label");

        gridLayout->addWidget(signpw2Label, 2, 0, 1, 1);

        levelLabel = new QLabel(layoutWidget);
        levelLabel->setObjectName("levelLabel");

        gridLayout->addWidget(levelLabel, 5, 0, 1, 1);

        levelComboBox = new QComboBox(layoutWidget);
        levelComboBox->addItem(QString());
        levelComboBox->addItem(QString());
        levelComboBox->addItem(QString());
        levelComboBox->addItem(QString());
        levelComboBox->setObjectName("levelComboBox");

        gridLayout->addWidget(levelComboBox, 5, 1, 1, 2);

        cancelClicked = new QPushButton(layoutWidget);
        cancelClicked->setObjectName("cancelClicked");
        cancelClicked->setAutoDefault(false);

        gridLayout->addWidget(cancelClicked, 10, 0, 1, 1);

        okButton = new QPushButton(layoutWidget);
        okButton->setObjectName("okButton");
        okButton->setAutoDefault(false);

        gridLayout->addWidget(okButton, 10, 1, 1, 2);


        retranslateUi(SignUpDialog);

        okButton->setDefault(true);


        QMetaObject::connectSlotsByName(SignUpDialog);
    } // setupUi

    void retranslateUi(QDialog *SignUpDialog)
    {
        SignUpDialog->setWindowTitle(QCoreApplication::translate("SignUpDialog", "Dialog", nullptr));
        signIDLabel->setText(QCoreApplication::translate("SignUpDialog", "ID", nullptr));
        useremail->setText(QCoreApplication::translate("SignUpDialog", "E-Mail", nullptr));
        signpwLabel->setText(QCoreApplication::translate("SignUpDialog", "PW", nullptr));
        username->setText(QCoreApplication::translate("SignUpDialog", "\354\235\264\353\246\204", nullptr));
        signpw2Label->setText(QCoreApplication::translate("SignUpDialog", "PW \355\231\225\354\235\270", nullptr));
        levelLabel->setText(QCoreApplication::translate("SignUpDialog", "\354\247\201\352\270\211", nullptr));
        levelComboBox->setItemText(0, QCoreApplication::translate("SignUpDialog", "\354\202\254\354\233\220", nullptr));
        levelComboBox->setItemText(1, QCoreApplication::translate("SignUpDialog", "\353\214\200\353\246\254", nullptr));
        levelComboBox->setItemText(2, QCoreApplication::translate("SignUpDialog", "\352\263\274\354\236\245", nullptr));
        levelComboBox->setItemText(3, QCoreApplication::translate("SignUpDialog", "\353\266\200\354\236\245", nullptr));

        cancelClicked->setText(QCoreApplication::translate("SignUpDialog", "\353\217\214\354\225\204\352\260\200\352\270\260", nullptr));
        okButton->setText(QCoreApplication::translate("SignUpDialog", "\355\232\214\354\233\220\352\260\200\354\236\205", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SignUpDialog: public Ui_SignUpDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SIGNUPDIALOG_H
