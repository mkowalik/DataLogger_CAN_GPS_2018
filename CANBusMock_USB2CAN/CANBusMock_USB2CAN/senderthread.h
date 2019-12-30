#ifndef SENDERTHREAD_H
#define SENDERTHREAD_H

#include <QWidget>
#include <QSpinBox>
#include <QThread>
#include "EUSB2CAN_Class.h"


class SenderThread : public QThread
{

public:
    SenderThread(QObject *parent = 0);
    void init(QSpinBox* rpm, QSpinBox* clt, QDoubleSpinBox* batt, QSpinBox* fuel, QDoubleSpinBox* oil, QSpinBox* gear, EUSB2CAN_CanSpeed canBitrate);
    ~SenderThread();
protected:
    void run();

private:
    QSpinBox* rpm;
    QSpinBox* clt; QDoubleSpinBox* batt;
    QSpinBox* fuel;
    QDoubleSpinBox* oil;
    QSpinBox* gear;
    EUSB2CAN_Class canHandle;

};

#endif // SENDERTHREAD_H
