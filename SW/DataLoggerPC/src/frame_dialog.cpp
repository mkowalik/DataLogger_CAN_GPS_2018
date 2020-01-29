#include "frame_dialog.h"
#include "ui_frame_dialog.h"

#include <QMessageBox>

FrameDialog::FrameDialog(const Config& config, const ConfigFrame* _pFrame, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FrameDialog),
    config(config),
    pFrame(_pFrame)
{
    ui->setupUi(this);
    if (_pFrame != nullptr){
        ui->id_SpinBox->setValue(static_cast<int>(_pFrame->getFrameID()));
        ui->dlc_SpinBox->setValue(static_cast<int>(_pFrame->getExpextedDLC()));
        ui->moduleName_lineEdit->setText(QString::fromStdString(_pFrame->getFrameName()));
    }
}

unsigned int FrameDialog::getFrameID(){
    return static_cast<unsigned int>(ui->id_SpinBox->value());
}

unsigned int FrameDialog::getExpectedDLC(){
    return static_cast<unsigned int>(ui->dlc_SpinBox->value());
}

QString FrameDialog::getModuleName(){
    return ui->moduleName_lineEdit->text();
}

FrameDialog::~FrameDialog()
{
    delete ui;
}
