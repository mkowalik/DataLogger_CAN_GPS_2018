#include "configure_logger_sd_dialog.h"
#include "ui_configure_logger_sd_dialog.h"

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
    ui->framesTreeWidget->header()->resizeSection(1, 30);
    ui->framesTreeWidget->header()->resizeSection(2, 65);
    ui->framesTreeWidget->header()->resizeSection(3, 60);
    ui->framesTreeWidget->header()->resizeSection(4, 50);
    ui->framesTreeWidget->header()->resizeSection(5, 50);
    ui->framesTreeWidget->header()->resizeSection(6, 80);
    ui->framesTreeWidget->header()->resizeSection(7, 120);

    this->pConfig = new Config();

    reloadConfigView();
}

ConfigureLoggerSDDialog::~ConfigureLoggerSDDialog()
{
    delete this->pConfig;
    delete ui;
}

void ConfigureLoggerSDDialog::on_selectOutputFileButton_clicked()
{
    //TODO przerobic na zwykle okno, zeby sprawdzic czy user nie kliknal cancel
    QString outputFilePath = QFileDialog::getSaveFileName(this, "Choose Output File", "", "AGH Config (*.aghconf)");

    if (!outputFilePath.endsWith(".aghconf")){
        QMessageBox::warning(this, "Wrong file name", "Given file does not have \".aghconf\" extension. Choose proper file.");
        return;
    }

    ui->selectOutputFileComboBox->addItem(outputFilePath);
    ui->selectOutputFileComboBox->setCurrentText(outputFilePath);
}

void ConfigureLoggerSDDialog::on_selectPrototypeFileButton_clicked()
{
    QString prototypeFilePath = QFileDialog::getOpenFileName(this, "Choose Prototype File", "", tr("AGH Config/Log (*.aghconf *.aghlog)"));

    if (!(prototypeFilePath.endsWith(".aghconf") || prototypeFilePath.endsWith(".aghlog"))){
        QMessageBox::warning(this, "Wrong file name", "Given file does not have \".aghconf\" or \".aghlog\" extension. Choose proper file.");
        return;
    }

    ReadingClass reader(prototypeFilePath.toStdString(), rawDataParser);
    pConfig->reset();
    try {
        pConfig->readFromBin(reader);
    } catch(std::logic_error e){
        QMessageBox::warning(this, "Prototype file problem.", QString("Problem with reading prototype file. ") + QString(e.what()));
        return;
    }

    ui->prototypeFileComboBox->addItem(prototypeFilePath);
    ui->prototypeFileComboBox->setCurrentText(prototypeFilePath);

    reloadConfigView();
}

void ConfigureLoggerSDDialog::on_resetButton_clicked()
{
    if (QMessageBox::question(this, "Reset", "Do you really want to reset all frames and channels?") == QMessageBox::Yes){
        ui->prototypeFileComboBox->setCurrentText("");
        pConfig->reset();
    }
    reloadConfigView();
}

void ConfigureLoggerSDDialog::on_framesTreeWidget_customContextMenuRequested(const QPoint &pos)
{
    QMenu menu(this); // add menu items
    menu.addAction(ui->actionEdit);
    menu.addAction(ui->actionDelete);

    ui->actionDelete->setData(QVariant(pos)); // if you will need the position data save it to the action
    ui->actionEdit->setData(QVariant(pos)); // if you will need the position data save it to the action

    menu.exec(ui->framesTreeWidget->mapToGlobal(pos));
}

void ConfigureLoggerSDDialog::on_actionEdit_triggered()
{
    QTreeWidgetItem *clickedItem = ui->framesTreeWidget->itemAt(ui->actionDelete->data().toPoint());
    QTreeWidgetItem *parent = clickedItem->parent();
    editGivenItem(clickedItem, parent);
}

void ConfigureLoggerSDDialog::on_actionDelete_triggered()
{
    QTreeWidgetItem *clickedItem = ui->framesTreeWidget->itemAt(ui->actionDelete->data().toPoint());

    QTreeWidgetItem *parent = clickedItem->parent();

    if (!parent){

        unsigned int frameId = this->idMap.at(clickedItem);

        if (QMessageBox::question(this, "Delete frame", "Do you relly want to delete this frame?") == QMessageBox::StandardButton::Yes){
            pConfig->removeFrameById(frameId);
            this->idMap.erase(clickedItem);
            ui->framesTreeWidget->takeTopLevelItem(ui->framesTreeWidget->indexOfTopLevelItem(clickedItem));
        }
    } else {
        unsigned int frameId = this->idMap.at(parent);
        ConfigFrame* pFrame = this->pConfig->getFrameById(frameId);

        unsigned int channelIndex = static_cast<unsigned int>(parent->indexOfChild(clickedItem));

        if (QMessageBox::question(this, "Delete channel", "Do you relly want to delete this channel?") == QMessageBox::StandardButton::Yes){
            pFrame->removeAndDeleteSignalByPosition(channelIndex);
            parent->removeChild(clickedItem);
            prepareFrameWidget(*pFrame, parent);
        }
    }
}

