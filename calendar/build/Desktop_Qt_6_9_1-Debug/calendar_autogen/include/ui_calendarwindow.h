/********************************************************************************
** Form generated from reading UI file 'calendarwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CALENDARWINDOW_H
#define UI_CALENDARWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_calendarwindow
{
public:

    void setupUi(QWidget *calendarwindow)
    {
        if (calendarwindow->objectName().isEmpty())
            calendarwindow->setObjectName("calendarwindow");
        calendarwindow->resize(400, 300);

        retranslateUi(calendarwindow);

        QMetaObject::connectSlotsByName(calendarwindow);
    } // setupUi

    void retranslateUi(QWidget *calendarwindow)
    {
        calendarwindow->setWindowTitle(QCoreApplication::translate("calendarwindow", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class calendarwindow: public Ui_calendarwindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CALENDARWINDOW_H
