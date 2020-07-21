#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>
#include <QMessageBox>

static const QString authors("AGH Racing: M. Kowalik, A. Morzywołek");

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    rawDataParser(RawDataParser::LittleEndian)
{
    ui->setupUi(this);

    QString version("ver: ");
    version += QString::number(Config::ACTUAL_VERSION);
    version += ".";
    version += QString::number(Config::ACTUAL_SUB_VERSION);

    QLabel *stat = new QLabel(authors + " [" + version + "]");
    stat->setAlignment(Qt::AlignRight);

    ui->statusBar->addWidget(stat, 1);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_downloadSDButton_clicked()
{
    try {
        downloadDialog = new DownloadDataSDDialog(rawDataParser, this);
        downloadDialog->exec();
    } catch (const std::logic_error& e){
        QMessageBox::warning(this, "Error", e.what());
    } catch (const std::exception& e){
        QMessageBox::warning(this, "Error", QString("Unkonwn error occured: ") + QString(e.what()));
    }
}

void MainWindow::on_configureSDButton_clicked()
{
    try {

        configureDialog = new ConfigureLoggerSDDialog(rawDataParser, this);
        configureDialog->exec();
    } catch (const std::logic_error& e){
        QMessageBox::warning(this, "Error", e.what());
    } catch (const std::exception& e){
        QMessageBox::warning(this, "Error", QString("Unkonwn error occured: ") + QString(e.what()));
    }
}
