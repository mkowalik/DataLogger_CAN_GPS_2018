#include "downloadfiles_dialog.h"
#include "ui_downloadfiles_dialog.h"

DownloadFilesDialog::DownloadFilesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DownloadFilesDialog)
{
    ui->setupUi(this);
    mThread = new Thread(this);
    connect(mThread, SIGNAL(Changed()),this,SLOT(onChanged));
    mThread->start();
}

DownloadFilesDialog::~DownloadFilesDialog()
{
    delete ui;
}


void DownloadFilesDialog::onChanged(){
    QString text("Threadd");
    ui->label->setText(text);
}

void DownloadFilesDialog::on_buttonCancel_clicked()
{
    mThread->Cancel = true;
}
