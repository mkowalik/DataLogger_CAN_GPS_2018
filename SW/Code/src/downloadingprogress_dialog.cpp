#include "downloadingprogress_dialog.h"
#include "ui_downloadingprogress_dialog.h"
#include "conversion_warnings_dialog.h"

#include "QStringIntMap.h"

#include <QPushButton>
#include <QColor>

static const QString startingString = "Downloading and converting files...";
static const QString doneString = "CONVERTED!";

DownloadInProgressDialog::DownloadInProgressDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FilesDownloadDialog)
{
    ui->setupUi(this);
    cleanUp();
}

void DownloadInProgressDialog::updateProgressBar(int progressBarValue){
    ui->progressBar->setValue(progressBarValue);
}

void DownloadInProgressDialog::addFileToList(QString sourceFileName, QString destinationFileName){
    QListWidgetItem* item = new QListWidgetItem(sourceFileName + " -> " + destinationFileName);
    ui->filesListWidget->addItem(item);
}

void DownloadInProgressDialog::errorInLastFile(QString reasonDescription){
    ui->filesListWidget->item(ui->filesListWidget->count()-1)->setForeground(Qt::red);
    if (reasonDescription.length() > 0){
        QString text = ui->filesListWidget->item(ui->filesListWidget->count()-1)->text();
        text += "\r\n";
        text += reasonDescription;
        ui->filesListWidget->item(ui->filesListWidget->count()-1)->setText(text);
    }
}

void DownloadInProgressDialog::warningsInLastFile(QStringIntMap warnings){
    if (warnings.size() > 0){
        ui->filesListWidget->item(ui->filesListWidget->count()-1)->setForeground(QColor(255,165,0));
        QListWidgetItem* pLastItem = ui->filesListWidget->item(ui->filesListWidget->count()-1);
        pLastItem->setText(ui->filesListWidget->item(ui->filesListWidget->count()-1)->text() +
                           QString::fromStdString("\n") +
                           QString(warnings.size()) +
                           " conversion warnings. Click to see details"
                    );
        QVariant v;
        v.setValue<QStringIntMap>(warnings);
        pLastItem->setData(Qt::UserRole, v);
    }
}

void DownloadInProgressDialog::downloadingComplete(){
    ui->downloadingLabel->setText(doneString);
    ui->closeCancelButtonBox->button(QDialogButtonBox::Close)->setEnabled(true);
    ui->closeCancelButtonBox->button(QDialogButtonBox::Cancel)->setEnabled(false);
}

void DownloadInProgressDialog::convertingThreadStarted(){
    ui->progressBar->setValue(0);
    ui->filesListWidget->clear();
    ui->downloadingLabel->setText(startingString);
    ui->closeCancelButtonBox->button(QDialogButtonBox::Close)->setEnabled(false);
    ui->closeCancelButtonBox->button(QDialogButtonBox::Cancel)->setEnabled(true);
}

void DownloadInProgressDialog::cleanUp()
{
    ui->progressBar->setValue(0);
    ui->filesListWidget->clear();
    ui->downloadingLabel->setText("");
    ui->closeCancelButtonBox->button(QDialogButtonBox::Close)->setEnabled(true);
    ui->closeCancelButtonBox->button(QDialogButtonBox::Cancel)->setEnabled(true);
}

DownloadInProgressDialog::~DownloadInProgressDialog(){
    delete ui;
}

void DownloadInProgressDialog::on_filesListWidget_itemClicked(QListWidgetItem *item)
{
    QStringIntMap warnings = item->data(Qt::UserRole).value<QStringIntMap>();
    ConversionWarningsDialog dialog(warnings, this);
    dialog.exec();
}
