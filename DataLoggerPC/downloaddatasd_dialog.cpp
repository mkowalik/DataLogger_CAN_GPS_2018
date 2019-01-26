#include "downloaddatasd_dialog.h"
#include "ui_downloaddatasd_dialog.h"
#include "downloadingprogress_dialog.h"

#include <QFileDialog>
#include <QDebug>
#include <QComboBox>
#include <QDir>
#include <QMessageBox>

#include <iostream>

const QStringList DownloadDataSDDialog::outputDataLayoutOptionsString = {"Event Timing Mode",
                                                                         "Static Period - 10Hz Mode",
                                                                         "Static Period - 100Hz Mode",
                                                                         "Static Period - 250Hz Mode",
                                                                         "Static Period - 500Hz Mode",
                                                                         "Static Period - 1000Hz Mode"};
const QList<WritableToCSV::FileTimingMode> DownloadDataSDDialog::outputDataLayoutOptionsTimingMode = {WritableToCSV::EventMode,
                                                                                                      WritableToCSV::StaticPeriod10HzMode,
                                                                                                      WritableToCSV::StaticPeriod100HzMode,
                                                                                                      WritableToCSV::StaticPeriod250HzMode,
                                                                                                      WritableToCSV::StaticPeriod500HzMode,
                                                                                                      WritableToCSV::StaticPeriod1000HzMode};

DownloadDataSDDialog::DownloadDataSDDialog(RawDataParser& rawDataParser, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DownloadDataSDDialog),
    rawDataParser(rawDataParser)
{
    ui->setupUi(this);
    convertFileThread = new ConvertFileThread(rawDataParser, this);
    filesDownloadDialog = new FilesDownloadDialog(this);

    ui->outputDataLayoutComboBox->clear();
    for (auto& str: outputDataLayoutOptionsString){
        ui->outputDataLayoutComboBox->addItem(str);
    }

    connect(convertFileThread, SIGNAL(actualProgress(int)), filesDownloadDialog, SLOT(updateProgressBar(int)));
    connect(convertFileThread, SIGNAL(actualFileConverting(QString, QString)), filesDownloadDialog, SLOT(addFileToList(QString, QString)));
    connect(convertFileThread, SIGNAL(errorWhileConvertingPreviousFile()), filesDownloadDialog, SLOT(errorInLastFile()));
    connect(convertFileThread, SIGNAL(finished()), filesDownloadDialog, SLOT(downloadingComplete()));
    connect(convertFileThread, SIGNAL(fatalErrorSignal()), this, SLOT(fatalErrorInConvertingThreadSlot()));
    connect(convertFileThread, SIGNAL(started()), filesDownloadDialog, SLOT(convertingThreadStarted()));
}

DownloadDataSDDialog::~DownloadDataSDDialog()
{
    delete ui;
    delete filesDownloadDialog;
    delete convertFileThread;
}

void DownloadDataSDDialog::fatalErrorInConvertingThreadSlot(){
    filesDownloadDialog->close();
    QMessageBox::warning(this, "Converting error", "Fatal error while converting data files.");
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

void DownloadDataSDDialog::on_selectAllButton_clicked()
{
    ui->dataFilesListWidget->selectAll();
}

void DownloadDataSDDialog::on_deselectAll_clicked()
{
    ui->dataFilesListWidget->clearSelection();
}

void DownloadDataSDDialog::on_convertSelectedButton_clicked()
{
    QList<QFileInfo> filesList;
    for (QListWidgetItem* pItem: ui->dataFilesListWidget->selectedItems()){
        filesList.append(QFileInfo(this->sourceDir + "/" + pItem->text()));
    }

    if (filesList.isEmpty()){
        QMessageBox::warning(this, "Choose files to conver.", "List of files to copy/convert is empty. Choose files to proceed.");
        return;
    }

    QDir dataDir(ui->destinationDirComboBox->currentText());
    if (ui->destinationDirComboBox->currentText().isEmpty() || !dataDir.exists()){
        QMessageBox::warning(this, "Given directory not exists.", "Givent destination directory doeas not exists. Choose proper directory path to save files.");
        return;
    }

    convertFileThread->setFileTimingMode(outputDataLayoutOptionsTimingMode[ui->outputDataLayoutComboBox->currentIndex()]);
    convertFileThread->addFilesToConvert(filesList);
    convertFileThread->setDecimaleSeparator(ui->decimalSeparatorComboBox->currentText()[0].toLatin1());

    convertFileThread->setDestinationDirectory(ui->destinationDirComboBox->currentText());
    convertFileThread->start();
    if (!filesDownloadDialog->exec()){
        convertFileThread->cancelExecution();
        convertFileThread->wait(threadWaitTimeout);
    }

    convertFileThread->cancelExecution();
}
