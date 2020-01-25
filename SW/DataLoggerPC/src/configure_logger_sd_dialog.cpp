#include "configure_logger_sd_dialog.h"
#include "ui_configure_logger_sd_dialog.h"

#include "AGHConfig/Config.h"
#include "AGHConfig/ConfigFrame.h"
#include "AGHConfig/ConfigSignal.h"

#include <QFileDialog>
#include <QDebug>
#include <QComboBox>
#include <QDir>
#include <QRegExp>
#include <QAction>
#include <QMenu>
#include <QMessageBox>

#include <iostream>

ConfigureLoggerSDDialog::ConfigureLoggerSDDialog(RawDataParser& rawDataParser, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigureLoggerSDDialog),
    rawDataParser(rawDataParser)
{
    ui->setupUi(this);
    ui->framesTreeWidget->header()->resizeSection(0, 220);
    ui->framesTreeWidget->header()->resizeSection(1, 50);
    ui->framesTreeWidget->header()->resizeSection(2, 50);
    ui->framesTreeWidget->header()->resizeSection(3, 90);
    ui->framesTreeWidget->header()->resizeSection(4, 70);
    ui->framesTreeWidget->header()->resizeSection(5, 50);
    ui->framesTreeWidget->header()->resizeSection(6, 60);
    ui->framesTreeWidget->header()->resizeSection(7, 50);
    ui->framesTreeWidget->header()->resizeSection(8, 50);
    ui->framesTreeWidget->header()->resizeSection(9, 100);

    this->pConfig = new Config(ui->logFilename_lineEdit->text().toStdString(),
                               stringToCANBitrate(ui->canBitrateComboBox->currentText().toStdString()),
                               stringToGPSFrequency(ui->gpsFreqComboBox->currentText().toStdString()),
                               static_cast<unsigned int>(ui->rtcConfigFrameID_spinBox->value())
                               );

    reloadConfigView();
}

ConfigureLoggerSDDialog::~ConfigureLoggerSDDialog()
{
    delete this->pConfig;
    delete ui;
}

void ConfigureLoggerSDDialog::reloadFramesTreeWidget(){

    ui->framesTreeWidget->clear();
    frameMap.clear();
    signalMap.clear();

    for (Config::FramesIterator frameIt = pConfig->beginFrames(); frameIt!=pConfig->endFrames(); frameIt++){

        QTreeWidgetItem* pFrameRow = new QTreeWidgetItem(ui->framesTreeWidget);
        prepareFrameRowWidget(pFrameRow, *frameIt);
        ui->framesTreeWidget->addTopLevelItem(pFrameRow);
        frameMap.insert({pFrameRow, (*frameIt)});

        for (auto sigIt = (*frameIt)->beginSignals(); sigIt != (*frameIt)->endSignals(); sigIt++){
            QTreeWidgetItem* pSignalRow = new QTreeWidgetItem(pFrameRow);
            prepareSignalRowWidget(pSignalRow, *sigIt);
            signalMap.insert({pSignalRow, (*sigIt)});
        }
    }
}

void ConfigureLoggerSDDialog::reloadCANBusBitrateWidget(){
    QString canBusBitrateString = QString::fromStdString(this->canBitrateToString(this->pConfig->getCANBitrate()));
    ui->canBitrateComboBox->setCurrentText(canBusBitrateString);
}

void ConfigureLoggerSDDialog::reloadGPSFrequencyWidget(){
    QString gpsFrequencyString = QString::fromStdString(this->gpsFrequencyToString(this->pConfig->getGPSFrequency()));
    ui->gpsFreqComboBox->setCurrentText(gpsFrequencyString);
}

void ConfigureLoggerSDDialog::reloadStartTriggersWidget(){
    ui->startTrigger_listWidget->clear();
    for (auto it = pConfig->cbeginStartTriggers(); it != pConfig->cendStartTriggers(); ++it){
        ui->startTrigger_listWidget->addItem(prepareTriggerListWidget(*it));
    }
}

void ConfigureLoggerSDDialog::reloadStopTriggersWidget(){
    ui->stopTrigger_listWidget->clear();
    for (auto it = pConfig->cbeginStopTriggers(); it != pConfig->cendStopTriggers(); ++it){
        ui->stopTrigger_listWidget->addItem(prepareTriggerListWidget(*it));
    }
}

void ConfigureLoggerSDDialog::reloadConfigView(){
    reloadFramesTreeWidget();
    reloadCANBusBitrateWidget();
    reloadGPSFrequencyWidget();
    reloadStartTriggersWidget();
    reloadStopTriggersWidget();
}

void ConfigureLoggerSDDialog::reloadEnableDisableSaveButton()
{
    if ((pConfig->getNumberOfStartTriggers() == 0) || (pConfig->framesEmpty())){
        ui->saveConfigButton->setEnabled(false);
    } else {
        ui->saveConfigButton->setEnabled(true);
    }
}

