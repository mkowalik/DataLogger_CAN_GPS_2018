#ifndef CONFIGURELOGGERSD_DIALOG_H
#define CONFIGURELOGGERSD_DIALOG_H

#include <QDialog>
#include "newframe_dialog.h"
#include "newchannel_dialog.h"
#include "AGHConfig/Config.h"
#include <QTreeWidgetItem>

namespace Ui {
class ConfigureLoggerSDDialog;
}

class ConfigureLoggerSDDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigureLoggerSDDialog(RawDataParser& rawDataParser, QWidget *parent = nullptr);
    ~ConfigureLoggerSDDialog();
private:
    void reloadFramesTreeWidget();
    void editGivenItem(QTreeWidgetItem *clickedItem, QTreeWidgetItem* parent);
    void prepareFrameWidget(const ConfigFrame& frame, QTreeWidgetItem* pWidget);
    void prepareChannelWidget(const ConfigChannel& channel, QTreeWidgetItem* pWidget);
private slots:
    void on_newFrameButton_clicked();
    void on_selectOutputFileButton_clicked();
    void on_selectPrototypeFileButton_clicked();
    void on_framesTreeWidget_customContextMenuRequested(const QPoint &pos);
    void on_actionEdit_triggered();
    void on_addChannelButton_clicked();
    void on_framesTreeWidget_itemDoubleClicked(QTreeWidgetItem *clickedItem, int column);
    void on_actionDelete_triggered();
    void on_resetButton_clicked();
    void on_saveConfigButton_clicked();
private:
    Ui::ConfigureLoggerSDDialog *ui;

    RawDataParser& rawDataParser;
    Config config;
};

#endif // CONFIGURELOGGERSD_DIALOG_H
