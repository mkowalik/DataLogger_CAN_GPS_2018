#ifndef FILESDOWNLOAD_DIALOG_H
#define FILESDOWNLOAD_DIALOG_H

#include "thread.h"
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
    Thread *mThread;

private:
    Ui::FilesDownloadDialog *ui;

public slots:
    void onChanged();
private slots:
    void onMessageSent(const int &, QString);
    void on_buttonBox_rejected();
};

#endif // FILESDOWNLOAD_DIALOG_H