void ConfigureLoggerSDDialog::editFrameRow(QTreeWidgetItem *pClickedFrameRow, ConfigFrame* pFrame){

    FrameDialog frameDialog(*(this->pConfig), pFrame, this);

    while (true) {
        try {
            if (frameDialog.exec() == QDialog::Accepted){
                pFrame->setFrameID(frameDialog.getFrameID());
                pFrame->setFrameName(frameDialog.getModuleName().toStdString());

                prepareFrameRowWidget(pClickedFrameRow, pFrame);
                ui->framesTreeWidget->takeTopLevelItem(ui->framesTreeWidget->indexOfTopLevelItem(pClickedFrameRow));
                insertFrameRowWidget(pClickedFrameRow, pFrame);
                ui->framesTreeWidget->clearSelection();
            }
            return;
        } catch (const std::logic_error& e) {
            QMessageBox::warning(this, "Error", e.what());
        }
    }
}

void ConfigureLoggerSDDialog::editSignalRow(QTreeWidgetItem *pClickedSignalRow, ConfigSignal* pSignal){

    SignalDialog signalDialog(*(pSignal->getParentFrame()), pSignal, this);

    while (true) {
        try {
            if (signalDialog.exec() == QDialog::Accepted){

                pSignal->setSignalID(signalDialog.getSignalID());
                pSignal->setStartBit(signalDialog.getStartBit());
                pSignal->setLengthBits(signalDialog.getLengthBits());
                pSignal->setValueType(ValueType(signalDialog.getIsSigned(), signalDialog.getIsBigEndian()));
                pSignal->setMultiplier(signalDialog.getMultiplier());
                pSignal->setDivider(signalDialog.getDivider());
                pSignal->setOffset(signalDialog.getOffset());
                pSignal->setSignallName(signalDialog.getSignalName().toStdString());
                pSignal->setUnitName(signalDialog.getUnit().toStdString());
                pSignal->setComment(signalDialog.getComment().toStdString());
                pSignal->setSignalID(signalDialog.getSignalID());

                prepareSignalRowWidget(pClickedSignalRow, pSignal);
                QTreeWidgetItem* pParentFrameRow = pClickedSignalRow->parent();
                pParentFrameRow->removeChild(pClickedSignalRow);
                insertSignalRowWidget(pClickedSignalRow, pParentFrameRow, pSignal);
                ui->framesTreeWidget->clearSelection();

                reloadStartTriggersWidget();
                reloadStopTriggersWidget();
            }
            return;
        } catch (const std::logic_error& e) {
            QMessageBox::warning(this, "Error", e.what());
        }
    }
}

void ConfigureLoggerSDDialog::deleteFrameRow(QTreeWidgetItem *pClickedFrameRow, ConfigFrame* pFrame){

    if (QMessageBox::question(this, "Delete frame", "Do you relly want to delete this frame?") == QMessageBox::StandardButton::Yes){
        pFrame->getParentConfig()->removeFrame(pFrame->getFrameID());
        this->frameMap.erase(pClickedFrameRow);
        for (auto pSignalRow : pClickedFrameRow->takeChildren()){
            signalMap.erase(pSignalRow);
        }
        delete ui->framesTreeWidget->takeTopLevelItem(ui->framesTreeWidget->indexOfTopLevelItem(pClickedFrameRow)); //< Removes the top-level item at the given index in the tree
        ui->framesTreeWidget->clearSelection();
    }
}

void ConfigureLoggerSDDialog::deleteSignalRow(QTreeWidgetItem *pClickedSignalRow, ConfigSignal* pSignal){

    if (QMessageBox::question(this, "Delete channel", "Do you relly want to delete this channel?") == QMessageBox::StandardButton::Yes){
        ConfigFrame * pParentFrame = pSignal->getParentFrame();
        pParentFrame->removeSignal(pSignal->getSignalID());
        pClickedSignalRow->parent()->removeChild(pClickedSignalRow);
        delete pClickedSignalRow;
        ui->framesTreeWidget->clearSelection();
    }
}

void ConfigureLoggerSDDialog::addNewFrameRow() {
    FrameDialog newFrameDialog(*(this->pConfig), nullptr, this);

    while (true) {
        try {
            if (newFrameDialog.exec() == QDialog::Accepted){

                ConfigFrame* pFrame = pConfig->addFrame(newFrameDialog.getFrameID(), newFrameDialog.getModuleName().toStdString());

                QTreeWidgetItem* pFrameRow = prepareFrameRowWidget(nullptr, pFrame);
                insertFrameRowWidget(pFrameRow, pFrame);
                ui->framesTreeWidget->clearSelection();
            }
            return;
        } catch (const std::logic_error& e) {
            QMessageBox::warning(this, "Error", e.what());
        }
    }
}

