#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>

static const QString authors("AGH Racing: M. Kowalik, A. Morzywołek");

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QLabel *stat = new QLabel(authors);
    stat->setAlignment(Qt::AlignRight);

    ui->statusBar->addWidget(stat, 1);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_downloadSDButton_clicked()
{
    downloadDialog = new DownloadDataSDDialog(this);
    downloadDialog->exec();
}

void MainWindow::on_configureSDButton_clicked()
{
    configureDialog = new ConfigureLoggerSDDialog(this);
    configureDialog->exec();
}
