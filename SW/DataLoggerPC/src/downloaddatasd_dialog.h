#ifndef DOWNLOADDATASD_DIALOG_H
#define DOWNLOADDATASD_DIALOG_H

#include <QDialog>
#include "downloadingprogress_dialog.h"
#include <QString>
#include <AGHUtils/WritingClass.h>

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
    void on_selectAllButton_clicked();
    void on_deselectAll_clicked();
    void fatalErrorInConvertingThreadSlot();
    void on_convertSelectedButton_clicked();

    void on_outputDataLayoutComboBox_currentTextChanged(const QString &arg1);

private:
    static constexpr unsigned int frameByFrameIndex = 1;
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
