#include "downloaddatasd_dialog.h"
#include "ui_downloaddatasd_dialog.h"
#include "filesdownload_dialog.h"

#include <QFileDialog>
#include <QDebug>
#include <QComboBox>
#include <QDir>
#include <QMessageBox>

#include <iostream>

DownloadDataSDDialog::DownloadDataSDDialog(RawDataParser& rawDataParser, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DownloadDataSDDialog),
    rawDataParser(rawDataParser)
{
    ui->setupUi(this);
    convertFileThread = new ConvertFileThread(rawDataParser, this);
    filesDownloadDialog = new FilesDownloadDialog(this);

    connect(convertFileThread, &ConvertFileThread::actualProgress, filesDownloadDialog, &FilesDownloadDialog::updateProgressBar);
    connect(convertFileThread, SIGNAL(actualFileConverting(QString)), filesDownloadDialog, SLOT(addFileToList(QString)));
}

DownloadDataSDDialog::~DownloadDataSDDialog()
{
    delete ui;
    delete filesDownloadDialog;
    delete convertFileThread;
}

void DownloadDataSDDialog::on_openDataDirButton_clicked()
{
    /*QFileDialog dialog;
    dialog.setFileMode(QFileDialog::DirectoryOnly);
    dialog.setOption(QFileDialog::DontUseNativeDialog, true);
    dialog.setOption(QFileDialog::ShowDirsOnly, false);
    if (!dialog.exec()){
        return;
    }

    QString dirPath = dialog.directory().absolutePath();*/

    QString dirPath = QFileDialog::getExistingDirectory(this, "Choose Destination Directory");

    ui->dataDirLineEdit->setText(dirPath);

    sourcePathChanged(dirPath);
}

void DownloadDataSDDialog::sourcePathChanged(QString dirPath){

    QDir dataDir(dirPath);
    dataDir.setNameFilters(QStringList() << "*.aghlog");

    QStringList files = dataDir.entryList();

    if (files.isEmpty()){
        QMessageBox::information(this, "Empty folder", "Directory does not contain any .aghlog files.");
        return;
    }

    ui->dataFilesListWidget->clear();
    ui->dataFilesListWidget->addItems(files);

    this->sourceDir = dirPath;
}

void DownloadDataSDDialog::on_openDestDirButton_clicked()
{
    /*QFileDialog dialog;
    dialog.setFileMode(QFileDialog::DirectoryOnly);
    dialog.setOption(QFileDialog::DontUseNativeDialog, true);
    dialog.setOption(QFileDialog::ShowDirsOnly, false);
    if (!dialog.exec()){
        return;
    }

    QString dirPath = dialog.directory().absolutePath();

    QString dirPath = QFileDialog::getSaveFileName(this, "Choose Output File", "", "AGH Config (*.aghlog)");

    int position = dirPath.lastIndexOf(QRegExp("/.+..+"), -1);
    dirPath.remove(position, dirPath.length());*/
    QString dirPath = QFileDialog::getExistingDirectory(this, "Choose Destination Directory");

    ui->destinationDirComboBox->addItem(dirPath);
    ui->destinationDirComboBox->setCurrentText(dirPath);

}

void DownloadDataSDDialog::on_downloadAndConvertButton_clicked()
{
    QList<QFileInfo> filesList;
    for (QListWidgetItem* pItem: ui->dataFilesListWidget->selectedItems()){
        filesList.append(QFileInfo(this->sourceDir + "/" + pItem->text()));
    }

    convertFileThread->addFilesToConvert(filesList);

    QDir dataDir(ui->destinationDirComboBox->currentText());
    if (ui->destinationDirComboBox->currentText().isEmpty() || !dataDir.exists()){
        QMessageBox::warning(this, "Given directory not exists.", "Givent destination directory doeas not exists. Choose proper directory path to save files.");
        return;
    }

    convertFileThread->setDestinationDirectory(ui->destinationDirComboBox->currentText());
    convertFileThread->start();
    if (!filesDownloadDialog->exec()){
        convertFileThread->cancel();
        convertFileThread->wait(threadWaitTimeout);
    }

    convertFileThread->cancel();
}

void DownloadDataSDDialog::on_selectAllButton_clicked()
{
    ui->dataFilesListWidget->selectAll();
}

void DownloadDataSDDialog::on_deselectAll_clicked()
{
    ui->dataFilesListWidget->clearSelection();
}
