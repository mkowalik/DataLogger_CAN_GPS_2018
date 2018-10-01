#include "newchannel_dialog.h"
#include "ui_newchannel_dialog.h"

NewChannelDialog::NewChannelDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewChannelDialog)
{
    ui->setupUi(this);
}

NewChannelDialog::~NewChannelDialog()
{
    delete ui;
}
