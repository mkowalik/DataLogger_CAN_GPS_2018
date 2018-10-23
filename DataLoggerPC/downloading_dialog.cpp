#include "downloading_dialog.h"
#include "ui_downloading_dialog.h"

DownloadingDialog::DownloadingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DownloadingDialog)
{
    ui->setupUi(this);
}

DownloadingDialog::~DownloadingDialog()
{
    delete ui;
}
