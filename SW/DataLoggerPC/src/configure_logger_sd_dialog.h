#ifndef CONFIGURELOGGERSD_DIALOG_H
#define CONFIGURELOGGERSD_DIALOG_H

#include <QDialog>
#include "frame_dialog.h"
#include "signal_dialog.h"
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
    void                   reloadStartTriggersWidget();
    void                   reloadStopTriggersWidget();
    void                   reloadConfigView();

    void                   editFrameRow(QTreeWidgetItem *pClickedRow, ConfigFrame* pFrame);
    void                   editSignalRow(QTreeWidgetItem *clickedItem, ConfigSignal* pSignal);
    void                   deleteFrameRow(QTreeWidgetItem *pClickedFrameRow, ConfigFrame *pFrame);
    void                   deleteSignalRow(QTreeWidgetItem *pClickedSignalRow, ConfigSignal *pSignal);
    void                   addNewFrameRow();
    void                   addNewSignalRow(QTreeWidgetItem * pClickedFrameRow, ConfigFrame * pFrame);

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
    void on_canBitrateComboBox_editTextChanged(const QString &arg1);
    void on_gpsFreqComboBox_currentTextChanged(const QString &arg1);
    void on_rtcConfigFrameID_spinBox_valueChanged(int arg1);
    void on_framesTreeWidget_customContextMenuRequested(const QPoint &pos);
    void on_actionEdit_triggered();
    void on_actionDelete_triggered();
    void on_framesTreeWidget_itemDoubleClicked(QTreeWidgetItem *clickedItem, int column);
    void on_newFrameButton_clicked();
    void on_addSignalButton_clicked();
    void on_addStartTrigger_button_clicked();
    void on_saveConfigButton_clicked();

private:
    Ui::ConfigureLoggerSDDialog *ui;

    RawDataParser& rawDataParser;
    Config* pConfig;
    map<QTreeWidgetItem*, ConfigFrame*> frameMap;
    map<QTreeWidgetItem*, ConfigSignal*> signalMap;
};

#endif // CONFIGURELOGGERSD_DIALOG_H
