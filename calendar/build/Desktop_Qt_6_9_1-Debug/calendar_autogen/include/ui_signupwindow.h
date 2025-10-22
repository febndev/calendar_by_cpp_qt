/********************************************************************************
** Form generated from reading UI file 'signupwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SIGNUPWINDOW_H
#define UI_SIGNUPWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_signupwindow
{
public:

    void setupUi(QWidget *signupwindow)
    {
        if (signupwindow->objectName().isEmpty())
            signupwindow->setObjectName("signupwindow");
        signupwindow->resize(400, 300);

        retranslateUi(signupwindow);

        QMetaObject::connectSlotsByName(signupwindow);
    } // setupUi

    void retranslateUi(QWidget *signupwindow)
    {
        signupwindow->setWindowTitle(QCoreApplication::translate("signupwindow", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class signupwindow: public Ui_signupwindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SIGNUPWINDOW_H
