#include "downloadingprogress_dialog.h"
#include "ui_downloadingprogress_dialog.h"
#include <QPushButton>

static const QString startingString = "Downloading and converting files...";
static const QString doneString = "CONVERTED!";

FilesDownloadDialog::FilesDownloadDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FilesDownloadDialog)
{
    ui->setupUi(this);
}

void FilesDownloadDialog::updateProgressBar(int progressBarValue){
    ui->progressBar->setValue(progressBarValue);
}

void FilesDownloadDialog::addFileToList(QString sourceFileName, QString destinationFileName){
    ui->filesListWidget->addItem(sourceFileName + " -> " + destinationFileName);
}

void FilesDownloadDialog::errorInLastFile(QString reasonDescription){
    ui->filesListWidget->item(ui->filesListWidget->count()-1)->setForeground(Qt::red);
    if (reasonDescription.length() > 0){
        QString text = ui->filesListWidget->item(ui->filesListWidget->count()-1)->text();
        text += "\r\n";
        text += reasonDescription;
        ui->filesListWidget->item(ui->filesListWidget->count()-1)->setText(text);
    }
}

void FilesDownloadDialog::downloadingComplete(){
    ui->downloadingLabel->setText(doneString);
    ui->closeCancelButtonBox->button(QDialogButtonBox::Close)->setEnabled(true);
    ui->closeCancelButtonBox->button(QDialogButtonBox::Cancel)->setEnabled(false);
}

void FilesDownloadDialog::convertingThreadStarted(){
    ui->progressBar->setValue(0);
    ui->downloadingLabel->setText(startingString);
    ui->closeCancelButtonBox->button(QDialogButtonBox::Close)->setEnabled(false);
    ui->closeCancelButtonBox->button(QDialogButtonBox::Cancel)->setEnabled(true);
    ui->filesListWidget->clear();
}

FilesDownloadDialog::~FilesDownloadDialog(){
    delete ui;
}
