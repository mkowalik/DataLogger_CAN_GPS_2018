#ifndef CONFIGURELOGGERSD_DIALOG_H
#define CONFIGURELOGGERSD_DIALOG_H

#include <QDialog>
#include "frame_dialog.h"
#include "signal_dialog.h"
#include "trigger_dialog.h"
#include "AGHConfig/Config.h"

#include <QTreeWidgetItem>
#include <QListWidgetItem>

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

    enum class TriggerType {
        StartTrigger,
        StopTrigger
    };

    void                   reloadFramesTreeWidget();
    void                   reloadCANBusBitrateWidget();
    void                   reloadGPSFrequencyWidget();
    void                   reloadRTCConfigurationFrameIDWidget();
    void                   reloadUseGPSDateWidget();
    void                   reloadTimeZoneWidget();
    void                   realodLogFilenameWidget();
    void                   reloadStartTriggersWidget();
    void                   reloadStopTriggersWidget();

    void                   reloadConfigView();

    void                   editFrameRow(QTreeWidgetItem *pClickedRow, ConfigFrame* pFrame);
    void                   editSignalRow(QTreeWidgetItem *clickedItem, ConfigSignal* pSignal);
    void                   deleteFrameRow(QTreeWidgetItem *pClickedFrameRow, ConfigFrame *pFrame);
    void                   deleteSignalRow(QTreeWidgetItem *pClickedSignalRow, ConfigSignal *pSignal);
    void                   addNewFrameRow();
    void                   addNewSignalRow(QTreeWidgetItem * pClickedFrameRow, ConfigFrame * pFrame);

    void                   addNewTriggerRow(TriggerType triggerType);
    void                   editTriggerRow(QListWidgetItem* pClickedItem, TriggerType triggerType);
    void                   deleteTriggerRow(QListWidgetItem* pClickedItem, TriggerType triggerType);

    QTreeWidgetItem*       prepareFrameRowWidget(QTreeWidgetItem* pPreviousFrameRow, ConfigFrame* pFrame);
    unsigned int           insertFrameRowWidget(QTreeWidgetItem* pFrameRow, const ConfigFrame* pFrame);
    QTreeWidgetItem*       prepareSignalRowWidget(QTreeWidgetItem* pPreviousChannelRow, ConfigSignal* pSignal);
    unsigned int           insertSignalRowWidget(QTreeWidgetItem * pSignalRow, QTreeWidgetItem * pFrameRow, const ConfigSignal* pSignal);

    string                 canBitrateToString(Config::EnCANBitrate canBitrate);
    Config::EnCANBitrate   stringToCANBitrate(string bitrateStr);
    string                 gpsFrequencyToString(Config::EnGPSFrequency gpsFrequency);
    Config::EnGPSFrequency stringToGPSFrequency(string gpsFrequencyString);

private slots:
    void on_selectOutputFileButton_clicked();
    void on_selectPrototypeFileButton_clicked();
    void on_resetButton_clicked();
    void on_canBitrateComboBox_currentTextChanged(const QString &arg1);
    void on_gpsFreqComboBox_currentTextChanged(const QString &arg1);
    void on_rtcConfigFrameID_spinBox_valueChanged(int arg1);
    void on_framesTreeWidget_customContextMenuRequested(const QPoint &pos);
    void on_framesTreeWidget_itemDoubleClicked(QTreeWidgetItem *clickedItem, int column);
    void on_newFrameButton_clicked();
    void on_addSignalButton_clicked();
    void on_addStartTrigger_button_clicked();
    void on_addStopTrigger_button_clicked();
    void on_saveConfigButton_clicked();
    void on_startTrigger_listWidget_customContextMenuRequested(const QPoint &pos);
    void on_actionEditFrameOrSignal_triggered();
    void on_actionDeleteFrameOrSignal_triggered();
    void on_actionDeleteStartLogTrigger_triggered();
    void on_actionEditStartLogTrigger_triggered();
    void on_startTrigger_listWidget_itemDoubleClicked(QListWidgetItem *item);
    void on_stopTrigger_listWidget_customContextMenuRequested(const QPoint &pos);
    void on_actionDeleteStopLogTrigger_triggered();
    void on_actionEditStopLogTrigger_triggered();
    void on_stopTrigger_listWidget_itemDoubleClicked(QListWidgetItem *item);
    void on_actionAddSignalToClickedFrame_triggered();
    void on_logFilename_lineEdit_textChanged(const QString &arg1);
    void on_useDateFromGPS_checkBox_stateChanged(int arg1);
    void on_timeZone_SpinBox_valueChanged(double arg1);

private:
    Ui::ConfigureLoggerSDDialog *ui;

    RawDataParser& rawDataParser;
    Config* pConfig;

    QListWidgetItem* prepareTriggerListWidget(ConfigTrigger* pTrigger);
};

Q_DECLARE_METATYPE(ConfigTrigger*)
Q_DECLARE_METATYPE(QListWidgetItem*)
Q_DECLARE_METATYPE(QTreeWidgetItem*)

#endif // CONFIGURELOGGERSD_DIALOG_H
