#include "newframe_dialog.h"
#include "ui_newframe_dialog.h"

NewFrameDialog::NewFrameDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewFrameDialog)
{
    ui->setupUi(this);
}

NewFrameDialog::~NewFrameDialog()
{
    delete ui;
}
