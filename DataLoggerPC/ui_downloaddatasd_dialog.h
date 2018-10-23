/********************************************************************************
** Form generated from reading UI file 'downloaddatasd_dialog.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DOWNLOADDATASD_DIALOG_H
#define UI_DOWNLOADDATASD_DIALOG_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_DownloadDataSDDialog
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *mainTitleLabel;
    QFrame *line_3;
    QLabel *downloadDataLabel;
    QGridLayout *gridLayout;
    QLabel *dataDirLabel;
    QComboBox *dataDirComboBox;
    QSpacerItem *horizontalSpacer_5;
    QLabel *destinationDirLabel;
    QComboBox *destinationDirComboBox;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *openDestDirButton;
    QPushButton *openDataDirButton;
    QFrame *line;
    QLabel *advancedLabel;
    QGridLayout *gridLayout_2;
    QLabel *outputDataLayoutLabel;
    QLabel *frequencyLabel;
    QComboBox *outputDataLayoutComboBox;
    QSpacerItem *horizontalSpacer_3;
    QSpacerItem *horizontalSpacer_6;
    QSpacerItem *horizontalSpacer_7;
    QSpinBox *frequencySinBox;
    QFrame *line_2;
    QLabel *filesLabel;
    QListWidget *dataFilesListWidget;
    QHBoxLayout *horizontalLayout;
    QPushButton *downloadAndConvertButton;
    QPushButton *downloadSelectedButton;
    QSpacerItem *horizontalSpacer_9;

    void setupUi(QDialog *DownloadDataSDDialog)
    {
        if (DownloadDataSDDialog->objectName().isEmpty())
            DownloadDataSDDialog->setObjectName(QStringLiteral("DownloadDataSDDialog"));
        DownloadDataSDDialog->resize(620, 520);
        verticalLayout = new QVBoxLayout(DownloadDataSDDialog);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        mainTitleLabel = new QLabel(DownloadDataSDDialog);
        mainTitleLabel->setObjectName(QStringLiteral("mainTitleLabel"));
        QFont font;
        font.setPointSize(14);
        mainTitleLabel->setFont(font);

        verticalLayout->addWidget(mainTitleLabel);

        line_3 = new QFrame(DownloadDataSDDialog);
        line_3->setObjectName(QStringLiteral("line_3"));
        line_3->setMinimumSize(QSize(300, 0));
        line_3->setFrameShape(QFrame::HLine);
        line_3->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line_3);

        downloadDataLabel = new QLabel(DownloadDataSDDialog);
        downloadDataLabel->setObjectName(QStringLiteral("downloadDataLabel"));
        QFont font1;
        font1.setPointSize(12);
        font1.setBold(true);
        font1.setWeight(75);
        downloadDataLabel->setFont(font1);
        downloadDataLabel->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom));

        verticalLayout->addWidget(downloadDataLabel);

        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        dataDirLabel = new QLabel(DownloadDataSDDialog);
        dataDirLabel->setObjectName(QStringLiteral("dataDirLabel"));

        gridLayout->addWidget(dataDirLabel, 0, 0, 1, 1);

        dataDirComboBox = new QComboBox(DownloadDataSDDialog);
        dataDirComboBox->setObjectName(QStringLiteral("dataDirComboBox"));
        dataDirComboBox->setMinimumSize(QSize(200, 0));
        dataDirComboBox->setEditable(true);

        gridLayout->addWidget(dataDirComboBox, 0, 1, 1, 1);

        horizontalSpacer_5 = new QSpacerItem(68, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_5, 0, 3, 1, 1);

        destinationDirLabel = new QLabel(DownloadDataSDDialog);
        destinationDirLabel->setObjectName(QStringLiteral("destinationDirLabel"));

        gridLayout->addWidget(destinationDirLabel, 1, 0, 1, 1);

        destinationDirComboBox = new QComboBox(DownloadDataSDDialog);
        destinationDirComboBox->setObjectName(QStringLiteral("destinationDirComboBox"));
        destinationDirComboBox->setEnabled(true);
        destinationDirComboBox->setMinimumSize(QSize(200, 0));
        destinationDirComboBox->setEditable(true);

        gridLayout->addWidget(destinationDirComboBox, 1, 1, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(68, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_2, 1, 3, 1, 1);

        openDestDirButton = new QPushButton(DownloadDataSDDialog);
        openDestDirButton->setObjectName(QStringLiteral("openDestDirButton"));

        gridLayout->addWidget(openDestDirButton, 1, 2, 1, 1);

        openDataDirButton = new QPushButton(DownloadDataSDDialog);
        openDataDirButton->setObjectName(QStringLiteral("openDataDirButton"));

        gridLayout->addWidget(openDataDirButton, 0, 2, 1, 1);


        verticalLayout->addLayout(gridLayout);

        line = new QFrame(DownloadDataSDDialog);
        line->setObjectName(QStringLiteral("line"));
        line->setMinimumSize(QSize(300, 0));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line);

        advancedLabel = new QLabel(DownloadDataSDDialog);
        advancedLabel->setObjectName(QStringLiteral("advancedLabel"));
        QFont font2;
        font2.setPointSize(10);
        font2.setBold(true);
        font2.setWeight(75);
        advancedLabel->setFont(font2);
        advancedLabel->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom));

        verticalLayout->addWidget(advancedLabel);

        gridLayout_2 = new QGridLayout();
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        outputDataLayoutLabel = new QLabel(DownloadDataSDDialog);
        outputDataLayoutLabel->setObjectName(QStringLiteral("outputDataLayoutLabel"));

        gridLayout_2->addWidget(outputDataLayoutLabel, 0, 0, 1, 1);

        frequencyLabel = new QLabel(DownloadDataSDDialog);
        frequencyLabel->setObjectName(QStringLiteral("frequencyLabel"));

        gridLayout_2->addWidget(frequencyLabel, 1, 0, 1, 1);

        outputDataLayoutComboBox = new QComboBox(DownloadDataSDDialog);
        outputDataLayoutComboBox->addItem(QString());
        outputDataLayoutComboBox->addItem(QString());
        outputDataLayoutComboBox->setObjectName(QStringLiteral("outputDataLayoutComboBox"));
        outputDataLayoutComboBox->setEnabled(false);

        gridLayout_2->addWidget(outputDataLayoutComboBox, 0, 1, 1, 2);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer_3, 0, 3, 1, 1);

        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer_6, 1, 2, 1, 1);

        horizontalSpacer_7 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer_7, 1, 3, 1, 1);

        frequencySinBox = new QSpinBox(DownloadDataSDDialog);
        frequencySinBox->setObjectName(QStringLiteral("frequencySinBox"));
        frequencySinBox->setEnabled(false);
        frequencySinBox->setMinimum(10);
        frequencySinBox->setMaximum(1000);
        frequencySinBox->setSingleStep(100);
        frequencySinBox->setValue(1000);

        gridLayout_2->addWidget(frequencySinBox, 1, 1, 1, 1);


        verticalLayout->addLayout(gridLayout_2);

        line_2 = new QFrame(DownloadDataSDDialog);
        line_2->setObjectName(QStringLiteral("line_2"));
        line_2->setMinimumSize(QSize(300, 0));
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line_2);

        filesLabel = new QLabel(DownloadDataSDDialog);
        filesLabel->setObjectName(QStringLiteral("filesLabel"));
        filesLabel->setFont(font2);
        filesLabel->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom));

        verticalLayout->addWidget(filesLabel);

        dataFilesListWidget = new QListWidget(DownloadDataSDDialog);
        dataFilesListWidget->setObjectName(QStringLiteral("dataFilesListWidget"));
        dataFilesListWidget->setSelectionMode(QAbstractItemView::MultiSelection);

        verticalLayout->addWidget(dataFilesListWidget);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        downloadAndConvertButton = new QPushButton(DownloadDataSDDialog);
        downloadAndConvertButton->setObjectName(QStringLiteral("downloadAndConvertButton"));

        horizontalLayout->addWidget(downloadAndConvertButton);

        downloadSelectedButton = new QPushButton(DownloadDataSDDialog);
        downloadSelectedButton->setObjectName(QStringLiteral("downloadSelectedButton"));

        horizontalLayout->addWidget(downloadSelectedButton);

        horizontalSpacer_9 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_9);


        verticalLayout->addLayout(horizontalLayout);

#ifndef QT_NO_SHORTCUT
        dataDirLabel->setBuddy(dataDirComboBox);
        destinationDirLabel->setBuddy(destinationDirComboBox);
        outputDataLayoutLabel->setBuddy(outputDataLayoutComboBox);
        frequencyLabel->setBuddy(frequencySinBox);
#endif // QT_NO_SHORTCUT
        QWidget::setTabOrder(dataDirComboBox, destinationDirComboBox);
        QWidget::setTabOrder(destinationDirComboBox, outputDataLayoutComboBox);
        QWidget::setTabOrder(outputDataLayoutComboBox, frequencySinBox);

        retranslateUi(DownloadDataSDDialog);

        QMetaObject::connectSlotsByName(DownloadDataSDDialog);
    } // setupUi

    void retranslateUi(QDialog *DownloadDataSDDialog)
    {
        DownloadDataSDDialog->setWindowTitle(QApplication::translate("DownloadDataSDDialog", "Dialog", nullptr));
        mainTitleLabel->setText(QApplication::translate("DownloadDataSDDialog", "AGH Racing Logger PC Software", nullptr));
        downloadDataLabel->setText(QApplication::translate("DownloadDataSDDialog", "Download Data", nullptr));
        dataDirLabel->setText(QApplication::translate("DownloadDataSDDialog", "Select Data Directory:", nullptr));
        destinationDirLabel->setText(QApplication::translate("DownloadDataSDDialog", "Destination Directory:", nullptr));
        openDestDirButton->setText(QApplication::translate("DownloadDataSDDialog", "Open Directory", nullptr));
        openDataDirButton->setText(QApplication::translate("DownloadDataSDDialog", "Open Directory", nullptr));
        advancedLabel->setText(QApplication::translate("DownloadDataSDDialog", "Advanced:", nullptr));
        outputDataLayoutLabel->setText(QApplication::translate("DownloadDataSDDialog", "Output data layout:", nullptr));
        frequencyLabel->setText(QApplication::translate("DownloadDataSDDialog", "Frequency", nullptr));
        outputDataLayoutComboBox->setItemText(0, QApplication::translate("DownloadDataSDDialog", "Event Timing", nullptr));
        outputDataLayoutComboBox->setItemText(1, QApplication::translate("DownloadDataSDDialog", "Set Frequency Timing", nullptr));

        frequencySinBox->setSuffix(QString());
        filesLabel->setText(QApplication::translate("DownloadDataSDDialog", "Files:", nullptr));
        downloadAndConvertButton->setText(QApplication::translate("DownloadDataSDDialog", "Download and Convert Selected", nullptr));
        downloadSelectedButton->setText(QApplication::translate("DownloadDataSDDialog", "Download Selected", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DownloadDataSDDialog: public Ui_DownloadDataSDDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DOWNLOADDATASD_DIALOG_H