void ConfigureLoggerSDDialog::on_framesTreeWidget_itemDoubleClicked(QTreeWidgetItem *clickedItem, int)
{
    QTreeWidgetItem *parent = clickedItem->parent();
    editGivenItem(clickedItem, parent);
}

void ConfigureLoggerSDDialog::on_newFrameButton_clicked()
{
    NewFrameDialog newFrameDialog(*(this->pConfig), this);

    if (newFrameDialog.exec() == QDialog::Accepted){

        ConfigFrame* pFrame = new ConfigFrame(newFrameDialog.getID(), 0, newFrameDialog.getModuleName().toStdString()); //TODO tutaj wpisać DLC
        this->pConfig->addFrame(pFrame);

        QTreeWidgetItem* item = new QTreeWidgetItem(ui->framesTreeWidget);

        prepareFrameWidget(*pFrame, item);

        idMap.insert(make_pair(item, pFrame->getID()));

        ui->framesTreeWidget->addTopLevelItem(item);
    }
}

void ConfigureLoggerSDDialog::on_addChannelButton_clicked()
{
    QList<QTreeWidgetItem*> selectedList = ui->framesTreeWidget->selectedItems();
    if (selectedList.isEmpty()){
        QMessageBox::warning(this, "Select frame", "Select frame to which add the channel");
        return;
    }
    QTreeWidgetItem* selectedItem = selectedList[0];
    if (ui->framesTreeWidget->indexOfTopLevelItem(selectedItem) == -1){
        selectedItem = selectedItem->parent();
    }
    if (ui->framesTreeWidget->indexOfTopLevelItem(selectedItem) == -1){
        QMessageBox::warning(this, "Select proper frame", "Select proper frame to which add the channel");
        return;
    }
    unsigned int id = idMap.at(selectedItem);
    ConfigFrame* pFrame = this->pConfig->getFrameById(id);//TODO try catch
    if (pFrame->getDLC() == 8){
        QMessageBox::warning(this, "Frame is full", "Selected frame has 8 bytes defined. Remove channel or choose another frame.");
        return;
    }

    NewChannelDialog newChannelDialog(*pFrame, nullptr, this);
    ConfigSignal* pChannel = new ConfigSignal(pFrame);

    if (newChannelDialog.exec() == QDialog::Accepted){
        ValueType vt(newChannelDialog.getIsSigned(),
                     newChannelDialog.getIs16Bit(),
                     newChannelDialog.getIsOnOff(),
                     newChannelDialog.getIsFlag(),
                     newChannelDialog.getIsCustom(),
                     newChannelDialog.getIsBigEndian());
        pChannel->setValueType(vt);
        pChannel->setMultiplier(newChannelDialog.getMultiplier());
        pChannel->setDivider(newChannelDialog.getDivider());
        pChannel->setOffset(newChannelDialog.getOffset());
        pChannel->setSignallName(newChannelDialog.getChannelName().toStdString());
        pChannel->setUnitName(newChannelDialog.getUnit().toStdString());
        pChannel->setComment(newChannelDialog.getComment().toStdString());

        pFrame->setDLC(pFrame->getDLC() + (pChannel->getValueType().is16BitLength() ? 2 : 1));

        QTreeWidgetItem* itemInner = new QTreeWidgetItem(selectedItem);

        prepareChannelWidget(*pChannel, itemInner);
        prepareFrameWidget(*pFrame, selectedItem);
    }
}

