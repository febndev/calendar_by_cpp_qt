/********************************************************************************
** Form generated from reading UI file 'addcalendardialog.ui'
**
** Created by: Qt User Interface Compiler version 6.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ADDCALENDARDIALOG_H
#define UI_ADDCALENDARDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>

QT_BEGIN_NAMESPACE

class Ui_AddCalendarDialog
{
public:

    void setupUi(QDialog *AddCalendarDialog)
    {
        if (AddCalendarDialog->objectName().isEmpty())
            AddCalendarDialog->setObjectName("AddCalendarDialog");
        AddCalendarDialog->resize(400, 300);

        retranslateUi(AddCalendarDialog);

        QMetaObject::connectSlotsByName(AddCalendarDialog);
    } // setupUi

    void retranslateUi(QDialog *AddCalendarDialog)
    {
        AddCalendarDialog->setWindowTitle(QCoreApplication::translate("AddCalendarDialog", "Dialog", nullptr));
    } // retranslateUi

};

namespace Ui {
    class AddCalendarDialog: public Ui_AddCalendarDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ADDCALENDARDIALOG_H
