/********************************************************************************
** Form generated from reading UI file 'widget.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_H
#define UI_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLCDNumber>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Widget
{
public:
    QGridLayout *gridLayout;
    QLabel *label;
    QSpinBox *spinBox_RPM;
    QSlider *sliderRPM;
    QLabel *label_2;
    QSpinBox *spinBox_CLT;
    QSlider *sliderCLT;
    QLabel *label_3;
    QDoubleSpinBox *doubleSpinBox_battVoltage;
    QSlider *sliderBatt;
    QLabel *label_4;
    QSpinBox *spinBox_FuelLevel;
    QSlider *sliderFuel;
    QLabel *label_5;
    QDoubleSpinBox *doubleSpinBox_oilPres;
    QSlider *sliderOil;
    QLabel *label_6;
    QSpinBox *spinBox_Gear;
    QLCDNumber *lcdGear;
    QSpacerItem *horizontalSpacer;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName(QStringLiteral("Widget"));
        Widget->resize(400, 345);
        gridLayout = new QGridLayout(Widget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        label = new QLabel(Widget);
        label->setObjectName(QStringLiteral("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        spinBox_RPM = new QSpinBox(Widget);
        spinBox_RPM->setObjectName(QStringLiteral("spinBox_RPM"));
        spinBox_RPM->setMaximum(15000);
        spinBox_RPM->setSingleStep(100);

        gridLayout->addWidget(spinBox_RPM, 0, 1, 1, 1);

        sliderRPM = new QSlider(Widget);
        sliderRPM->setObjectName(QStringLiteral("sliderRPM"));
        sliderRPM->setMaximum(15000);
        sliderRPM->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(sliderRPM, 0, 2, 1, 2);

        label_2 = new QLabel(Widget);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        spinBox_CLT = new QSpinBox(Widget);
        spinBox_CLT->setObjectName(QStringLiteral("spinBox_CLT"));
        spinBox_CLT->setMinimum(-40);
        spinBox_CLT->setMaximum(250);
        spinBox_CLT->setValue(30);

        gridLayout->addWidget(spinBox_CLT, 1, 1, 1, 1);

        sliderCLT = new QSlider(Widget);
        sliderCLT->setObjectName(QStringLiteral("sliderCLT"));
        sliderCLT->setMinimum(-40);
        sliderCLT->setMaximum(250);
        sliderCLT->setValue(30);
        sliderCLT->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(sliderCLT, 1, 2, 1, 2);

        label_3 = new QLabel(Widget);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout->addWidget(label_3, 2, 0, 1, 1);

        doubleSpinBox_battVoltage = new QDoubleSpinBox(Widget);
        doubleSpinBox_battVoltage->setObjectName(QStringLiteral("doubleSpinBox_battVoltage"));
        doubleSpinBox_battVoltage->setDecimals(1);
        doubleSpinBox_battVoltage->setMaximum(20);
        doubleSpinBox_battVoltage->setSingleStep(0.1);
        doubleSpinBox_battVoltage->setValue(10);

        gridLayout->addWidget(doubleSpinBox_battVoltage, 2, 1, 1, 1);

        sliderBatt = new QSlider(Widget);
        sliderBatt->setObjectName(QStringLiteral("sliderBatt"));
        sliderBatt->setMaximum(200);
        sliderBatt->setSingleStep(1);
        sliderBatt->setValue(100);
        sliderBatt->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(sliderBatt, 2, 2, 1, 2);

        label_4 = new QLabel(Widget);
        label_4->setObjectName(QStringLiteral("label_4"));

        gridLayout->addWidget(label_4, 3, 0, 1, 1);

        spinBox_FuelLevel = new QSpinBox(Widget);
        spinBox_FuelLevel->setObjectName(QStringLiteral("spinBox_FuelLevel"));
        spinBox_FuelLevel->setMaximum(100);
        spinBox_FuelLevel->setValue(50);

        gridLayout->addWidget(spinBox_FuelLevel, 3, 1, 1, 1);

        sliderFuel = new QSlider(Widget);
        sliderFuel->setObjectName(QStringLiteral("sliderFuel"));
        sliderFuel->setMaximum(100);
        sliderFuel->setValue(50);
        sliderFuel->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(sliderFuel, 3, 2, 1, 2);

        label_5 = new QLabel(Widget);
        label_5->setObjectName(QStringLiteral("label_5"));

        gridLayout->addWidget(label_5, 4, 0, 1, 1);

        doubleSpinBox_oilPres = new QDoubleSpinBox(Widget);
        doubleSpinBox_oilPres->setObjectName(QStringLiteral("doubleSpinBox_oilPres"));
        doubleSpinBox_oilPres->setDecimals(1);
        doubleSpinBox_oilPres->setMaximum(12);
        doubleSpinBox_oilPres->setSingleStep(0.1);
        doubleSpinBox_oilPres->setValue(2);

        gridLayout->addWidget(doubleSpinBox_oilPres, 4, 1, 1, 1);

        sliderOil = new QSlider(Widget);
        sliderOil->setObjectName(QStringLiteral("sliderOil"));
        sliderOil->setMaximum(120);
        sliderOil->setValue(20);
        sliderOil->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(sliderOil, 4, 2, 1, 2);

        label_6 = new QLabel(Widget);
        label_6->setObjectName(QStringLiteral("label_6"));

        gridLayout->addWidget(label_6, 5, 0, 1, 1);

        spinBox_Gear = new QSpinBox(Widget);
        spinBox_Gear->setObjectName(QStringLiteral("spinBox_Gear"));
        spinBox_Gear->setMaximum(5);

        gridLayout->addWidget(spinBox_Gear, 5, 1, 1, 1);

        lcdGear = new QLCDNumber(Widget);
        lcdGear->setObjectName(QStringLiteral("lcdGear"));
        lcdGear->setFrameShape(QFrame::NoFrame);
        lcdGear->setFrameShadow(QFrame::Plain);
        lcdGear->setLineWidth(1);
        lcdGear->setMidLineWidth(0);
        lcdGear->setSmallDecimalPoint(false);
        lcdGear->setSegmentStyle(QLCDNumber::Flat);

        gridLayout->addWidget(lcdGear, 5, 2, 1, 1);

        horizontalSpacer = new QSpacerItem(183, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 5, 3, 1, 1);

#ifndef QT_NO_SHORTCUT
        label->setBuddy(spinBox_RPM);
        label_2->setBuddy(spinBox_CLT);
        label_3->setBuddy(doubleSpinBox_battVoltage);
        label_4->setBuddy(spinBox_FuelLevel);
        label_5->setBuddy(doubleSpinBox_oilPres);
        label_6->setBuddy(spinBox_Gear);
#endif // QT_NO_SHORTCUT

        retranslateUi(Widget);
        QObject::connect(spinBox_RPM, SIGNAL(valueChanged(int)), sliderRPM, SLOT(setValue(int)));
        QObject::connect(spinBox_CLT, SIGNAL(valueChanged(int)), sliderCLT, SLOT(setValue(int)));
        QObject::connect(spinBox_FuelLevel, SIGNAL(valueChanged(int)), sliderFuel, SLOT(setValue(int)));
        QObject::connect(spinBox_Gear, SIGNAL(valueChanged(int)), lcdGear, SLOT(display(int)));
        QObject::connect(sliderRPM, SIGNAL(valueChanged(int)), spinBox_RPM, SLOT(setValue(int)));
        QObject::connect(sliderCLT, SIGNAL(valueChanged(int)), spinBox_CLT, SLOT(setValue(int)));
        QObject::connect(sliderFuel, SIGNAL(valueChanged(int)), spinBox_FuelLevel, SLOT(setValue(int)));

        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QApplication::translate("Widget", "Widget", nullptr));
        label->setText(QApplication::translate("Widget", "RPM", nullptr));
        label_2->setText(QApplication::translate("Widget", "CLT", nullptr));
        label_3->setText(QApplication::translate("Widget", "Batt Voltage", nullptr));
        label_4->setText(QApplication::translate("Widget", "Fuel Level", nullptr));
        label_5->setText(QApplication::translate("Widget", "Oil Pressure", nullptr));
        label_6->setText(QApplication::translate("Widget", "Gear", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