void ConfigureLoggerSDDialog::on_saveConfigButton_clicked()
{
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

void ConfigureLoggerSDDialog::reloadFramesTreeWidget(){

    ui->framesTreeWidget->clear();

    for (Config::iterator it = pConfig->begin(); it!=pConfig->end(); it++){

        QTreeWidgetItem* item = new QTreeWidgetItem(ui->framesTreeWidget);
        prepareFrameWidget(**it, item);
        ui->framesTreeWidget->addTopLevelItem(item);

        idMap.insert(make_pair(item, it->getID()));

        for (auto itCh = it->begin(); itCh != it->end(); itCh++){
            QTreeWidgetItem* itemInner = new QTreeWidgetItem(item);
            prepareChannelWidget(**itCh, itemInner);
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

void ConfigureLoggerSDDialog::reloadConfigView(){
    reloadFramesTreeWidget();
    reloadCANBusBitrateWidget();
    reloadGPSFrequencyWidget();
}

void ConfigureLoggerSDDialog::editGivenItem(QTreeWidgetItem *clickedItem, QTreeWidgetItem* parent){

    if (!parent){

        unsigned int id = this->idMap.at(clickedItem);
        ConfigFrame& fr = *(this->pConfig->getFrameById(id));

        NewFrameDialog frameDialog(fr.getID(), QString::fromStdString(fr.getModuleName()), *(this->pConfig), this);

        if (frameDialog.exec() == QDialog::Accepted){
            fr.setID(frameDialog.getID());
            fr.setModuleName(frameDialog.getModuleName().toStdString());
        }

        prepareFrameWidget(fr, clickedItem);

    } else {

        unsigned int id = idMap.at(parent);
        ConfigFrame& fr = *(this->pConfig->getFrameById(id));

        unsigned int channelIndex = parent->indexOfChild(clickedItem);
        ConfigSignal& ch = fr.getSignalByPosition(channelIndex);

        NewChannelDialog channelDialog(ch.getValueType(), ch.getMultiplier(), ch.getDivider(), ch.getOffset(),
                                       QString::fromStdString(ch.getSignalName()), QString::fromStdString(ch.getUnitName()),
                                       QString::fromStdString(ch.getComment()), fr, &ch, this);

        if (channelDialog.exec() == QDialog::Accepted){
            ValueType vt(channelDialog.getIsSigned(),
                         channelDialog.getIs16Bit(),
                         channelDialog.getIsOnOff(),
                         channelDialog.getIsFlag(),
                         channelDialog.getIsCustom(),
                         channelDialog.getIsBigEndian());
            ch.setValueType(vt);
            ch.setMultiplier(channelDialog.getMultiplier());
            ch.setDivider(channelDialog.getDivider());
            ch.setOffset(channelDialog.getOffset());
            ch.setSignallName(channelDialog.getChannelName().toStdString());
            ch.setUnitName(channelDialog.getUnit().toStdString());
            ch.setComment(channelDialog.getComment().toStdString());

            prepareFrameWidget(fr, parent);
            prepareChannelWidget(ch, clickedItem);
        }
    }
}

void ConfigureLoggerSDDialog::prepareFrameWidget(const ConfigFrame& frame, QTreeWidgetItem* pWidget){

    const static int baseForFrameID = 16;

    QString text = QString::fromStdString("0x");
    text.append(QString::number(frame.getID(), baseForFrameID));
    text.append(" ");
    text.append(QString::fromStdString(frame.getModuleName()));
    text.append(" (DLC: ");
    text.append(QString::number(frame.getDLC()));
    text.append(")");

    pWidget->setText(0, text);

}

void ConfigureLoggerSDDialog::prepareChannelWidget(const ConfigSignal& channel, QTreeWidgetItem* pWidget){

    pWidget->setText(0, QString::fromStdString(channel.getSignalName()));
    pWidget->setText(1, QString::number(channel.getLengthBits()));
    pWidget->setText(2, QString("0x") + QString::number(channel.getValueType().getHexValue(), 16));
    pWidget->setText(3, QString::number(channel.getMultiplier()));
    pWidget->setText(4, QString::number(channel.getDivider()));
    pWidget->setText(5, QString::number(channel.getOffset()));
    pWidget->setText(6, QString::fromStdString(channel.getUnitName()));
    pWidget->setText(7, QString::fromStdString(channel.getComment()));

}

void ConfigureLoggerSDDialog::on_canSpeedComboBox_currentTextChanged(const QString &val)
{
    this->pConfig->setCANBitrate(this->stringToCANBitrate(val.toStdString()));
}

void ConfigureLoggerSDDialog::on_gpsFreqComboBox_currentTextChanged(const QString &val)
{
    this->pConfig->setGPSFrequency(this->stringToGPSFrequency(val.toStdString()));
}
