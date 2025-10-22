/********************************************************************************
** Form generated from reading UI file 'logo.ui'
**
** Created by: Qt User Interface Compiler version 6.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGO_H
#define UI_LOGO_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Logo
{
public:
    QLabel *logoLabel;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QLabel *titleLabel;
    QPushButton *startButton;

    void setupUi(QWidget *Logo)
    {
        if (Logo->objectName().isEmpty())
            Logo->setObjectName("Logo");
        Logo->resize(1200, 800);
        logoLabel = new QLabel(Logo);
        logoLabel->setObjectName("logoLabel");
        logoLabel->setGeometry(QRect(320, 140, 66, 18));
        layoutWidget = new QWidget(Logo);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(560, 660, 140, 78));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        titleLabel = new QLabel(layoutWidget);
        titleLabel->setObjectName("titleLabel");
        QFont font;
        font.setPointSize(20);
        font.setBold(true);
        font.setItalic(true);
        titleLabel->setFont(font);

        verticalLayout->addWidget(titleLabel);

        startButton = new QPushButton(layoutWidget);
        startButton->setObjectName("startButton");

        verticalLayout->addWidget(startButton);


        retranslateUi(Logo);

        QMetaObject::connectSlotsByName(Logo);
    } // setupUi

    void retranslateUi(QWidget *Logo)
    {
        Logo->setWindowTitle(QCoreApplication::translate("Logo", "Form", nullptr));
        logoLabel->setText(QCoreApplication::translate("Logo", "TextLabel", nullptr));
        titleLabel->setText(QCoreApplication::translate("Logo", "\353\213\254\353\240\244\353\235\274 \353\213\254\353\240\245", nullptr));
        startButton->setText(QCoreApplication::translate("Logo", "\354\213\234\354\236\221\355\225\230\352\270\260", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Logo: public Ui_Logo {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGO_H
