/********************************************************************************
** Form generated from reading UI file 'newchannel_dialog.ui'
**
** Created by: Qt User Interface Compiler version 5.11.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NEWCHANNEL_DIALOG_H
#define UI_NEWCHANNEL_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_NewChannelDialog
{
public:
    QVBoxLayout *verticalLayout_2;
    QGridLayout *gridLayout;
    QLabel *label;
    QLineEdit *lineEdit;
    QVBoxLayout *verticalLayout;
    QCheckBox *checkBox;
    QCheckBox *checkBox_2;
    QRadioButton *radioButton_4;
    QRadioButton *radioButton;
    QRadioButton *radioButton_2;
    QRadioButton *radioButton_3;
    QLabel *label_2;
    QLineEdit *lineEdit_2;
    QLabel *label_3;
    QLineEdit *lineEdit_3;
    QLabel *label_4;
    QSpinBox *spinBox;
    QLabel *label_5;
    QSpinBox *spinBox_2;
    QLabel *label_6;
    QSpinBox *spinBox_3;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *NewChannelDialog)
    {
        if (NewChannelDialog->objectName().isEmpty())
            NewChannelDialog->setObjectName(QStringLiteral("NewChannelDialog"));
        NewChannelDialog->resize(249, 309);
        verticalLayout_2 = new QVBoxLayout(NewChannelDialog);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        label = new QLabel(NewChannelDialog);
        label->setObjectName(QStringLiteral("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        lineEdit = new QLineEdit(NewChannelDialog);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));

        gridLayout->addWidget(lineEdit, 0, 1, 1, 1);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        checkBox = new QCheckBox(NewChannelDialog);
        checkBox->setObjectName(QStringLiteral("checkBox"));

        verticalLayout->addWidget(checkBox);

        checkBox_2 = new QCheckBox(NewChannelDialog);
        checkBox_2->setObjectName(QStringLiteral("checkBox_2"));

        verticalLayout->addWidget(checkBox_2);

        radioButton_4 = new QRadioButton(NewChannelDialog);
        radioButton_4->setObjectName(QStringLiteral("radioButton_4"));
        radioButton_4->setChecked(true);

        verticalLayout->addWidget(radioButton_4);

        radioButton = new QRadioButton(NewChannelDialog);
        radioButton->setObjectName(QStringLiteral("radioButton"));

        verticalLayout->addWidget(radioButton);

        radioButton_2 = new QRadioButton(NewChannelDialog);
        radioButton_2->setObjectName(QStringLiteral("radioButton_2"));

        verticalLayout->addWidget(radioButton_2);

        radioButton_3 = new QRadioButton(NewChannelDialog);
        radioButton_3->setObjectName(QStringLiteral("radioButton_3"));

        verticalLayout->addWidget(radioButton_3);


        gridLayout->addLayout(verticalLayout, 1, 0, 1, 1);

        label_2 = new QLabel(NewChannelDialog);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout->addWidget(label_2, 2, 0, 1, 1);

        lineEdit_2 = new QLineEdit(NewChannelDialog);
        lineEdit_2->setObjectName(QStringLiteral("lineEdit_2"));

        gridLayout->addWidget(lineEdit_2, 2, 1, 1, 1);

        label_3 = new QLabel(NewChannelDialog);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout->addWidget(label_3, 3, 0, 1, 1);

        lineEdit_3 = new QLineEdit(NewChannelDialog);
        lineEdit_3->setObjectName(QStringLiteral("lineEdit_3"));

        gridLayout->addWidget(lineEdit_3, 3, 1, 1, 1);

        label_4 = new QLabel(NewChannelDialog);
        label_4->setObjectName(QStringLiteral("label_4"));

        gridLayout->addWidget(label_4, 4, 0, 1, 1);

        spinBox = new QSpinBox(NewChannelDialog);
        spinBox->setObjectName(QStringLiteral("spinBox"));

        gridLayout->addWidget(spinBox, 4, 1, 1, 1);

        label_5 = new QLabel(NewChannelDialog);
        label_5->setObjectName(QStringLiteral("label_5"));

        gridLayout->addWidget(label_5, 5, 0, 1, 1);

        spinBox_2 = new QSpinBox(NewChannelDialog);
        spinBox_2->setObjectName(QStringLiteral("spinBox_2"));

        gridLayout->addWidget(spinBox_2, 5, 1, 1, 1);

        label_6 = new QLabel(NewChannelDialog);
        label_6->setObjectName(QStringLiteral("label_6"));

        gridLayout->addWidget(label_6, 6, 0, 1, 1);

        spinBox_3 = new QSpinBox(NewChannelDialog);
        spinBox_3->setObjectName(QStringLiteral("spinBox_3"));

        gridLayout->addWidget(spinBox_3, 6, 1, 1, 1);


        verticalLayout_2->addLayout(gridLayout);

        buttonBox = new QDialogButtonBox(NewChannelDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Apply|QDialogButtonBox::Discard);

        verticalLayout_2->addWidget(buttonBox);


        retranslateUi(NewChannelDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), NewChannelDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), NewChannelDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(NewChannelDialog);
    } // setupUi

    void retranslateUi(QDialog *NewChannelDialog)
    {
        NewChannelDialog->setWindowTitle(QApplication::translate("NewChannelDialog", "Dialog", nullptr));
        label->setText(QApplication::translate("NewChannelDialog", "Channel Name:", nullptr));
        checkBox->setText(QApplication::translate("NewChannelDialog", "signed", nullptr));
        checkBox_2->setText(QApplication::translate("NewChannelDialog", "16-bit", nullptr));
        radioButton_4->setText(QApplication::translate("NewChannelDialog", "Integer Value", nullptr));
        radioButton->setText(QApplication::translate("NewChannelDialog", "ON-OFF", nullptr));
        radioButton_2->setText(QApplication::translate("NewChannelDialog", "FLAG", nullptr));
        radioButton_3->setText(QApplication::translate("NewChannelDialog", "Custom", nullptr));
        label_2->setText(QApplication::translate("NewChannelDialog", "Unit:", nullptr));
        label_3->setText(QApplication::translate("NewChannelDialog", "Comment:", nullptr));
        label_4->setText(QApplication::translate("NewChannelDialog", "Multiplier:", nullptr));
        label_5->setText(QApplication::translate("NewChannelDialog", "Divider:", nullptr));
        label_6->setText(QApplication::translate("NewChannelDialog", "Offset:", nullptr));
    } // retranslateUi

};

namespace Ui {
    class NewChannelDialog: public Ui_NewChannelDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NEWCHANNEL_DIALOG_H
