#include "canspeedwidget.h"
#include "ui_canspeedwidget.h"

CANSpeedWidget::CANSpeedWidget(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CANSpeedWidget)
{
    ui->setupUi(this);
}

CANSpeedWidget::~CANSpeedWidget()
{
    delete ui;
}


EUSB2CAN_CanSpeed CANSpeedWidget::getCANBitrate(){
    QString bitrateStr = ui->canSpeedComboBox->currentText();
    if (bitrateStr == "125kbps"){
        return EUSB2CAN_CS_125K;
    } else if (bitrateStr == "250kbps"){
        return EUSB2CAN_CS_250K;
    } else if (bitrateStr == "500kbps"){
        return EUSB2CAN_CS_500K;
    } else if (bitrateStr == "1Mbps"){
        return EUSB2CAN_CS_1M;
    } else {
        throw std::invalid_argument("Invalid value of CAN bitrate string.");
    }
}
