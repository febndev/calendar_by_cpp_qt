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
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AddCalendarDialog
{
public:
    QWidget *gridLayoutWidget;
    QGridLayout *gridLayout;
    QSpacerItem *verticalSpacer;
    QPushButton *btnCancel;
    QLabel *publicLabel;
    QCheckBox *cbPublic;
    QLabel *titleLabel;
    QPushButton *btnCreate;
    QSpacerItem *verticalSpacer_2;
    QLineEdit *leName;
    QSpacerItem *verticalSpacer_3;

    void setupUi(QDialog *AddCalendarDialog)
    {
        if (AddCalendarDialog->objectName().isEmpty())
            AddCalendarDialog->setObjectName("AddCalendarDialog");
        AddCalendarDialog->resize(400, 600);
        gridLayoutWidget = new QWidget(AddCalendarDialog);
        gridLayoutWidget->setObjectName("gridLayoutWidget");
        gridLayoutWidget->setGeometry(QRect(0, 220, 401, 161));
        gridLayout = new QGridLayout(gridLayoutWidget);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(0, 0, 0, 0);
        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        gridLayout->addItem(verticalSpacer, 1, 2, 1, 1);

        btnCancel = new QPushButton(gridLayoutWidget);
        btnCancel->setObjectName("btnCancel");

        gridLayout->addWidget(btnCancel, 2, 1, 1, 1);

        publicLabel = new QLabel(gridLayoutWidget);
        publicLabel->setObjectName("publicLabel");

        gridLayout->addWidget(publicLabel, 1, 1, 1, 1);

        cbPublic = new QCheckBox(gridLayoutWidget);
        cbPublic->setObjectName("cbPublic");

        gridLayout->addWidget(cbPublic, 1, 3, 1, 1);

        titleLabel = new QLabel(gridLayoutWidget);
        titleLabel->setObjectName("titleLabel");

        gridLayout->addWidget(titleLabel, 0, 1, 1, 1);

        btnCreate = new QPushButton(gridLayoutWidget);
        btnCreate->setObjectName("btnCreate");
        btnCreate->setAutoDefault(false);

        gridLayout->addWidget(btnCreate, 2, 3, 1, 1);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        gridLayout->addItem(verticalSpacer_2, 1, 0, 1, 1);

        leName = new QLineEdit(gridLayoutWidget);
        leName->setObjectName("leName");

        gridLayout->addWidget(leName, 0, 3, 1, 1);

        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        gridLayout->addItem(verticalSpacer_3, 1, 4, 1, 1);


        retranslateUi(AddCalendarDialog);

        btnCreate->setDefault(true);


        QMetaObject::connectSlotsByName(AddCalendarDialog);
    } // setupUi

    void retranslateUi(QDialog *AddCalendarDialog)
    {
        AddCalendarDialog->setWindowTitle(QCoreApplication::translate("AddCalendarDialog", "Dialog", nullptr));
        btnCancel->setText(QCoreApplication::translate("AddCalendarDialog", "\353\217\214\354\225\204\352\260\200\352\270\260", nullptr));
        publicLabel->setText(QCoreApplication::translate("AddCalendarDialog", "\354\272\230\353\246\260\353\215\224 \352\263\265\354\234\240 \354\227\254\353\266\200", nullptr));
        cbPublic->setText(QCoreApplication::translate("AddCalendarDialog", "\352\263\265\354\234\240\354\272\230\353\246\260\353\215\224", nullptr));
        titleLabel->setText(QCoreApplication::translate("AddCalendarDialog", "\354\272\230\353\246\260\353\215\224 \354\240\234\353\252\251", nullptr));
        btnCreate->setText(QCoreApplication::translate("AddCalendarDialog", "\354\272\230\353\246\260\353\215\224 \354\203\235\354\204\261\355\225\230\352\270\260", nullptr));
    } // retranslateUi

};

namespace Ui {
    class AddCalendarDialog: public Ui_AddCalendarDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ADDCALENDARDIALOG_H
