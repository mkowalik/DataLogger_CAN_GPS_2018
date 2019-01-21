#include "filesdownload_dialog.h"
#include "ui_filesdownload_dialog.h"

FilesDownloadDialog::FilesDownloadDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FilesDownloadDialog)
{
    ui->setupUi(this);
}

void FilesDownloadDialog::updateProgressBar(int progressBarValue){
    ui->progressBar->setValue(progressBarValue);
}

void FilesDownloadDialog::addFileToList(QString fileName){
    ui->listWidget->addItem(fileName);
}

FilesDownloadDialog::~FilesDownloadDialog(){
    delete ui;
}

void FilesDownloadDialog::on_buttonBox_rejected(){
//    mThread->Cancel = true;
    //tu ubijamy watek mThread
    //TODO trzeba dobrze ubic watek
}
