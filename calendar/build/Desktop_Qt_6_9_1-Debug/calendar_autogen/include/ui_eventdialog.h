/********************************************************************************
** Form generated from reading UI file 'eventdialog.ui'
**
** Created by: Qt User Interface Compiler version 6.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_EVENTDIALOG_H
#define UI_EVENTDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>

QT_BEGIN_NAMESPACE

class Ui_eventDialog
{
public:

    void setupUi(QDialog *eventDialog)
    {
        if (eventDialog->objectName().isEmpty())
            eventDialog->setObjectName("eventDialog");
        eventDialog->resize(400, 300);

        retranslateUi(eventDialog);

        QMetaObject::connectSlotsByName(eventDialog);
    } // setupUi

    void retranslateUi(QDialog *eventDialog)
    {
        eventDialog->setWindowTitle(QCoreApplication::translate("eventDialog", "Dialog", nullptr));
    } // retranslateUi

};

namespace Ui {
    class eventDialog: public Ui_eventDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EVENTDIALOG_H
