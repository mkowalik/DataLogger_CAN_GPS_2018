#ifndef DOWNLOADDATASD_DIALOG_H
#define DOWNLOADDATASD_DIALOG_H

#include <QDialog>
#include "filesdownload_dialog.h"
#include <QString>
#include <AGHConfig/WritingClass.h>

namespace Ui {
class DownloadDataSDDialog;
}

class DownloadDataSDDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DownloadDataSDDialog(RawDataParser& rawDataParser, QWidget *parent = nullptr);
    ~DownloadDataSDDialog();

private slots:
    void on_openDataDirButton_clicked();
    void on_openDestDirButton_clicked();
    void on_downloadAndConvertButton_clicked();
    void on_selectAllButton_clicked();
    void on_deselectAll_clicked();

private:
    static const QStringList outputDataLayoutOptionsString;
    static const QList<WritableToCSV::FileTimingMode> outputDataLayoutOptionsTimingMode;
    Ui::DownloadDataSDDialog *ui;
    FilesDownloadDialog * filesDownloadDialog;
    ConvertFileThread * convertFileThread;

    QString sourceDir;
    RawDataParser rawDataParser;

    static const unsigned int threadWaitTimeout = 1000;

    void sourcePathChanged(QString dirPath);
};

#endif // DOWNLOADDATASD_DIALOG_H
