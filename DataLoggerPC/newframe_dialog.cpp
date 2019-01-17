#include "newframe_dialog.h"
#include "ui_newframe_dialog.h"

NewFrameDialog::NewFrameDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewFrameDialog)
{
    ui->setupUi(this);
}

NewFrameDialog::NewFrameDialog(int id, QString moduleName, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewFrameDialog)
{
    ui->setupUi(this);
    ui->id_SpinBox->setValue(id);
    ui->moduleName_lineEdit->setText(moduleName);
}

int NewFrameDialog::getID(){
    return ui->id_SpinBox->value();
}

QString NewFrameDialog::getModuleName(){
    return ui->moduleName_lineEdit->text();
}

NewFrameDialog::~NewFrameDialog()
{
    delete ui;
}
