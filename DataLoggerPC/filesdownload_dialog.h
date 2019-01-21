#ifndef FILESDOWNLOAD_DIALOG_H
#define FILESDOWNLOAD_DIALOG_H

#include "convertfile_thread.h"
#include <QDialog>

namespace Ui {
class FilesDownloadDialog;
}

class FilesDownloadDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FilesDownloadDialog(QWidget *parent = nullptr);
    ~FilesDownloadDialog();
private:
    Ui::FilesDownloadDialog *ui;
public slots:
    void updateProgressBar(int);
    void addFileToList(QString fileName);
    void on_buttonBox_rejected();
};

#endif // FILESDOWNLOAD_DIALOG_H