void ConfigureLoggerSDDialog::addNewSignalRow(QTreeWidgetItem * pSelectedFrameRow, ConfigFrame * pFrame) {

    SignalDialog signalDialog(*pFrame, nullptr, this);

    while (true) {
        try {
            if (signalDialog.exec() == QDialog::Accepted){
                ConfigSignal* pSignal = nullptr;
                if (signalDialog.getAutoSignalID()){
                    pSignal = pFrame->addSignal(signalDialog.getStartBit(),
                                                signalDialog.getLengthBits(),
                                                ValueType(signalDialog.getIsSigned(), signalDialog.getIsBigEndian()),
                                                signalDialog.getMultiplier(),
                                                signalDialog.getDivider(),
                                                signalDialog.getOffset(),
                                                signalDialog.getSignalName().toStdString(),
                                                signalDialog.getUnit().toStdString(),
                                                signalDialog.getComment().toStdString()
                                                );
                } else {
                    pSignal = pFrame->addSignal(signalDialog.getSignalID(),
                                                signalDialog.getStartBit(),
                                                signalDialog.getLengthBits(),
                                                ValueType(signalDialog.getIsSigned(), signalDialog.getIsBigEndian()),
                                                signalDialog.getMultiplier(),
                                                signalDialog.getDivider(),
                                                signalDialog.getOffset(),
                                                signalDialog.getSignalName().toStdString(),
                                                signalDialog.getUnit().toStdString(),
                                                signalDialog.getComment().toStdString()
                                                );
                }

                QTreeWidgetItem* pSignalRow = prepareSignalRowWidget(nullptr, pSignal);
                insertSignalRowWidget(pSignalRow, pSelectedFrameRow, pSignal);
                ui->framesTreeWidget->clearSelection();
            }
            return;
        } catch (const std::logic_error& e) {
            QMessageBox::warning(this, "Error", e.what());
        }
    }
}

void ConfigureLoggerSDDialog::addNewTriggerRow(TriggerType triggerType) {

    TriggerDialog newTriggerDialog(*pConfig, nullptr, this);
    ConfigTrigger* pTrigger;

    while (true) {
        try {
            if (newTriggerDialog.exec() == QDialog::Accepted){
                switch (triggerType) {
                case TriggerType::StartTrigger:
                    pTrigger            = pConfig->addStartTrigger(newTriggerDialog.getTriggerName().toStdString(), newTriggerDialog.getSelectedFrame(), newTriggerDialog.getSelectedSignal(), newTriggerDialog.getConstCompareValue(), newTriggerDialog.getCompareOperator());
                    ui->startTrigger_listWidget->addItem(prepareTriggerListWidget(pTrigger));
                    break;
                case TriggerType::StopTrigger:
                    pTrigger            = pConfig->addStopTrigger(newTriggerDialog.getTriggerName().toStdString(), newTriggerDialog.getSelectedFrame(), newTriggerDialog.getSelectedSignal(), newTriggerDialog.getConstCompareValue(), newTriggerDialog.getCompareOperator());
                    ui->stopTrigger_listWidget->addItem(prepareTriggerListWidget(pTrigger));
                    break;
                }
            }
            return;
        } catch (const std::logic_error& e) {
            QMessageBox::warning(this, "Error", e.what());
        }
    }
}

void ConfigureLoggerSDDialog::editTriggerRow(QListWidgetItem *pClickedItem, TriggerType)
{
    ConfigTrigger* pTrigger = pClickedItem->data(Qt::UserRole).value<ConfigTrigger*>();

    TriggerDialog newTriggerDialog(*pConfig, pTrigger, this);

    while (true) {
        try {
            if (newTriggerDialog.exec() == QDialog::Accepted){
                pTrigger->setTriggerName(newTriggerDialog.getTriggerName().toStdString());
                pTrigger->setCompareConstValue(newTriggerDialog.getConstCompareValue());
                pTrigger->setFrameSignalOperator(newTriggerDialog.getSelectedFrame(), newTriggerDialog.getSelectedSignal(), newTriggerDialog.getCompareOperator());
                pClickedItem->setText(newTriggerDialog.getTriggerName() + " (" + newTriggerDialog.getFormulaRenderValue() + ")");
            }
            return;
        } catch (const std::logic_error& e) {
            QMessageBox::warning(this, "Error", e.what());
        }
    }

}

void ConfigureLoggerSDDialog::deleteTriggerRow(QListWidgetItem *pClickedItem, TriggerType triggerType)
{
    if (QMessageBox::question(this, "Delete trigger", "Do you relly want to delete this trigger?") == QMessageBox::StandardButton::Yes){
        ConfigTrigger * pTriggerToRemove = pClickedItem->data(Qt::UserRole).value<ConfigTrigger*>();
        switch (triggerType){
        case TriggerType::StartTrigger:
            pConfig->removeStartTrigger(pTriggerToRemove);
            delete ui->startTrigger_listWidget->takeItem(ui->startTrigger_listWidget->row(pClickedItem));
            ui->startLogTriggers_label->clearFocus();
            break;
        case TriggerType::StopTrigger:
        default:
            pConfig->removeStopTrigger(pTriggerToRemove);
            delete ui->stopTrigger_listWidget->takeItem(ui->stopTrigger_listWidget->row(pClickedItem));
            ui->startLogTriggers_label->clearFocus();
            break;
        }
        ui->framesTreeWidget->clearSelection();
    }
}

