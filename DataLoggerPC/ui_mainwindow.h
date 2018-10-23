/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionExit;
    QAction *actionSave_As;
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QLabel *MainTitleLabel;
    QFrame *lineDownload;
    QLabel *downloadDataLabel;
    QHBoxLayout *horizontalLayout;
    QPushButton *downloadSDButton;
    QSpacerItem *horizontalSpacer_2;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *downloadCANButton;
    QSpacerItem *horizontalSpacer_3;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *downloadUSBButton;
    QSpacerItem *horizontalSpacer_4;
    QFrame *lineTitle;
    QLabel *ConfigureLoggerLabel;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *configureSDButton;
    QSpacerItem *horizontalSpacer_6;
    QHBoxLayout *horizontalLayout_5;
    QPushButton *configureCANButton;
    QSpacerItem *horizontalSpacer_7;
    QHBoxLayout *horizontalLayout_6;
    QPushButton *configureUSBButton;
    QSpacerItem *horizontalSpacer_8;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(509, 336);
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName(QStringLiteral("actionExit"));
        actionSave_As = new QAction(MainWindow);
        actionSave_As->setObjectName(QStringLiteral("actionSave_As"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        MainTitleLabel = new QLabel(centralWidget);
        MainTitleLabel->setObjectName(QStringLiteral("MainTitleLabel"));
        QFont font;
        font.setPointSize(14);
        MainTitleLabel->setFont(font);

        verticalLayout->addWidget(MainTitleLabel);

        lineDownload = new QFrame(centralWidget);
        lineDownload->setObjectName(QStringLiteral("lineDownload"));
        lineDownload->setMinimumSize(QSize(300, 0));
        lineDownload->setFrameShape(QFrame::HLine);
        lineDownload->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(lineDownload);

        downloadDataLabel = new QLabel(centralWidget);
        downloadDataLabel->setObjectName(QStringLiteral("downloadDataLabel"));
        QFont font1;
        font1.setPointSize(12);
        font1.setBold(true);
        font1.setWeight(75);
        downloadDataLabel->setFont(font1);
        downloadDataLabel->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom));

        verticalLayout->addWidget(downloadDataLabel);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        downloadSDButton = new QPushButton(centralWidget);
        downloadSDButton->setObjectName(QStringLiteral("downloadSDButton"));

        horizontalLayout->addWidget(downloadSDButton);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        downloadCANButton = new QPushButton(centralWidget);
        downloadCANButton->setObjectName(QStringLiteral("downloadCANButton"));
        downloadCANButton->setEnabled(false);

        horizontalLayout_2->addWidget(downloadCANButton);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_3);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        downloadUSBButton = new QPushButton(centralWidget);
        downloadUSBButton->setObjectName(QStringLiteral("downloadUSBButton"));
        downloadUSBButton->setEnabled(false);

        horizontalLayout_3->addWidget(downloadUSBButton);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_4);


        verticalLayout->addLayout(horizontalLayout_3);

        lineTitle = new QFrame(centralWidget);
        lineTitle->setObjectName(QStringLiteral("lineTitle"));
        lineTitle->setWindowModality(Qt::WindowModal);
        lineTitle->setEnabled(true);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(lineTitle->sizePolicy().hasHeightForWidth());
        lineTitle->setSizePolicy(sizePolicy);
        lineTitle->setMinimumSize(QSize(300, 0));
        lineTitle->setSizeIncrement(QSize(0, 0));
        lineTitle->setBaseSize(QSize(300, 0));
#ifndef QT_NO_STATUSTIP
        lineTitle->setStatusTip(QStringLiteral(""));
#endif // QT_NO_STATUSTIP
        lineTitle->setLayoutDirection(Qt::LeftToRight);
        lineTitle->setFrameShape(QFrame::HLine);
        lineTitle->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(lineTitle);

        ConfigureLoggerLabel = new QLabel(centralWidget);
        ConfigureLoggerLabel->setObjectName(QStringLiteral("ConfigureLoggerLabel"));
        ConfigureLoggerLabel->setFont(font1);

        verticalLayout->addWidget(ConfigureLoggerLabel);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        configureSDButton = new QPushButton(centralWidget);
        configureSDButton->setObjectName(QStringLiteral("configureSDButton"));

        horizontalLayout_4->addWidget(configureSDButton);

        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_6);


        verticalLayout->addLayout(horizontalLayout_4);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        configureCANButton = new QPushButton(centralWidget);
        configureCANButton->setObjectName(QStringLiteral("configureCANButton"));
        configureCANButton->setEnabled(false);

        horizontalLayout_5->addWidget(configureCANButton);

        horizontalSpacer_7 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_7);


        verticalLayout->addLayout(horizontalLayout_5);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setSpacing(6);
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        configureUSBButton = new QPushButton(centralWidget);
        configureUSBButton->setObjectName(QStringLiteral("configureUSBButton"));
        configureUSBButton->setEnabled(false);

        horizontalLayout_6->addWidget(configureUSBButton);

        horizontalSpacer_8 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_8);


        verticalLayout->addLayout(horizontalLayout_6);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 509, 21));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        MainWindow->setMenuBar(menuBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);
        QWidget::setTabOrder(downloadSDButton, downloadCANButton);
        QWidget::setTabOrder(downloadCANButton, downloadUSBButton);
        QWidget::setTabOrder(downloadUSBButton, configureSDButton);
        QWidget::setTabOrder(configureSDButton, configureCANButton);
        QWidget::setTabOrder(configureCANButton, configureUSBButton);

        menuBar->addAction(menuFile->menuAction());
        menuFile->addAction(actionExit);

        retranslateUi(MainWindow);
        QObject::connect(actionExit, SIGNAL(triggered()), MainWindow, SLOT(close()));

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", nullptr));
        actionExit->setText(QApplication::translate("MainWindow", "Exit", nullptr));
        actionSave_As->setText(QApplication::translate("MainWindow", "Save As", nullptr));
        MainTitleLabel->setText(QApplication::translate("MainWindow", "AGH Racing Logger PC Software", nullptr));
        downloadDataLabel->setText(QApplication::translate("MainWindow", "Download Data", nullptr));
        downloadSDButton->setText(QApplication::translate("MainWindow", "from MicroSD Card", nullptr));
        downloadCANButton->setText(QApplication::translate("MainWindow", "using CAN bus (USB2CAN)", nullptr));
        downloadUSBButton->setText(QApplication::translate("MainWindow", "using USB", nullptr));
        ConfigureLoggerLabel->setText(QApplication::translate("MainWindow", "Configure Logger", nullptr));
        configureSDButton->setText(QApplication::translate("MainWindow", "using MicroSD Config File", nullptr));
        configureCANButton->setText(QApplication::translate("MainWindow", "using CAN bus (USB2CAN)", nullptr));
        configureUSBButton->setText(QApplication::translate("MainWindow", "using USB", nullptr));
        menuFile->setTitle(QApplication::translate("MainWindow", "File", nullptr));
#ifndef QT_NO_ACCESSIBILITY
        statusBar->setAccessibleName(QString());
#endif // QT_NO_ACCESSIBILITY
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
