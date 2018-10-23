#ifndef CONFIGURELOGGERSD_DIALOG_H
#define CONFIGURELOGGERSD_DIALOG_H

#include <QDialog>
#include "newframe_dialog.h"
#include "newchannel_dialog.h"

namespace Ui {
class ConfigureLoggerSDDialog;
}

class ConfigureLoggerSDDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigureLoggerSDDialog(QWidget *parent = nullptr);
    ~ConfigureLoggerSDDialog();

private slots:
    void on_newFrameButton_clicked();

    void on_selectOutputFileButton_clicked();

    void on_selectPrototypeFileButton_clicked();

private:
    Ui::ConfigureLoggerSDDialog *ui;
    NewFrameDialog *newFrameDialog;
    NewChannelDialog *newChannelDialog;
};

#endif // CONFIGURELOGGERSD_DIALOG_H
