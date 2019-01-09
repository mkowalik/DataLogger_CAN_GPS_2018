#ifndef DOWNLOADDATASD_DIALOG_H
#define DOWNLOADDATASD_DIALOG_H

#include <QDialog>
#include "filesdownload_dialog.h"

namespace Ui {
class DownloadDataSDDialog;
}

class DownloadDataSDDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DownloadDataSDDialog(QWidget *parent = nullptr);
    ~DownloadDataSDDialog();

private slots:
    void on_openDataDirButton_clicked();
    void on_openDestDirButton_clicked();

    void on_downloadAndConvertButton_clicked();

private:
    Ui::DownloadDataSDDialog *ui;
    FilesDownloadDialog * filesDownloadDialog;
};

#endif // DOWNLOADDATASD_DIALOG_H
