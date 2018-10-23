#include "configureloggersd_dialog.h"
#include "ui_configureloggersd_dialog.h"

#include <QFileDialog>
#include <QDebug>
#include <QComboBox>
#include <QDir>
#include <QRegExp>

ConfigureLoggerSDDialog::ConfigureLoggerSDDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigureLoggerSDDialog)
{
    ui->setupUi(this);
}

ConfigureLoggerSDDialog::~ConfigureLoggerSDDialog()
{
    delete ui;
}

void ConfigureLoggerSDDialog::on_newFrameButton_clicked()
{
    newFrameDialog = new NewFrameDialog(this);
    newFrameDialog->accept();
}

void ConfigureLoggerSDDialog::on_selectOutputFileButton_clicked()
{
    QString outputFilePath = QFileDialog::getSaveFileName(this, "Choose Output File");

    int position = outputFilePath.lastIndexOf(QRegExp("/.+..+"), -1);
    outputFilePath.remove(position, outputFilePath.length());
    outputFilePath.append("/logger.aghconf");

    ui->selectOutputFileComboBox->addItem(outputFilePath);
    ui->selectOutputFileComboBox->setCurrentText(outputFilePath);

}

void ConfigureLoggerSDDialog::on_selectPrototypeFileButton_clicked()
{
    QString prototypeFilePath = QFileDialog::getOpenFileName(this, "Choose Prototype File", "", "AGH Log (*.aghlog);;AGH Config (*.aghconf)");

    ui->prototypeFileComboBox->addItem(prototypeFilePath);
    ui->prototypeFileComboBox->setCurrentText(prototypeFilePath);
}