QTreeWidgetItem *ConfigureLoggerSDDialog::prepareFrameRowWidget(QTreeWidgetItem *pPreviousFrameRow, ConfigFrame *pFrame)
{
    if (pPreviousFrameRow == nullptr){
        pPreviousFrameRow = new QTreeWidgetItem();
        frameMap.insert({pPreviousFrameRow, pFrame});
    }

    QString text;
    text.append(QString::fromStdString(pFrame->getFrameName()));
    text.append(" [");
    text.append("0x");
    text.append(QString::number(pFrame->getFrameID(), 16));
    text.append("]");

    pPreviousFrameRow->setText(0, text);

    return pPreviousFrameRow;
}

unsigned int ConfigureLoggerSDDialog::insertFrameRowWidget(QTreeWidgetItem* pFrameRow, const ConfigFrame* pFrame)
{
    int position = 0;
    for (; position < ui->framesTreeWidget->topLevelItemCount(); position++){
        QTreeWidgetItem* pFrameRow = ui->framesTreeWidget->topLevelItem(position);
        unsigned int id = frameMap.at(pFrameRow)->getFrameID();
        if (id > pFrame->getFrameID()){
            break;
        }
    }

    ui->framesTreeWidget->insertTopLevelItem(position, pFrameRow);

    return static_cast<unsigned int>(position);
}

QTreeWidgetItem *ConfigureLoggerSDDialog::prepareSignalRowWidget(QTreeWidgetItem *pPreviousChannelRow, ConfigSignal * pSignal)
{
    if (pPreviousChannelRow == nullptr){
        pPreviousChannelRow = new QTreeWidgetItem();
        signalMap.insert({pPreviousChannelRow, pSignal});
    }

    pPreviousChannelRow->setText(0, QString::fromStdString(pSignal->getSignalName()));
    pPreviousChannelRow->setText(1, QString::number(pSignal->getSignalID()));
    pPreviousChannelRow->setText(2, QString::number(pSignal->getStartBit()));
    pPreviousChannelRow->setText(3, QString::number(pSignal->getLengthBits()));
    pPreviousChannelRow->setText(4, QString(pSignal->getValueType().isBigEndianType() ? "true" : "false"));
    pPreviousChannelRow->setText(5, QString(pSignal->getValueType().isSignedType() ? "true" : "false"));
    pPreviousChannelRow->setText(6, QString::number(pSignal->getMultiplier()));
    pPreviousChannelRow->setText(7, QString::number(pSignal->getDivider()));
    pPreviousChannelRow->setText(8, QString::number(pSignal->getOffset()));
    pPreviousChannelRow->setText(9, QString::fromStdString(pSignal->getUnitName()));
    pPreviousChannelRow->setText(10, QString::fromStdString(pSignal->getComment()));

    return pPreviousChannelRow;
}

unsigned int ConfigureLoggerSDDialog::insertSignalRowWidget(QTreeWidgetItem * pSignalRow, QTreeWidgetItem * pFrameRow, const ConfigSignal* pSignal) {
    int position = 0;
    for (; position < pFrameRow->childCount(); position++){
        if (signalMap.at(pFrameRow->child(position))->getSignalID() > pSignal->getSignalID()){
            break;
        }
    }

    pFrameRow->insertChild(position, pSignalRow);

    return static_cast<unsigned int>(position);
}

string ConfigureLoggerSDDialog::canBitrateToString(Config::EnCANBitrate canBitrate){
    switch(canBitrate){
    case Config::EnCANBitrate::bitrate_50kbps:
        return "50kbps";
    case Config::EnCANBitrate::bitrate_125kbps:
        return "125kbps";
    case Config::EnCANBitrate::bitrate_250kbps:
        return "250kbps";
    case Config::EnCANBitrate::bitrate_500kbps:
        return "500kbps";
    case Config::EnCANBitrate::bitrate_1Mbps:
        return "1Mbps";
    default:
         throw std::invalid_argument("Invalid value of CAN bitrate.");
    }
}

Config::EnCANBitrate ConfigureLoggerSDDialog::stringToCANBitrate(string bitrateStr){
    if(bitrateStr == "50kbps"){
        return Config::EnCANBitrate::bitrate_50kbps;
    } else if (bitrateStr == "125kbps"){
        return Config::EnCANBitrate::bitrate_125kbps;
    } else if (bitrateStr == "250kbps"){
        return Config::EnCANBitrate::bitrate_250kbps;
    } else if (bitrateStr == "500kbps"){
        return Config::EnCANBitrate::bitrate_500kbps;
    } else if (bitrateStr == "1Mbps"){
        return Config::EnCANBitrate::bitrate_1Mbps;
    } else {
        throw std::invalid_argument("Invalid value of CAN bitrate string.");
    }
}

