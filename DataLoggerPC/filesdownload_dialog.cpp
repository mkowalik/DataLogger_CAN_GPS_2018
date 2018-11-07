#include "filesdownload_dialog.h"
#include "ui_filesdownload_dialog.h"

FilesDownloadDialog::FilesDownloadDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FilesDownloadDialog)
{
    ui->setupUi(this);
    mThread = new Thread(this);
    connect(mThread, SIGNAL(dataChanged(const int&, QString)),this,SLOT(onMessageSent(const int&, QString)));
    mThread->start();
}

void FilesDownloadDialog::onMessageSent(const int& message, QString name){
    ui->progressBar->setValue(message);
    ui->listWidget->addItem(name);
}

FilesDownloadDialog::~FilesDownloadDialog(){
    delete ui;
}

void FilesDownloadDialog::on_buttonBox_rejected(){
    mThread->Cancel = true;
    //tu ubijamy watek mThread
    //TODO trzeba dobrze ubic watek
}
