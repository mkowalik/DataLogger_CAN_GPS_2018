#ifndef CONFIGURELOGGERSD_DIALOG_H
#define CONFIGURELOGGERSD_DIALOG_H

#include <QDialog>
#include "newframe_dialog.h"
#include "newchannel_dialog.h"
#include "AGHConfig/Config.h"

namespace Ui {
class ConfigureLoggerSDDialog;
}

class ConfigureLoggerSDDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigureLoggerSDDialog(QWidget *parent = nullptr);
    ~ConfigureLoggerSDDialog();
private:
    void reloadFramesTreeWidget();
private slots:
    void on_newFrameButton_clicked();

    void on_selectOutputFileButton_clicked();

    void on_selectPrototypeFileButton_clicked();

    void on_framesTreeWidget_customContextMenuRequested(const QPoint &pos);

    void on_actionEdit_triggered();

    void on_addChannelButton_clicked();

private:
    Ui::ConfigureLoggerSDDialog *ui;

    RawDataParser rawDataParser;
    Config config;
};

#endif // CONFIGURELOGGERSD_DIALOG_H