string ConfigureLoggerSDDialog::gpsFrequencyToString(Config::EnGPSFrequency gpsFrequency){
    switch(gpsFrequency){
    case Config::EnGPSFrequency::freq_GPS_OFF:
        return "GPS OFF";
    case Config::EnGPSFrequency::freq_0_5_Hz:
        return "0.5 Hz";
    case Config::EnGPSFrequency::freq_1_Hz:
        return "1 Hz";
    case Config::EnGPSFrequency::freq_2_Hz:
        return "2 Hz";
    case Config::EnGPSFrequency::freq_5_Hz:
        return "5 Hz";
    case Config::EnGPSFrequency::freq_10_Hz:
        return "10 Hz";
    default:
        throw std::invalid_argument("Invalid value of GPS frequency.");
    }
}

Config::EnGPSFrequency ConfigureLoggerSDDialog::stringToGPSFrequency(string gpsFrequencyString){
    if (gpsFrequencyString == "GPS OFF"){
        return Config::EnGPSFrequency::freq_GPS_OFF;
    } else if (gpsFrequencyString == "0.5 Hz"){
        return Config::EnGPSFrequency::freq_0_5_Hz;
    } else if (gpsFrequencyString == "1 Hz"){
        return Config::EnGPSFrequency::freq_1_Hz;
    } else if (gpsFrequencyString == "2 Hz"){
        return Config::EnGPSFrequency::freq_2_Hz;
    } else if (gpsFrequencyString == "5 Hz"){
        return Config::EnGPSFrequency::freq_5_Hz;
    } else if (gpsFrequencyString == "10 Hz"){
        return Config::EnGPSFrequency::freq_10_Hz;
    } else {
        throw std::invalid_argument("Invalid value of GPS frequency string.");
    }
}

void ConfigureLoggerSDDialog::on_selectOutputFileButton_clicked()
{
    try {
        //TODO przerobic na zwykle okno, zeby sprawdzic czy user nie kliknal cancel
        QString outputFilePath = QFileDialog::getSaveFileName(this, "Choose Output File", "", "AGH Config (*.aghconf)");

        if (!outputFilePath.endsWith(".aghconf")){
            QMessageBox::warning(this, "Wrong file name", "Given file does not have \".aghconf\" extension. Choose proper file.");
            return;
        }

        ui->selectOutputFileComboBox->addItem(outputFilePath);
        ui->selectOutputFileComboBox->setCurrentText(outputFilePath);
    } catch (const std::logic_error& e){
        QMessageBox::warning(this, "Error", e.what());
    } catch (const std::exception& e){
        QMessageBox::warning(this, "Error", QString("Unkonwn error occured: ") + QString(e.what()));
    }
}

void ConfigureLoggerSDDialog::on_selectPrototypeFileButton_clicked()
{
    try {
        QString prototypeFilePath = QFileDialog::getOpenFileName(this, "Choose Prototype File", "", tr("AGH Config/Log (*.aghconf *.aghlog)"));

        if (!(prototypeFilePath.endsWith(".aghconf") || prototypeFilePath.endsWith(".aghlog"))){
            QMessageBox::warning(this, "Wrong file name", "Given file does not have \".aghconf\" or \".aghlog\" extension. Choose proper file.");
            return;
        }

        ReadingClass reader(prototypeFilePath.toStdString(), rawDataParser);
        pConfig->reset();
        try {
            pConfig->readFromBin(reader);
        } catch(const std::logic_error& e){
            QMessageBox::warning(this, "Prototype file problem.", QString("Problem with reading prototype file. ") + QString(e.what()));
            return;
        }

        ui->prototypeFile_comboBox->addItem(prototypeFilePath);
        ui->prototypeFile_comboBox->setCurrentText(prototypeFilePath);

        reloadConfigView();

    } catch (const std::logic_error& e){
        QMessageBox::warning(this, "Error", e.what());
    } catch (const std::exception& e){
        QMessageBox::warning(this, "Error", QString("Unkonwn error occured: ") + QString(e.what()));
    }
}

void ConfigureLoggerSDDialog::on_resetButton_clicked()
{
    try {
        if (QMessageBox::question(this, "Reset", "Do you really want to reset all frames, signals and triggers?") == QMessageBox::Yes){
            ui->prototypeFile_comboBox->setCurrentText("");
            pConfig->reset();
        }
        reloadConfigView();

    } catch (const std::logic_error& e){
        QMessageBox::warning(this, "Error", e.what());
    } catch (const std::exception& e){
        QMessageBox::warning(this, "Error", QString("Unkonwn error occured: ") + QString(e.what()));
    }
}

