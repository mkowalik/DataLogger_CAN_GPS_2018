#include "senderthread.h"

#include <iostream>

void SenderThread::init(QSpinBox* rpm, QSpinBox* clt, QDoubleSpinBox* batt, QSpinBox* fuel, QDoubleSpinBox* oil, QSpinBox* gear, EUSB2CAN_CanSpeed canBitrate){
    this->rpm = rpm;
    this->clt = clt;
    this->batt = batt;
    this->fuel = fuel;
    this->oil = oil;
    this->gear = gear;

    EUSB2CAN_Status canStatus = canHandle.initialize(canBitrate);

    std::cout << "Init status: " << canStatus << std::endl;
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
            msg.dlc = 8;
            msg.data[0] = msg.data[1] = msg.data[2] = msg.data[3] = msg.data[4] = msg.data[5] = msg.data[6] = msg.data[7] = 0;
            msg.data[1] = (((unsigned int)rpm->value()) >> 8) & 0xFF;
            msg.data[0] = ((unsigned int)rpm->value()) & 0xFF;

            s = canHandle.write(&msg);
            s = canHandle.flush();

            msg.id = 0x601;
            msg.dlc = 8;
            msg.data[0] = msg.data[1] = msg.data[2] = msg.data[3] = msg.data[4] = msg.data[5] = msg.data[6] = msg.data[7] = 0;

            s = canHandle.write(&msg);
            s = canHandle.flush();

            msg.id = 0x602;
            msg.dlc = 8;
            msg.data[0] = msg.data[1] = msg.data[2] = msg.data[3] = msg.data[4] = msg.data[5] = msg.data[6] = msg.data[7] = 0;
            msg.data[4] = ((unsigned int)(oil->value()*16)) & 0xFF;
            msg.data[7] = (((unsigned int)clt->value()) >> 8) & 0xFF;
            msg.data[6] = ((unsigned int)clt->value()) & 0xFF;

            s = canHandle.write(&msg);
            s = canHandle.flush();

            msg.id = 0x603;
            msg.dlc = 8;
            msg.data[0] = msg.data[1] = msg.data[2] = msg.data[3] = msg.data[4] = msg.data[5] = msg.data[6] = msg.data[7] = 0;

            s = canHandle.write(&msg);
            s = canHandle.flush();

            msg.id = 0x604;
            msg.dlc = 8;
            msg.data[0] = msg.data[1] = msg.data[2] = msg.data[3] = msg.data[4] = msg.data[5] = msg.data[6] = msg.data[7] = 0;
            msg.data[3] = (((unsigned int)(batt->value()*37)) >> 8) & 0xFF;
            msg.data[2] = ((unsigned int)(batt->value()*37)) & 0xFF;

    //        std::cout << (int)msg.data[2] << ";" << (int)msg.data[3] << std::endl;

            s = canHandle.write(&msg);
            s = canHandle.flush();
        }

        msg.id = 0x200;
        msg.dlc = 6;
        msg.data[0] = msg.data[1] = msg.data[2] = msg.data[3] = msg.data[4] = msg.data[5] = msg.data[6] = msg.data[7] = 0;
        msg.data[0] = (212) & 0xFF;

        s = canHandle.write(&msg);
        s = canHandle.flush();

        msg.id = 0x280;
        msg.dlc = 6;
        msg.data[0] = msg.data[1] = msg.data[2] = msg.data[3] = msg.data[4] = msg.data[5] = msg.data[6] = msg.data[7] = 0;
        msg.data[0] = (198) & 0xFF;

        s = canHandle.write(&msg);
        s = canHandle.flush();

        msg.id = 0x300;
        msg.dlc = 6;
        msg.data[0] = msg.data[1] = msg.data[2] = msg.data[3] = msg.data[4] = msg.data[5] = msg.data[6] = msg.data[7] = 0;
        msg.data[0] = (123) & 0xFF;

        s = canHandle.write(&msg);
        s = canHandle.flush();

        msg.id = 0x380;
        msg.dlc = 6;
        msg.data[0] = msg.data[1] = msg.data[2] = msg.data[3] = msg.data[4] = msg.data[5] = msg.data[6] = msg.data[7] = 0;
        msg.data[0] = (231) & 0xFF;

        s = canHandle.write(&msg);
        s = canHandle.flush();

        msg.id = 0x080;
        msg.dlc = 8;
        msg.data[0] = msg.data[1] = msg.data[2] = msg.data[3] = msg.data[4] = msg.data[5] = msg.data[6] = msg.data[7] = 0;
        msg.data[0] = ((unsigned int)gear->value()) & 0xFF;

        s = canHandle.write(&msg);
        s = canHandle.flush();

        msg.id = 0x400;
        msg.dlc = 1;
        msg.data[0] = msg.data[1] = msg.data[2] = msg.data[3] = msg.data[4] = msg.data[5] = msg.data[6] = msg.data[7] = 0;
        msg.data[0] = ((unsigned int)fuel->value()) & 0xFF;

        s = canHandle.write(&msg);
        s = canHandle.flush();

        msg.id = 0x000;
        msg.dlc = 1;
        msg.data[0] = msg.data[1] = msg.data[2] = msg.data[3] = msg.data[4] = msg.data[5] = msg.data[6] = msg.data[7] = 0;
        msg.data[0] = 0xFF;

        s = canHandle.write(&msg);
        s = canHandle.flush();

        msg.id = 0x001;
        msg.dlc = 1;
        msg.data[0] = msg.data[1] = msg.data[2] = msg.data[3] = msg.data[4] = msg.data[5] = msg.data[6] = msg.data[7] = 0;
        msg.data[0] = 0xFF;

        s = canHandle.write(&msg);
        s = canHandle.flush();

        QThread::msleep(10);
    }
}

SenderThread::SenderThread(QObject *parent){
}

SenderThread::~SenderThread(){
    canHandle.uninitialize();
}

