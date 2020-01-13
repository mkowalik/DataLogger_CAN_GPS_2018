#include "trigger_dialog.h"
#include "ui_trigger_dialog.h"

TriggerDialog::TriggerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TriggerDialog)
{
    ui->setupUi(this);
}

TriggerDialog::~TriggerDialog()
{
    delete ui;
}