void ConfigureLoggerSDDialog::on_canBitrateComboBox_editTextChanged(const QString &val)
{
    try {
        this->pConfig->setCANBitrate(this->stringToCANBitrate(val.toStdString()));
    } catch (const std::logic_error& e){
        QMessageBox::warning(this, "Error", e.what());
    } catch (const std::exception& e){
        QMessageBox::warning(this, "Error", QString("Unkonwn error occured: ") + QString(e.what()));
    }
}

void ConfigureLoggerSDDialog::on_gpsFreqComboBox_currentTextChanged(const QString &val)
{
    try {
        this->pConfig->setGPSFrequency(this->stringToGPSFrequency(val.toStdString()));
    } catch (const std::logic_error& e){
        QMessageBox::warning(this, "Error", e.what());
    } catch (const std::exception& e){
        QMessageBox::warning(this, "Error", QString("Unkonwn error occured: ") + QString(e.what()));
    }
}

void ConfigureLoggerSDDialog::on_rtcConfigFrameID_spinBox_valueChanged(int val)
{
    try {
        this->pConfig->setRTCConfigurationFrameID(static_cast<unsigned int>(val));
    } catch (const std::logic_error& e){
        QMessageBox::warning(this, "Error", e.what());
    } catch (const std::exception& e){
        QMessageBox::warning(this, "Error", QString("Unkonwn error occured: ") + QString(e.what()));
    }
}

void ConfigureLoggerSDDialog::on_framesTreeWidget_customContextMenuRequested(const QPoint &pos)
{
    try {
        if (pConfig->framesEmpty()){
            return;
        }
        QMenu menu(this); // add menu items
        menu.addAction(ui->actionEditFrameOrSignal);
        menu.addAction(ui->actionDeleteFrameOrSignal);
        menu.addAction(ui->actionAddSignalToClickedFrame);

        QTreeWidgetItem *pClickedItem = ui->framesTreeWidget->itemAt(pos);

        if (pClickedItem == nullptr){
            return;
        }

        QVariant v;
        v.setValue(pClickedItem);

        ui->actionDeleteFrameOrSignal->setData(v); // if you will need the position data save it to the action
        ui->actionEditFrameOrSignal->setData(v); // if you will need the position data save it to the action

        menu.exec(ui->framesTreeWidget->mapToGlobal(pos));
    } catch (const std::logic_error& e){
        QMessageBox::warning(this, "Error", e.what());
    } catch (const std::exception& e){
        QMessageBox::warning(this, "Error", QString("Unkonwn error occured: ") + QString(e.what()));
    }
}

void ConfigureLoggerSDDialog::on_framesTreeWidget_itemDoubleClicked(QTreeWidgetItem *pClickedItem, int)
{
    try {
        if (pClickedItem->parent()){
            ConfigSignal * pSignal = signalMap.at(pClickedItem);
            editSignalRow(pClickedItem, pSignal);
        } else {
            ConfigFrame* pFrame = frameMap.at(pClickedItem);
            editFrameRow(pClickedItem, pFrame);
        }
    } catch (const std::logic_error& e){
        QMessageBox::warning(this, "Error", e.what());
    } catch (const std::exception& e){
        QMessageBox::warning(this, "Error", QString("Unkonwn error occured: ") + QString(e.what()));
    }
}

void ConfigureLoggerSDDialog::on_newFrameButton_clicked()
{
    try {
        addNewFrameRow();
    } catch (const std::logic_error& e){
        QMessageBox::warning(this, "Error", e.what());
    } catch (const std::exception& e){
        QMessageBox::warning(this, "Error", QString("Unkonwn error occured: ") + QString(e.what()));
    }
}

void ConfigureLoggerSDDialog::on_addSignalButton_clicked() {
    try {
        QList<QTreeWidgetItem*> selectedList = ui->framesTreeWidget->selectedItems();
        if (selectedList.isEmpty() || (selectedList.size() != 1)){
            QMessageBox::warning(this, "Select frame", "Select one frame to which add the signal.");
            return;
        }
        QTreeWidgetItem* pSelectedFrameRow = selectedList[0];
        while (ui->framesTreeWidget->indexOfTopLevelItem(pSelectedFrameRow) == -1){
            pSelectedFrameRow = pSelectedFrameRow->parent();
        }
        if (ui->framesTreeWidget->indexOfTopLevelItem(pSelectedFrameRow) == -1){
            QMessageBox::warning(this, "Select proper frame", "Select proper frame to which add the signal.");
            return;
        }
        ConfigFrame* pFrame = frameMap.at(pSelectedFrameRow);
        addNewSignalRow(pSelectedFrameRow, pFrame);

    } catch (const std::logic_error& e){
        QMessageBox::warning(this, "Error", e.what());
    } catch (const std::exception& e){
        QMessageBox::warning(this, "Error", QString("Unkonwn error occured: ") + QString(e.what()));
    }
}

