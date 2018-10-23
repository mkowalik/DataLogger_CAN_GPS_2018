#ifndef DOWNLOADING_DIALOG_H
#define DOWNLOADING_DIALOG_H

#include <QDialog>

namespace Ui {
class DownloadingDialog;
}

class DownloadingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DownloadingDialog(QWidget *parent = nullptr);
    ~DownloadingDialog();

private:
    Ui::DownloadingDialog *ui;
};

#endif // DOWNLOADING_DIALOG_H
