/********************************************************************************
** Form generated from reading UI file 'downloadfiles_dialog.ui'
**
** Created by: Qt User Interface Compiler version 5.11.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DOWNLOADFILES_DIALOG_H
#define UI_DOWNLOADFILES_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListView>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_DownloadFilesDialog
{
public:
    QPushButton *buttonCancel;
    QLabel *label;
    QProgressBar *progressBar;
    QListView *listView;

    void setupUi(QDialog *DownloadFilesDialog)
    {
        if (DownloadFilesDialog->objectName().isEmpty())
            DownloadFilesDialog->setObjectName(QStringLiteral("DownloadFilesDialog"));
        DownloadFilesDialog->resize(400, 300);
        buttonCancel = new QPushButton(DownloadFilesDialog);
        buttonCancel->setObjectName(QStringLiteral("buttonCancel"));
        buttonCancel->setGeometry(QRect(300, 270, 75, 23));
        label = new QLabel(DownloadFilesDialog);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(20, 10, 221, 16));
        progressBar = new QProgressBar(DownloadFilesDialog);
        progressBar->setObjectName(QStringLiteral("progressBar"));
        progressBar->setGeometry(QRect(20, 50, 361, 21));
        progressBar->setValue(24);
        listView = new QListView(DownloadFilesDialog);
        listView->setObjectName(QStringLiteral("listView"));
        listView->setGeometry(QRect(20, 90, 361, 161));

        retranslateUi(DownloadFilesDialog);

        QMetaObject::connectSlotsByName(DownloadFilesDialog);
    } // setupUi

    void retranslateUi(QDialog *DownloadFilesDialog)
    {
        DownloadFilesDialog->setWindowTitle(QApplication::translate("DownloadFilesDialog", "Dialog", nullptr));
        buttonCancel->setText(QApplication::translate("DownloadFilesDialog", "Cancel", nullptr));
        label->setText(QApplication::translate("DownloadFilesDialog", "Downloading", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DownloadFilesDialog: public Ui_DownloadFilesDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DOWNLOADFILES_DIALOG_H