void ConfigureLoggerSDDialog::on_addStartTrigger_button_clicked() {
    try {
        addNewTriggerRow(TriggerType::StartTrigger);
    } catch (const std::logic_error& e){
        QMessageBox::warning(this, "Error", e.what());
    } catch (const std::exception& e){
        QMessageBox::warning(this, "Error", QString("Unkonwn error occured: ") + QString(e.what()));
    }
}

void ConfigureLoggerSDDialog::on_addStopTrigger_button_clicked()
{
    try {
        addNewTriggerRow(TriggerType::StopTrigger);
    } catch (const std::logic_error& e){
        QMessageBox::warning(this, "Error", e.what());
    } catch (const std::exception& e){
        QMessageBox::warning(this, "Error", QString("Unkonwn error occured: ") + QString(e.what()));
    }
}

void ConfigureLoggerSDDialog::on_saveConfigButton_clicked()
{
    try {
        QString filePath = ui->selectOutputFileComboBox->currentText();
        if (filePath.isEmpty()){
            QMessageBox::warning(this, "Choose file", "Choose file path to which you want to save config.");
            return;
        }
        if (!filePath.endsWith(".aghconf")){
            QMessageBox::warning(this, "Choose file", "Choose file with .aghconf extension.");
            return;
        }
        QFileInfo file(filePath);

        if (file.exists()){
            if (QMessageBox::question(this, "File exists", "Chosen file exists. Do You want to overwrite it?") != QMessageBox::Yes){
                return;
            }
        }

        WritingClass writer(filePath.toStdString(), rawDataParser);
        this->pConfig->writeToBin(writer);

        QMessageBox::information(this, "Config export completed", "Your config has ben exported to \"" + filePath + "\" file.");

        reloadConfigView();

    } catch (const std::logic_error& e){
        QMessageBox::warning(this, "Error", e.what());
    } catch (const std::exception& e){
        QMessageBox::warning(this, "Error", QString("Unkonwn error occured: ") + QString(e.what()));
    }
}

void ConfigureLoggerSDDialog::on_startTrigger_listWidget_customContextMenuRequested(const QPoint &pos)
{
    try {
        QMenu menu(this); // add menu items
        menu.addAction(ui->actionEditStartLogTrigger);
        menu.addAction(ui->actionDeleteStartLogTrigger);

        QListWidgetItem *pClickedItem = ui->startTrigger_listWidget->itemAt(pos);

        if (pClickedItem == nullptr){
            return;
        }

        QVariant v;
        v.setValue(pClickedItem);

        ui->actionDeleteStartLogTrigger->setData(v); //< save pointer to row for later use
        ui->actionEditStartLogTrigger->setData(v); //< save pointer to row for later use

        menu.exec(ui->startTrigger_listWidget->mapToGlobal(pos));
    } catch (const std::logic_error& e){
        QMessageBox::warning(this, "Error", e.what());
    } catch (const std::exception& e){
        QMessageBox::warning(this, "Error", QString("Unkonwn error occured: ") + QString(e.what()));
    }
}

void ConfigureLoggerSDDialog::on_actionEditFrameOrSignal_triggered()
{
    try {
        QTreeWidgetItem *pClickedItem = ui->actionEditFrameOrSignal->data().value<QTreeWidgetItem*>();

        if (pClickedItem->parent()){
            ConfigSignal * pSignal = signalMap.at(pClickedItem);
            editSignalRow(pClickedItem, pSignal);
        } else {
            ConfigFrame* pFrame = this->frameMap.at(pClickedItem);
            editFrameRow(pClickedItem, pFrame);
        }
        reloadStartTriggersWidget();
        reloadStopTriggersWidget();
    } catch (const std::logic_error& e){
        QMessageBox::warning(this, "Error", e.what());
    } catch (const std::exception& e){
        QMessageBox::warning(this, "Error", QString("Unkonwn error occured: ") + QString(e.what()));
    }
}

void ConfigureLoggerSDDialog::on_actionDeleteFrameOrSignal_triggered()
{
    try {
        QTreeWidgetItem *pClickedItem = ui->actionEditFrameOrSignal->data().value<QTreeWidgetItem*>();

        if (pClickedItem->parent()){
            ConfigSignal * pSignal = signalMap.at(pClickedItem);
            deleteSignalRow(pClickedItem, pSignal);
        } else {
            ConfigFrame* pFrame = this->frameMap.at(pClickedItem);
            deleteFrameRow(pClickedItem, pFrame);
        }
        reloadStartTriggersWidget();
        reloadStopTriggersWidget();
    } catch (const std::logic_error& e){
        QMessageBox::warning(this, "Error", e.what());
    } catch (const std::exception& e){
        QMessageBox::warning(this, "Error", QString("Unkonwn error occured: ") + QString(e.what()));
    }
}

