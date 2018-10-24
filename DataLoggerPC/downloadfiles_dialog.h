#ifndef DOWNLOADFILES_DIALOG_H
#define DOWNLOADFILES_DIALOG_H

#include "thread.h"
#include <QDialog>

namespace Ui {
class DownloadFilesDialog;
}

class DownloadFilesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DownloadFilesDialog(QWidget *parent = nullptr);
    ~DownloadFilesDialog();
    Thread *mThread;

private:
    Ui::DownloadFilesDialog *ui;

public slots:
    void onChanged();
private slots:
    void on_buttonCancel_clicked();
};

#endif // DOWNLOADFILES_DIALOG_H
;
