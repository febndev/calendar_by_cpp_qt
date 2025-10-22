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
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDateTimeEdit>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_EventDialog
{
public:
    QWidget *gridLayoutWidget;
    QGridLayout *gridLayout;
    QLabel *endLabel;
    QSpacerItem *verticalSpacer;
    QDateTimeEdit *dateTimeEdit_2;
    QLineEdit *edtTitle;
    QLabel *memoLabel;
    QSpacerItem *verticalSpacer_2;
    QSpacerItem *horizontalSpacer_2;
    QLabel *titleLabel;
    QPushButton *btnCancel;
    QSpacerItem *horizontalSpacer_3;
    QPushButton *btnSubmit;
    QComboBox *cmbCalendar;
    QPlainTextEdit *txtMemo;
    QSpacerItem *horizontalSpacer;
    QLabel *startLabel;
    QLabel *cmbCalLabel;
    QDateTimeEdit *dateTimeEdit;
    QSpacerItem *verticalSpacer_3;
    QLabel *lblHint;

    void setupUi(QDialog *EventDialog)
    {
        if (EventDialog->objectName().isEmpty())
            EventDialog->setObjectName("EventDialog");
        EventDialog->resize(400, 600);
        gridLayoutWidget = new QWidget(EventDialog);
        gridLayoutWidget->setObjectName("gridLayoutWidget");
        gridLayoutWidget->setGeometry(QRect(-1, 49, 401, 521));
        gridLayout = new QGridLayout(gridLayoutWidget);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(0, 0, 0, 0);
        endLabel = new QLabel(gridLayoutWidget);
        endLabel->setObjectName("endLabel");

        gridLayout->addWidget(endLabel, 3, 1, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        gridLayout->addItem(verticalSpacer, 6, 2, 1, 1);

        dateTimeEdit_2 = new QDateTimeEdit(gridLayoutWidget);
        dateTimeEdit_2->setObjectName("dateTimeEdit_2");
        dateTimeEdit_2->setMaximumDateTime(QDateTime(QDate(2100, 12, 31), QTime(23, 59, 59)));
        dateTimeEdit_2->setMinimumDateTime(QDateTime(QDate(1970, 9, 14), QTime(0, 0, 0)));
        dateTimeEdit_2->setCalendarPopup(true);

        gridLayout->addWidget(dateTimeEdit_2, 3, 3, 1, 1);

        edtTitle = new QLineEdit(gridLayoutWidget);
        edtTitle->setObjectName("edtTitle");

        gridLayout->addWidget(edtTitle, 1, 3, 1, 1);

        memoLabel = new QLabel(gridLayoutWidget);
        memoLabel->setObjectName("memoLabel");

        gridLayout->addWidget(memoLabel, 6, 1, 1, 1);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        gridLayout->addItem(verticalSpacer_2, 6, 0, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout->addItem(horizontalSpacer_2, 5, 1, 1, 1);

        titleLabel = new QLabel(gridLayoutWidget);
        titleLabel->setObjectName("titleLabel");

        gridLayout->addWidget(titleLabel, 1, 1, 1, 1);

        btnCancel = new QPushButton(gridLayoutWidget);
        btnCancel->setObjectName("btnCancel");
        btnCancel->setAutoDefault(false);

        gridLayout->addWidget(btnCancel, 10, 1, 1, 1);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout->addItem(horizontalSpacer_3, 9, 1, 1, 1);

        btnSubmit = new QPushButton(gridLayoutWidget);
        btnSubmit->setObjectName("btnSubmit");
        btnSubmit->setAutoDefault(false);

        gridLayout->addWidget(btnSubmit, 10, 3, 1, 1);

        cmbCalendar = new QComboBox(gridLayoutWidget);
        cmbCalendar->setObjectName("cmbCalendar");

        gridLayout->addWidget(cmbCalendar, 0, 3, 1, 1);

        txtMemo = new QPlainTextEdit(gridLayoutWidget);
        txtMemo->setObjectName("txtMemo");

        gridLayout->addWidget(txtMemo, 6, 3, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout->addItem(horizontalSpacer, 7, 1, 1, 1);

        startLabel = new QLabel(gridLayoutWidget);
        startLabel->setObjectName("startLabel");

        gridLayout->addWidget(startLabel, 2, 1, 1, 1);

        cmbCalLabel = new QLabel(gridLayoutWidget);
        cmbCalLabel->setObjectName("cmbCalLabel");

        gridLayout->addWidget(cmbCalLabel, 0, 1, 1, 1);

        dateTimeEdit = new QDateTimeEdit(gridLayoutWidget);
        dateTimeEdit->setObjectName("dateTimeEdit");
        dateTimeEdit->setDateTime(QDateTime(QDate(2001, 1, 1), QTime(0, 0, 1)));
        dateTimeEdit->setMaximumDateTime(QDateTime(QDate(2100, 12, 31), QTime(23, 59, 59)));
        dateTimeEdit->setMinimumDateTime(QDateTime(QDate(1970, 9, 14), QTime(0, 0, 0)));
        dateTimeEdit->setCalendarPopup(true);

        gridLayout->addWidget(dateTimeEdit, 2, 3, 1, 1);

        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        gridLayout->addItem(verticalSpacer_3, 6, 4, 1, 1);

        lblHint = new QLabel(gridLayoutWidget);
        lblHint->setObjectName("lblHint");

        gridLayout->addWidget(lblHint, 8, 1, 1, 3);


        retranslateUi(EventDialog);

        btnSubmit->setDefault(true);


        QMetaObject::connectSlotsByName(EventDialog);
    } // setupUi

    void retranslateUi(QDialog *EventDialog)
    {
        EventDialog->setWindowTitle(QCoreApplication::translate("EventDialog", "Dialog", nullptr));
        endLabel->setText(QCoreApplication::translate("EventDialog", "\354\242\205\353\243\214", nullptr));
        dateTimeEdit_2->setDisplayFormat(QCoreApplication::translate("EventDialog", "yyyy- MM -dd HH:mm", nullptr));
        memoLabel->setText(QCoreApplication::translate("EventDialog", "\353\251\224\353\252\250", nullptr));
        titleLabel->setText(QCoreApplication::translate("EventDialog", "\354\235\274\354\240\225 \354\240\234\353\252\251", nullptr));
        btnCancel->setText(QCoreApplication::translate("EventDialog", "\354\267\250\354\206\214", nullptr));
        btnSubmit->setText(QCoreApplication::translate("EventDialog", "\354\235\274\354\240\225 \354\266\224\352\260\200 / \354\210\230\354\240\225", nullptr));
        startLabel->setText(QCoreApplication::translate("EventDialog", "\354\213\234\354\236\221", nullptr));
        cmbCalLabel->setText(QCoreApplication::translate("EventDialog", "\354\272\230\353\246\260\353\215\224", nullptr));
        dateTimeEdit->setDisplayFormat(QCoreApplication::translate("EventDialog", "yyyy- MM -dd HH:mm", nullptr));
        lblHint->setText(QCoreApplication::translate("EventDialog", "TextLabel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class EventDialog: public Ui_EventDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EVENTDIALOG_H
