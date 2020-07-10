#include "widget.h"
#include "canspeedwidget.h"
#include <QApplication>
#include <QWidget>
#include <QSpinBox>
#include <QThread>
#include <iostream>

#include "EUSB2CAN_Class.h"
#include "senderthread.h"

using namespace std;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    CANSpeedWidget wCan;
    wCan.exec();
    EUSB2CAN_CanSpeed canBitrate = wCan.getCANBitrate();

    SenderThread* thr = new SenderThread();

    Widget w(thr);
    w.show();

    QCheckBox* activate = w.findChild<QCheckBox*>("activate_checkBox");
    QSpinBox* rpm = w.findChild<QSpinBox*>("spinBox_RPM");
    QSpinBox* clt = w.findChild<QSpinBox*>("spinBox_CLT");
    QDoubleSpinBox* batt = w.findChild<QDoubleSpinBox*>("doubleSpinBox_battVoltage");
    QSpinBox* fuel = w.findChild<QSpinBox*>("spinBox_FuelLevel");
    QDoubleSpinBox* oil = w.findChild<QDoubleSpinBox*>("doubleSpinBox_oilPres");
    QSpinBox* gear = w.findChild<QSpinBox*>("spinBox_Gear");

    thr->init(activate, rpm, clt, batt, fuel, oil, gear, canBitrate);
    thr->start();

    return a.exec();
}
