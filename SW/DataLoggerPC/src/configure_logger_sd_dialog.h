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
    void                   reloadFramesTreeWidget();
    void                   reloadCANBusBitrateWidget();
    void                   reloadGPSFrequencyWidget();
    void                   reloadConfigView();
    void                   editGivenItem(QTreeWidgetItem *clickedItem, QTreeWidgetItem* parent);
    void                   prepareFrameWidget(const ConfigFrame& frame, QTreeWidgetItem* pWidget);
    void                   prepareChannelWidget(const ConfigSignal& channel, QTreeWidgetItem* pWidget);
    string                 canBitrateToString(Config::EnCANBitrate canBitrate);
    Config::EnCANBitrate   stringToCANBitrate(string bitrateStr);
    string                 gpsFrequencyToString(Config::EnGPSFrequency gpsFrequency);
    Config::EnGPSFrequency stringToGPSFrequency(string gpsFrequencyString);
private slots:
    void on_selectOutputFileButton_clicked();
    void on_selectPrototypeFileButton_clicked();

    void on_resetButton_clicked();
    void on_framesTreeWidget_customContextMenuRequested(const QPoint &pos);
    void on_actionEdit_triggered();
    void on_actionDelete_triggered();
    void on_framesTreeWidget_itemDoubleClicked(QTreeWidgetItem *clickedItem, int column);
    void on_newFrameButton_clicked();
    void on_addChannelButton_clicked();
    void on_saveConfigButton_clicked();
    void on_canSpeedComboBox_currentTextChanged(const QString &val);

    void on_gpsFreqComboBox_currentTextChanged(const QString &arg1);

private:
    Ui::ConfigureLoggerSDDialog *ui;

    RawDataParser& rawDataParser;
    Config* pConfig;
    map<QTreeWidgetItem*, unsigned int> idMap;
};

#endif // CONFIGURELOGGERSD_DIALOG_H