void ConfigureLoggerSDDialog::on_actionDeleteStartLogTrigger_triggered()
{
    try {
        QListWidgetItem *pClickedItem = ui->actionEditStartLogTrigger->data().value<QListWidgetItem*>();
        deleteTriggerRow(pClickedItem, TriggerType::StartTrigger);
    } catch (const std::logic_error& e){
        QMessageBox::warning(this, "Error", e.what());
    } catch (const std::exception& e){
        QMessageBox::warning(this, "Error", QString("Unkonwn error occured: ") + QString(e.what()));
    }
}

void ConfigureLoggerSDDialog::on_actionEditStartLogTrigger_triggered()
{
    try {
        QListWidgetItem *pClickedItem = ui->actionEditStartLogTrigger->data().value<QListWidgetItem*>();
        editTriggerRow(pClickedItem, TriggerType::StartTrigger);
    } catch (const std::logic_error& e){
        QMessageBox::warning(this, "Error", e.what());
    } catch (const std::exception& e){
        QMessageBox::warning(this, "Error", QString("Unkonwn error occured: ") + QString(e.what()));
    }
}

void ConfigureLoggerSDDialog::on_startTrigger_listWidget_itemDoubleClicked(QListWidgetItem *pClickedItem)
{
    try {
        editTriggerRow(pClickedItem, TriggerType::StartTrigger);
    } catch (const std::logic_error& e){
        QMessageBox::warning(this, "Error", e.what());
    } catch (const std::exception& e){
        QMessageBox::warning(this, "Error", QString("Unkonwn error occured: ") + QString(e.what()));
    }
}

void ConfigureLoggerSDDialog::on_stopTrigger_listWidget_customContextMenuRequested(const QPoint &pos)
{
    try {
        QMenu menu(this); // add menu items
        menu.addAction(ui->actionEditStopLogTrigger);
        menu.addAction(ui->actionDeleteStopLogTrigger);

        QListWidgetItem *pClickedItem = ui->stopTrigger_listWidget->itemAt(pos);

        if (pClickedItem == nullptr){
            return;
        }

        QVariant v;
        v.setValue(pClickedItem);

        ui->actionDeleteStopLogTrigger->setData(v); //< save pointer to row for later use
        ui->actionEditStopLogTrigger->setData(v); //< save pointer to row for later use

        menu.exec(ui->stopTrigger_listWidget->mapToGlobal(pos));
    } catch (const std::logic_error& e){
        QMessageBox::warning(this, "Error", e.what());
    } catch (const std::exception& e){
        QMessageBox::warning(this, "Error", QString("Unkonwn error occured: ") + QString(e.what()));
    }
}

void ConfigureLoggerSDDialog::on_actionDeleteStopLogTrigger_triggered()
{
    try {
        QListWidgetItem *pClickedItem = ui->actionEditStopLogTrigger->data().value<QListWidgetItem*>();
        deleteTriggerRow(pClickedItem, TriggerType::StopTrigger);
    } catch (const std::logic_error& e){
        QMessageBox::warning(this, "Error", e.what());
    } catch (const std::exception& e){
        QMessageBox::warning(this, "Error", QString("Unkonwn error occured: ") + QString(e.what()));
    }

}

void ConfigureLoggerSDDialog::on_actionEditStopLogTrigger_triggered()
{
    try {
        QListWidgetItem *pClickedItem = ui->actionEditStopLogTrigger->data().value<QListWidgetItem*>();
        editTriggerRow(pClickedItem, TriggerType::StopTrigger);
    } catch (const std::logic_error& e){
        QMessageBox::warning(this, "Error", e.what());
    } catch (const std::exception& e){
        QMessageBox::warning(this, "Error", QString("Unkonwn error occured: ") + QString(e.what()));
    }

}

void ConfigureLoggerSDDialog::on_stopTrigger_listWidget_itemDoubleClicked(QListWidgetItem *pClickedItem)
{
    try {
        editTriggerRow(pClickedItem, TriggerType::StopTrigger);
    } catch (const std::logic_error& e){
        QMessageBox::warning(this, "Error", e.what());
    } catch (const std::exception& e){
        QMessageBox::warning(this, "Error", QString("Unkonwn error occured: ") + QString(e.what()));
    }
}

QListWidgetItem *ConfigureLoggerSDDialog::prepareTriggerListWidget(ConfigTrigger *pTrigger)

{
    QVariant v;
    QListWidgetItem* pTriggerWidgetItem = new QListWidgetItem(
                    QString::fromStdString(pTrigger->getTriggerName()) +
                    " (" +
                    TriggerDialog::prepareFormulaRender(pTrigger->getFrame(), pTrigger->getSignal(), pTrigger->getCompareOperator(), pTrigger->getCompareConstValue()) +
                    ")"
                );
    v.setValue<ConfigTrigger*>(pTrigger);
    pTriggerWidgetItem->setData(Qt::UserRole, v);
    return pTriggerWidgetItem;
}
