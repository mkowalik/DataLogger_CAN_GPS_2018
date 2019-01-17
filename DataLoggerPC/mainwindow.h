#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "downloaddatasd_dialog.h"
#include "configureloggersd_dialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_downloadSDButton_clicked();
    void on_configureSDButton_clicked();
    void on_actionTest_triggered();

private:
    Ui::MainWindow *ui;
    DownloadDataSDDialog *downloadDialog;
    ConfigureLoggerSDDialog *configureDialog;
};

#endif // MAINWINDOW_H
