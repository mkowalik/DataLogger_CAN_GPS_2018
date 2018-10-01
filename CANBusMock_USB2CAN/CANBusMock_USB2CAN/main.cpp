#include "widget.h"
#include <QApplication>
#include <QWidget>
#include <QSpinBox>
#include <QThread>
#include "EUSB2CAN_Class.h"
#include <iostream>

using namespace std;

class SenderThread : public QThread
{

public:
    SenderThread(QObject *parent = 0);
    void init(QSpinBox* rpm, QSpinBox* clt, QDoubleSpinBox* batt, QSpinBox* fuel, QDoubleSpinBox* oil, QSpinBox* gear, EUSB2CAN_Class* canHandle);

protected:
    void run();

private:
    QSpinBox* rpm;
    QSpinBox* clt; QDoubleSpinBox* batt;
    QSpinBox* fuel;
    QDoubleSpinBox* oil;
    QSpinBox* gear;
    EUSB2CAN_Class* canHandle;

};

void SenderThread::init(QSpinBox* rpm, QSpinBox* clt, QDoubleSpinBox* batt, QSpinBox* fuel, QDoubleSpinBox* oil, QSpinBox* gear, EUSB2CAN_Class* canHandle){
    this->rpm = rpm;
    this->clt = clt;
    this->batt = batt;
    this->fuel = fuel;
    this->oil = oil;
    this->gear = gear;
    this->canHandle = canHandle;
}

int counter = 0;

void SenderThread::run(){
    while (1){

        EUSB2CAN_CanMsg msg;
        EUSB2CAN_Status s;
        msg.ext_id = 0;
        msg.rtr = 0;
        msg.dlc = 8;

        if ((counter++)%5 == 0){
            msg.id = 0x600;
            msg.data[0] = msg.data[1] = msg.data[2] = msg.data[3] = msg.data[4] = msg.data[5] = msg.data[6] = msg.data[7] = 0;
            msg.data[1] = (((unsigned int)rpm->value()) >> 8) & 0xFF;
            msg.data[0] = ((unsigned int)rpm->value()) & 0xFF;

            s = canHandle->canWrite(&msg);
            s = canHandle->flush();

            msg.id = 0x601;
            msg.data[0] = msg.data[1] = msg.data[2] = msg.data[3] = msg.data[4] = msg.data[5] = msg.data[6] = msg.data[7] = 0;

            s = canHandle->canWrite(&msg);
            s = canHandle->flush();

            msg.id = 0x602;
            msg.data[0] = msg.data[1] = msg.data[2] = msg.data[3] = msg.data[4] = msg.data[5] = msg.data[6] = msg.data[7] = 0;
            msg.data[4] = ((unsigned int)(oil->value()*16)) & 0xFF;
            msg.data[7] = (((unsigned int)clt->value()) >> 8) & 0xFF;
            msg.data[6] = ((unsigned int)clt->value()) & 0xFF;

            s = canHandle->canWrite(&msg);
            s = canHandle->flush();

            msg.id = 0x603;
            msg.data[0] = msg.data[1] = msg.data[2] = msg.data[3] = msg.data[4] = msg.data[5] = msg.data[6] = msg.data[7] = 0;

            s = canHandle->canWrite(&msg);
            s = canHandle->flush();

            msg.id = 0x604;
            msg.data[0] = msg.data[1] = msg.data[2] = msg.data[3] = msg.data[4] = msg.data[5] = msg.data[6] = msg.data[7] = 0;
            msg.data[3] = (((unsigned int)(batt->value()*37)) >> 8) & 0xFF;
            msg.data[2] = ((unsigned int)(batt->value()*37)) & 0xFF;

    //        std::cout << (int)msg.data[2] << ";" << (int)msg.data[3] << std::endl;

            s = canHandle->canWrite(&msg);
            s = canHandle->flush();
        }
        msg.id = 0x300;
        msg.data[0] = msg.data[1] = msg.data[2] = msg.data[3] = msg.data[4] = msg.data[5] = msg.data[6] = msg.data[7] = 0;
        msg.data[0] = (123) & 0xFF;

        s = canHandle->canWrite(&msg);
        s = canHandle->flush();

        msg.id = 0x380;
        msg.data[0] = msg.data[1] = msg.data[2] = msg.data[3] = msg.data[4] = msg.data[5] = msg.data[6] = msg.data[7] = 0;
        msg.data[0] = (231) & 0xFF;

        s = canHandle->canWrite(&msg);
        s = canHandle->flush();

        msg.id = 0x080;
        msg.dlc = 2;
        msg.data[0] = msg.data[1] = msg.data[2] = msg.data[3] = msg.data[4] = msg.data[5] = msg.data[6] = msg.data[7] = 0;
        msg.data[0] = ((unsigned int)gear->value()) & 0xFF;

        s = canHandle->canWrite(&msg);
        s = canHandle->flush();

        msg.id = 0x400;
        msg.dlc = 1;
        msg.data[0] = msg.data[1] = msg.data[2] = msg.data[3] = msg.data[4] = msg.data[5] = msg.data[6] = msg.data[7] = 0;
        msg.data[0] = ((unsigned int)fuel->value()) & 0xFF;

        s = canHandle->canWrite(&msg);
        s = canHandle->flush();

        msg.id = 0x000;
        msg.dlc = 1;
        msg.data[0] = msg.data[1] = msg.data[2] = msg.data[3] = msg.data[4] = msg.data[5] = msg.data[6] = msg.data[7] = 0;
        msg.data[0] = 0xFF;

        s = canHandle->canWrite(&msg);
        s = canHandle->flush();

        msg.id = 0x001;
        msg.dlc = 1;
        msg.data[0] = msg.data[1] = msg.data[2] = msg.data[3] = msg.data[4] = msg.data[5] = msg.data[6] = msg.data[7] = 0;
        msg.data[0] = 0xFF;

        s = canHandle->canWrite(&msg);
        s = canHandle->flush();

        QThread::msleep(10);
    }
}

SenderThread::SenderThread(QObject *parent){
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();

    EUSB2CAN_Class canHandle;
    EUSB2CAN_Status canStatus = canHandle.initialize(EUSB2CAN_BR_1M);

    std::cout << "Init status: " << canStatus << std::endl;

    EUSB2CAN_CanMsg msg;
    EUSB2CAN_Timestamp ts;

//    int counter = 0;
//    while (1){
//        canStatus = canHandle.canRead(&msg, &ts);
//        std::cout << "status: " << canStatus << std::endl;
//        std::cout << msg.id << ": " << msg.data[0] << std::endl;

//        std::cout << "read msg:" << counter++ << std::endl;
//        QThread::msleep(100);

//        if (canStatus==0){
//            while (1);
//        }
//    }

    QSpinBox* rpm = w.findChild<QSpinBox*>("spinBox_RPM");
    QSpinBox* clt = w.findChild<QSpinBox*>("spinBox_CLT");
    QDoubleSpinBox* batt = w.findChild<QDoubleSpinBox*>("doubleSpinBox_battVoltage");
    QSpinBox* fuel = w.findChild<QSpinBox*>("spinBox_FuelLevel");
    QDoubleSpinBox* oil = w.findChild<QDoubleSpinBox*>("doubleSpinBox_oilPres");
    QSpinBox* gear = w.findChild<QSpinBox*>("spinBox_Gear");

    SenderThread* thr = new SenderThread();
    thr->init(rpm, clt, batt, fuel, oil, gear, &canHandle);
    thr->start();

    return a.exec();
}
