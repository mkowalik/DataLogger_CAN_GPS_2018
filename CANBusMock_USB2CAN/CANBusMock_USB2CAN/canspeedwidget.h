#ifndef CANSPEEDWIDGET_H
#define CANSPEEDWIDGET_H

#include <QDialog>
#include "EUSB2CAN_Class.h"

namespace Ui {
class CANSpeedWidget;
}

class CANSpeedWidget : public QDialog
{
    Q_OBJECT

public:
    explicit CANSpeedWidget(QWidget *parent = nullptr);
    EUSB2CAN_CanSpeed getCANBitrate();
    ~CANSpeedWidget();

private:
    Ui::CANSpeedWidget *ui;
};

#endif // CANSPEEDWIDGET_H
