/********************************************************************************
** Form generated from reading UI file 'configureloggersd_dialog.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONFIGURELOGGERSD_DIALOG_H
#define UI_CONFIGURELOGGERSD_DIALOG_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_ConfigureLoggerSDDialog
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *mainTitleLabel;
    QFrame *line;
    QLabel *downloadDataLabel;
    QGridLayout *gridLayout;
    QComboBox *selectOutputFileComboBox;
    QSpacerItem *horizontalSpacer_3;
    QLabel *prototypeFileLabel;
    QPushButton *resetButton;
    QLabel *warningLabel;
    QPushButton *selectOutputFileButton;
    QPushButton *selectPrototypeFileButton;
    QSpacerItem *horizontalSpacer_5;
    QLabel *selectOutputFileLabel_3;
    QComboBox *prototypeFileComboBox;
    QSpacerItem *horizontalSpacer;
    QSpacerItem *horizontalSpacer_2;
    QSpacerItem *horizontalSpacer_6;
    QFrame *line_2;
    QLabel *framesLabel;
    QTreeWidget *framesTreeWidget;
    QHBoxLayout *horizontalLayout;
    QPushButton *newFrameButton;
    QPushButton *addChannelButton;
    QSpacerItem *horizontalSpacer_12;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *saveConfigButton;
    QSpacerItem *horizontalSpacer_4;

    void setupUi(QDialog *ConfigureLoggerSDDialog)
    {
        if (ConfigureLoggerSDDialog->objectName().isEmpty())
            ConfigureLoggerSDDialog->setObjectName(QStringLiteral("ConfigureLoggerSDDialog"));
        ConfigureLoggerSDDialog->resize(603, 415);
        verticalLayout = new QVBoxLayout(ConfigureLoggerSDDialog);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        mainTitleLabel = new QLabel(ConfigureLoggerSDDialog);
        mainTitleLabel->setObjectName(QStringLiteral("mainTitleLabel"));
        QFont font;
        font.setPointSize(14);
        mainTitleLabel->setFont(font);

        verticalLayout->addWidget(mainTitleLabel);

        line = new QFrame(ConfigureLoggerSDDialog);
        line->setObjectName(QStringLiteral("line"));
        line->setMinimumSize(QSize(300, 0));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line);

        downloadDataLabel = new QLabel(ConfigureLoggerSDDialog);
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
        selectOutputFileComboBox = new QComboBox(ConfigureLoggerSDDialog);
        selectOutputFileComboBox->setObjectName(QStringLiteral("selectOutputFileComboBox"));
        selectOutputFileComboBox->setMinimumSize(QSize(250, 0));
        selectOutputFileComboBox->setEditable(true);

        gridLayout->addWidget(selectOutputFileComboBox, 0, 1, 1, 1);

        horizontalSpacer_3 = new QSpacerItem(128, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_3, 1, 0, 1, 1);

        prototypeFileLabel = new QLabel(ConfigureLoggerSDDialog);
        prototypeFileLabel->setObjectName(QStringLiteral("prototypeFileLabel"));

        gridLayout->addWidget(prototypeFileLabel, 2, 0, 1, 1);

        resetButton = new QPushButton(ConfigureLoggerSDDialog);
        resetButton->setObjectName(QStringLiteral("resetButton"));

        gridLayout->addWidget(resetButton, 2, 3, 1, 1);

        warningLabel = new QLabel(ConfigureLoggerSDDialog);
        warningLabel->setObjectName(QStringLiteral("warningLabel"));

        gridLayout->addWidget(warningLabel, 1, 1, 1, 3);

        selectOutputFileButton = new QPushButton(ConfigureLoggerSDDialog);
        selectOutputFileButton->setObjectName(QStringLiteral("selectOutputFileButton"));

        gridLayout->addWidget(selectOutputFileButton, 0, 2, 1, 1);

        selectPrototypeFileButton = new QPushButton(ConfigureLoggerSDDialog);
        selectPrototypeFileButton->setObjectName(QStringLiteral("selectPrototypeFileButton"));

        gridLayout->addWidget(selectPrototypeFileButton, 2, 2, 1, 1);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_5, 0, 3, 1, 1);

        selectOutputFileLabel_3 = new QLabel(ConfigureLoggerSDDialog);
        selectOutputFileLabel_3->setObjectName(QStringLiteral("selectOutputFileLabel_3"));

        gridLayout->addWidget(selectOutputFileLabel_3, 0, 0, 1, 1);

        prototypeFileComboBox = new QComboBox(ConfigureLoggerSDDialog);
        prototypeFileComboBox->setObjectName(QStringLiteral("prototypeFileComboBox"));
        prototypeFileComboBox->setMinimumSize(QSize(250, 0));
        prototypeFileComboBox->setEditable(true);

        gridLayout->addWidget(prototypeFileComboBox, 2, 1, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 2, 4, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_2, 1, 4, 1, 1);

        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_6, 0, 4, 1, 1);


        verticalLayout->addLayout(gridLayout);

        line_2 = new QFrame(ConfigureLoggerSDDialog);
        line_2->setObjectName(QStringLiteral("line_2"));
        line_2->setMinimumSize(QSize(300, 0));
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line_2);

        framesLabel = new QLabel(ConfigureLoggerSDDialog);
        framesLabel->setObjectName(QStringLiteral("framesLabel"));
        QFont font2;
        font2.setPointSize(10);
        font2.setBold(true);
        font2.setWeight(75);
        framesLabel->setFont(font2);
        framesLabel->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom));

        verticalLayout->addWidget(framesLabel);

        framesTreeWidget = new QTreeWidget(ConfigureLoggerSDDialog);
        framesTreeWidget->setObjectName(QStringLiteral("framesTreeWidget"));

        verticalLayout->addWidget(framesTreeWidget);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        newFrameButton = new QPushButton(ConfigureLoggerSDDialog);
        newFrameButton->setObjectName(QStringLiteral("newFrameButton"));

        horizontalLayout->addWidget(newFrameButton);

        addChannelButton = new QPushButton(ConfigureLoggerSDDialog);
        addChannelButton->setObjectName(QStringLiteral("addChannelButton"));

        horizontalLayout->addWidget(addChannelButton);

        horizontalSpacer_12 = new QSpacerItem(195, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_12);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        saveConfigButton = new QPushButton(ConfigureLoggerSDDialog);
        saveConfigButton->setObjectName(QStringLiteral("saveConfigButton"));

        horizontalLayout_2->addWidget(saveConfigButton);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_4);


        verticalLayout->addLayout(horizontalLayout_2);


        retranslateUi(ConfigureLoggerSDDialog);

        QMetaObject::connectSlotsByName(ConfigureLoggerSDDialog);
    } // setupUi

    void retranslateUi(QDialog *ConfigureLoggerSDDialog)
    {
        ConfigureLoggerSDDialog->setWindowTitle(QApplication::translate("ConfigureLoggerSDDialog", "Dialog", nullptr));
        mainTitleLabel->setText(QApplication::translate("ConfigureLoggerSDDialog", "AGH Racing Logger PC Software", nullptr));
        downloadDataLabel->setText(QApplication::translate("ConfigureLoggerSDDialog", "Configure Data Logger", nullptr));
        prototypeFileLabel->setText(QApplication::translate("ConfigureLoggerSDDialog", "Choose prototype file:", nullptr));
        resetButton->setText(QApplication::translate("ConfigureLoggerSDDialog", "Reset", nullptr));
        warningLabel->setText(QApplication::translate("ConfigureLoggerSDDialog", "<html><head/><body><p><span style=\" color:#ff0000;\">Because of alpha-release file must have name logger.aghconf</span></p></body></html>", nullptr));
        selectOutputFileButton->setText(QApplication::translate("ConfigureLoggerSDDialog", "Select File", nullptr));
        selectPrototypeFileButton->setText(QApplication::translate("ConfigureLoggerSDDialog", "Select File", nullptr));
        selectOutputFileLabel_3->setText(QApplication::translate("ConfigureLoggerSDDialog", "Select output file:", nullptr));
        framesLabel->setText(QApplication::translate("ConfigureLoggerSDDialog", "Frames:", nullptr));
        QTreeWidgetItem *___qtreewidgetitem = framesTreeWidget->headerItem();
        ___qtreewidgetitem->setText(5, QApplication::translate("ConfigureLoggerSDDialog", "Comment", nullptr));
        ___qtreewidgetitem->setText(4, QApplication::translate("ConfigureLoggerSDDialog", "Unit", nullptr));
        ___qtreewidgetitem->setText(3, QApplication::translate("ConfigureLoggerSDDialog", "Name", nullptr));
        ___qtreewidgetitem->setText(2, QApplication::translate("ConfigureLoggerSDDialog", "Offset", nullptr));
        ___qtreewidgetitem->setText(1, QApplication::translate("ConfigureLoggerSDDialog", "Divider", nullptr));
        ___qtreewidgetitem->setText(0, QApplication::translate("ConfigureLoggerSDDialog", "Multiplier", nullptr));
        newFrameButton->setText(QApplication::translate("ConfigureLoggerSDDialog", "Add New Frame", nullptr));
        addChannelButton->setText(QApplication::translate("ConfigureLoggerSDDialog", "Add channel to selected frame", nullptr));
        saveConfigButton->setText(QApplication::translate("ConfigureLoggerSDDialog", "Save config to .aghconf file", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ConfigureLoggerSDDialog: public Ui_ConfigureLoggerSDDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONFIGURELOGGERSD_DIALOG_H
