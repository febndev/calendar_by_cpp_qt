/********************************************************************************
** Form generated from reading UI file 'calendarwidget.ui'
**
** Created by: Qt User Interface Compiler version 6.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CALENDARWIDGET_H
#define UI_CALENDARWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CalendarWidget
{
public:

    void setupUi(QWidget *CalendarWidget)
    {
        if (CalendarWidget->objectName().isEmpty())
            CalendarWidget->setObjectName("CalendarWidget");
        CalendarWidget->resize(600, 400);

        retranslateUi(CalendarWidget);

        QMetaObject::connectSlotsByName(CalendarWidget);
    } // setupUi

    void retranslateUi(QWidget *CalendarWidget)
    {
        CalendarWidget->setWindowTitle(QCoreApplication::translate("CalendarWidget", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class CalendarWidget: public Ui_CalendarWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CALENDARWIDGET_H
