/********************************************************************************
** Form generated from reading UI file 'download_files_dialog.ui'
**
** Created by: Qt User Interface Compiler version 5.11.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DOWNLOAD_FILES_DIALOG_H
#define UI_DOWNLOAD_FILES_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QProgressBar>

QT_BEGIN_NAMESPACE

class Ui_download_files_dialog
{
public:
    QDialogButtonBox *buttonBox;
    QProgressBar *progressBar;

    void setupUi(QDialog *download_files_dialog)
    {
        if (download_files_dialog->objectName().isEmpty())
            download_files_dialog->setObjectName(QStringLiteral("download_files_dialog"));
        download_files_dialog->resize(400, 300);
        buttonBox = new QDialogButtonBox(download_files_dialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(30, 240, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        progressBar = new QProgressBar(download_files_dialog);
        progressBar->setObjectName(QStringLiteral("progressBar"));
        progressBar->setGeometry(QRect(30, 70, 301, 21));
        progressBar->setValue(24);

        retranslateUi(download_files_dialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), download_files_dialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), download_files_dialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(download_files_dialog);
    } // setupUi

    void retranslateUi(QDialog *download_files_dialog)
    {
        download_files_dialog->setWindowTitle(QApplication::translate("download_files_dialog", "Dialog", nullptr));
    } // retranslateUi

};

namespace Ui {
    class download_files_dialog: public Ui_download_files_dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DOWNLOAD_FILES_DIALOG_H
