#include "newchannel_dialog.h"
#include "ui_newchannel_dialog.h"

#include <iostream>
#include <QMessageBox>

NewChannelDialog::NewChannelDialog(ConfigFrame& frame, ConfigChannel* pPreviousChannel, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewChannelDialog),
    frame(frame),
    pPreviousChannel(pPreviousChannel)
{
    ui->setupUi(this);
}

NewChannelDialog::NewChannelDialog(ValueType valueType, int multipilier, int divider, int offset,
                          QString channelName, QString unitName, QString comment,
                          ConfigFrame& frame, ConfigChannel* pPreviousChannel, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewChannelDialog),
    frame(frame),
    pPreviousChannel(pPreviousChannel)
{
    ui->setupUi(this);

    if (valueType.isOnOffType()) {
        ui->onOff_radioButton->setChecked(true);
    } else if (valueType.isFlagType()){
        ui->flag_radioButton->setChecked(true);
    } else if (valueType.isCustomType()){
        ui->custom_radioButton->setChecked(true);
    } else {
        ui->integer_radioButton->setChecked(true);
    }
    if (valueType.is16BitLength()){
        ui->bit16_checkBox->setChecked(true);
    }
    if (valueType.isSignedType()){
        ui->signed_checkBox->setChecked(true);
    }
    if (valueType.isBigEndianType()){
        ui->bigEndian_checkBox->setChecked(true);
    }

    ui->multiplier_spinBox->setValue(multipilier);
    ui->divider_spinBox->setValue(divider);
    ui->offset_spinBox->setValue(offset);
    ui->channelName_lineEdit->setText(channelName);
    ui->unit_lineEdit->setText(unitName);
    ui->comment_lineEdit->setText(comment);

}

bool NewChannelDialog::getIsSigned(){
    return ui->signed_checkBox->isChecked();
}

bool NewChannelDialog::getIs16Bit(){
    return ui->bit16_checkBox->isChecked();
}

bool NewChannelDialog::getIsBigEndian(){
    return ui->bigEndian_checkBox->isChecked();
}

bool NewChannelDialog::getIsInteger(){
    return ui->integer_radioButton->isChecked();
}

bool NewChannelDialog::getIsOnOff(){
    return ui->onOff_radioButton->isChecked();
}

bool NewChannelDialog::getIsFlag(){
    return ui->flag_radioButton->isChecked();
}

bool NewChannelDialog::getIsCustom(){
    return ui->custom_radioButton->isChecked();
}

int NewChannelDialog::getMultiplier(){
    return ui->multiplier_spinBox->value();
}

int NewChannelDialog::getDivider(){
    return ui->divider_spinBox->value();
}

int NewChannelDialog::getOffset(){
    return ui->offset_spinBox->value();
}

QString NewChannelDialog::getChannelName(){
    return ui->channelName_lineEdit->text();
}

QString NewChannelDialog::getUnit(){
    return ui->unit_lineEdit->text();
}

QString NewChannelDialog::getComment(){
    return ui->comment_lineEdit->text();
}

NewChannelDialog::~NewChannelDialog()
{
    delete ui;
}

void NewChannelDialog::on_buttonBox_accepted()
{
    int thisDLC = frame.get_DLC();
    thisDLC += ui->bit16_checkBox->isChecked() ? 2 : 1;
    if (pPreviousChannel != nullptr){
        thisDLC -= pPreviousChannel->get_DLC();
    }

    if (thisDLC <= 8){
        accept();
    } else {
        QMessageBox::warning(this, "Frame length error", "Frame must not have more than 8 bytes.");
    }

}
