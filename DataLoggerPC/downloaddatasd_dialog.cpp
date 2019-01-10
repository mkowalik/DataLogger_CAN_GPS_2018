#include "downloaddatasd_dialog.h"
#include "ui_downloaddatasd_dialog.h"
#include "filesdownload_dialog.h"

#include <QFileDialog>
#include <QDebug>
#include <QComboBox>
#include <QDir>

DownloadDataSDDialog::DownloadDataSDDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DownloadDataSDDialog)
{
    ui->setupUi(this);


}

DownloadDataSDDialog::~DownloadDataSDDialog()
{
    delete ui;
}

void DownloadDataSDDialog::on_openDataDirButton_clicked()
{
    QString dirPath = QFileDialog::getExistingDirectory(this, "Choose Data Directory");
    ui->dataDirComboBox->addItem(dirPath);
    ui->dataDirComboBox->setCurrentText(dirPath);

    QDir dataDir(dirPath);
    dataDir.setNameFilters(QStringList() << "*.aghlog");
    ui->dataFilesListWidget->addItems(dataDir.entryList(QDir::Files));
}

void DownloadDataSDDialog::on_openDestDirButton_clicked()
{
    QString dirPath = QFileDialog::getExistingDirectory(this, "Choose Destination Directory");

    ui->destinationDirComboBox->addItem(dirPath);
    ui->destinationDirComboBox->setCurrentText(dirPath);


}

void DownloadDataSDDialog::on_downloadAndConvertButton_clicked()
{
    filesDownloadDialog = new FilesDownloadDialog(this);
    filesDownloadDialog->exec();
}
