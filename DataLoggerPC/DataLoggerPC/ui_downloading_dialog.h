/********************************************************************************
** Form generated from reading UI file 'downloading_dialog.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DOWNLOADING_DIALOG_H
#define UI_DOWNLOADING_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>

QT_BEGIN_NAMESPACE

class Ui_DownloadingDialog
{
public:
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *DownloadingDialog)
    {
        if (DownloadingDialog->objectName().isEmpty())
            DownloadingDialog->setObjectName(QStringLiteral("DownloadingDialog"));
        DownloadingDialog->resize(400, 300);
        buttonBox = new QDialogButtonBox(DownloadingDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(30, 240, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        retranslateUi(DownloadingDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), DownloadingDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), DownloadingDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(DownloadingDialog);
    } // setupUi

    void retranslateUi(QDialog *DownloadingDialog)
    {
        DownloadingDialog->setWindowTitle(QApplication::translate("DownloadingDialog", "Dialog", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DownloadingDialog: public Ui_DownloadingDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DOWNLOADING_DIALOG_H
