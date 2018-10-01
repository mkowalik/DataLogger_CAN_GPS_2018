/********************************************************************************
** Form generated from reading UI file 'newframe_dialog.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NEWFRAME_DIALOG_H
#define UI_NEWFRAME_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_NewFrameDialog
{
public:
    QVBoxLayout *verticalLayout;
    QGridLayout *gridLayout;
    QLabel *label;
    QHBoxLayout *horizontalLayout;
    QLabel *label_2;
    QSpinBox *spinBox;
    QSpacerItem *horizontalSpacer;
    QLabel *label_3;
    QLineEdit *lineEdit;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *NewFrameDialog)
    {
        if (NewFrameDialog->objectName().isEmpty())
            NewFrameDialog->setObjectName(QStringLiteral("NewFrameDialog"));
        NewFrameDialog->resize(226, 97);
        verticalLayout = new QVBoxLayout(NewFrameDialog);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        label = new QLabel(NewFrameDialog);
        label->setObjectName(QStringLiteral("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label_2 = new QLabel(NewFrameDialog);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout->addWidget(label_2);

        spinBox = new QSpinBox(NewFrameDialog);
        spinBox->setObjectName(QStringLiteral("spinBox"));
        spinBox->setFrame(true);
        spinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
        spinBox->setProperty("showGroupSeparator", QVariant(false));
        spinBox->setPrefix(QStringLiteral(""));
        spinBox->setMaximum(2048);
        spinBox->setDisplayIntegerBase(16);

        horizontalLayout->addWidget(spinBox);


        gridLayout->addLayout(horizontalLayout, 0, 1, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 0, 2, 1, 1);

        label_3 = new QLabel(NewFrameDialog);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout->addWidget(label_3, 1, 0, 1, 1);

        lineEdit = new QLineEdit(NewFrameDialog);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));
        lineEdit->setMaxLength(20);

        gridLayout->addWidget(lineEdit, 1, 1, 1, 2);


        verticalLayout->addLayout(gridLayout);

        buttonBox = new QDialogButtonBox(NewFrameDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Apply|QDialogButtonBox::Discard);
        buttonBox->setCenterButtons(false);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(NewFrameDialog);
        QObject::connect(buttonBox, SIGNAL(rejected()), NewFrameDialog, SLOT(reject()));
        QObject::connect(buttonBox, SIGNAL(accepted()), NewFrameDialog, SLOT(accept()));

        QMetaObject::connectSlotsByName(NewFrameDialog);
    } // setupUi

    void retranslateUi(QDialog *NewFrameDialog)
    {
        NewFrameDialog->setWindowTitle(QApplication::translate("NewFrameDialog", "Dialog", nullptr));
        label->setText(QApplication::translate("NewFrameDialog", "ID:", nullptr));
        label_2->setText(QApplication::translate("NewFrameDialog", "0x", nullptr));
        spinBox->setSpecialValueText(QString());
        label_3->setText(QApplication::translate("NewFrameDialog", "Module name:", nullptr));
    } // retranslateUi

};

namespace Ui {
    class NewFrameDialog: public Ui_NewFrameDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NEWFRAME_DIALOG_H
