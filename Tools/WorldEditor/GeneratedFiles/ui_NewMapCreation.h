/********************************************************************************
** Form generated from reading UI file 'NewMapCreation.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NEWMAPCREATION_H
#define UI_NEWMAPCREATION_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_NewMapCreation
{
public:
    QPushButton *pushButton_Cancel;
    QPushButton *pushButton_OK;
    QLineEdit *widthEditor;
    QLineEdit *heightEditor;

    void setupUi(QWidget *NewMapCreation)
    {
        if (NewMapCreation->objectName().isEmpty())
            NewMapCreation->setObjectName(QStringLiteral("NewMapCreation"));
        NewMapCreation->resize(341, 224);
        pushButton_Cancel = new QPushButton(NewMapCreation);
        pushButton_Cancel->setObjectName(QStringLiteral("pushButton_Cancel"));
        pushButton_Cancel->setGeometry(QRect(230, 160, 93, 28));
        pushButton_OK = new QPushButton(NewMapCreation);
        pushButton_OK->setObjectName(QStringLiteral("pushButton_OK"));
        pushButton_OK->setGeometry(QRect(10, 160, 93, 28));
        widthEditor = new QLineEdit(NewMapCreation);
        widthEditor->setObjectName(QStringLiteral("widthEditor"));
        widthEditor->setGeometry(QRect(10, 30, 311, 21));
        heightEditor = new QLineEdit(NewMapCreation);
        heightEditor->setObjectName(QStringLiteral("heightEditor"));
        heightEditor->setGeometry(QRect(10, 80, 311, 21));
        heightEditor->setMaxLength(64);

        retranslateUi(NewMapCreation);
        QObject::connect(pushButton_OK, SIGNAL(clicked()), NewMapCreation, SLOT(OnOK()));
        QObject::connect(pushButton_Cancel, SIGNAL(clicked()), NewMapCreation, SLOT(OnCancel()));

        QMetaObject::connectSlotsByName(NewMapCreation);
    } // setupUi

    void retranslateUi(QWidget *NewMapCreation)
    {
        NewMapCreation->setWindowTitle(QApplication::translate("NewMapCreation", "NewMapCreation", nullptr));
        pushButton_Cancel->setText(QApplication::translate("NewMapCreation", "&Cancel", nullptr));
        pushButton_OK->setText(QApplication::translate("NewMapCreation", "&OK", nullptr));
        widthEditor->setPlaceholderText(QApplication::translate("NewMapCreation", "width:", nullptr));
        heightEditor->setPlaceholderText(QApplication::translate("NewMapCreation", "height:", nullptr));
    } // retranslateUi

};

namespace Ui {
    class NewMapCreation: public Ui_NewMapCreation {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NEWMAPCREATION_H
