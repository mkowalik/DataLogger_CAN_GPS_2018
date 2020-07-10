#ifndef SENDERTHREAD_H
#define SENDERTHREAD_H

#include <QWidget>
#include <QSpinBox>
#include <QCheckBox>
#include <QThread>
#include "EUSB2CAN_Class.h"


class SenderThread : public QThread
{

public:
    SenderThread(QObject *parent = 0);
    void init(QCheckBox* activate, QSpinBox* rpm, QSpinBox* clt, QDoubleSpinBox* batt, QSpinBox* fuel, QDoubleSpinBox* oil, QSpinBox* gear, EUSB2CAN_CanSpeed canBitrate);
    ~SenderThread();
protected:
    void run();

private:
    QCheckBox* activate;
    QSpinBox* rpm;
    QSpinBox* clt; QDoubleSpinBox* batt;
    QSpinBox* fuel;
    QDoubleSpinBox* oil;
    QSpinBox* gear;
    EUSB2CAN_Class canHandle;

};

#endif // SENDERTHREAD_H
