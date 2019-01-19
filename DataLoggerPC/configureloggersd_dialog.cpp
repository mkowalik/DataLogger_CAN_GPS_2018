#include "configureloggersd_dialog.h"
#include "ui_configureloggersd_dialog.h"

#include <QFileDialog>
#include <QDebug>
#include <QComboBox>
#include <QDir>
#include <QRegExp>
#include <QAction>
#include <QMenu>
#include <QMessageBox>

#include <iostream>

ConfigureLoggerSDDialog::ConfigureLoggerSDDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigureLoggerSDDialog),
    rawDataParser(RawDataParser::LittleEndian)
{
    ui->setupUi(this);
    ui->framesTreeWidget->header()->resizeSection(0, 220);
    ui->framesTreeWidget->header()->resizeSection(1, 50);
    ui->framesTreeWidget->header()->resizeSection(2, 65);
    ui->framesTreeWidget->header()->resizeSection(3, 60);
    ui->framesTreeWidget->header()->resizeSection(4, 50);
    ui->framesTreeWidget->header()->resizeSection(5, 50);
    ui->framesTreeWidget->header()->resizeSection(6, 80);
    ui->framesTreeWidget->header()->resizeSection(7, 120);
}

ConfigureLoggerSDDialog::~ConfigureLoggerSDDialog()
{
    delete ui;
}

void ConfigureLoggerSDDialog::on_newFrameButton_clicked()
{
    NewFrameDialog newFrameDialog(config, this);

    if (newFrameDialog.exec() == QDialog::Accepted){

        ConfigFrame fr;
        fr.set_ID(newFrameDialog.getID());
        fr.set_moduleName(newFrameDialog.getModuleName().toStdString());
        config.add_frame(fr);

        QTreeWidgetItem* item = new QTreeWidgetItem(ui->framesTreeWidget);

        prepareFrameWidget(fr, item);

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
    int index = ui->framesTreeWidget->indexOfTopLevelItem(selectedItem);
    if (index == -1){
        selectedItem = selectedItem->parent();
        index = ui->framesTreeWidget->indexOfTopLevelItem(selectedItem);
    }
    if (index == -1){
        QMessageBox::warning(this, "Select proper frame", "Select proper frame to which add the channel");
        return;
    }
    ConfigFrame& fr = config.get_frame_by_position(index);
    if (fr.get_DLC() == 8){
        QMessageBox::warning(this, "Frame is full", "Selected frame has 8 bytes defined. Remove channel or choose another frame.");
        return;
    }

    NewChannelDialog newChannelDialog(fr, nullptr, this);
    ConfigChannel ch;

    if (newChannelDialog.exec() == QDialog::Accepted){
        ValueType vt(newChannelDialog.getIsSigned(),
                     newChannelDialog.getIs16Bit(),
                     newChannelDialog.getIsOnOff(),
                     newChannelDialog.getIsFlag(),
                     newChannelDialog.getIsCustom());
        ch.set_valueType(vt);
        ch.set_multiplier(newChannelDialog.getMultiplier());
        ch.set_divider(newChannelDialog.getDivider());
        ch.set_offset(newChannelDialog.getOffset());
        ch.set_channelName(newChannelDialog.getChannelName().toStdString());
        ch.set_unitName(newChannelDialog.getUnit().toStdString());
        ch.set_comment(newChannelDialog.getComment().toStdString());
    }
    fr.add_channel(ch);

    QTreeWidgetItem* itemInner = new QTreeWidgetItem(selectedItem);

    prepareChannelWidget(ch, itemInner);

    prepareFrameWidget(fr, selectedItem);
}

void ConfigureLoggerSDDialog::on_selectOutputFileButton_clicked()
{
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
    QString prototypeFilePath = QFileDialog::getOpenFileName(this, "Choose Prototype File", "", "AGH Log (*.aghlog);AGH Config (*.aghconf)");

    ui->prototypeFileComboBox->addItem(prototypeFilePath);
    ui->prototypeFileComboBox->setCurrentText(prototypeFilePath);

    ReadingClass reader(prototypeFilePath.toStdString(), rawDataParser);
    config.reset();
    config.read_from_bin(reader);   //TODO sprawdzic czy sukces

    reloadFramesTreeWidget();
}

void ConfigureLoggerSDDialog::reloadFramesTreeWidget(){

    ui->framesTreeWidget->clear();

    for (auto it = config.get_frames_begin_citerator(); it!=config.get_frames_end_citerator(); it++){

        QTreeWidgetItem* item = new QTreeWidgetItem(ui->framesTreeWidget);

        prepareFrameWidget(*it, item);

        ui->framesTreeWidget->addTopLevelItem(item);

        for (auto itCh = it->get_channels_begin_iterator(); itCh != it->get_channels_end_iterator(); itCh++){

            QTreeWidgetItem* itemInner = new QTreeWidgetItem(item);

            prepareChannelWidget(*itCh, itemInner);

        }
    }
}

void ConfigureLoggerSDDialog::prepareFrameWidget(const ConfigFrame& frame, QTreeWidgetItem* pWidget){

    const static int baseForFrameID = 16;

    QString text = QString::fromStdString("0x");
    text.append(QString::number(frame.get_ID(), baseForFrameID));
    text.append(" ");
    text.append(QString::fromStdString(frame.get_moduleName()));
    text.append(" (DLC: ");
    text.append(QString::number(frame.get_DLC()));
    text.append(")");

    pWidget->setText(0, text);

}

void ConfigureLoggerSDDialog::prepareChannelWidget(const ConfigChannel& channel, QTreeWidgetItem* pWidget){

    pWidget->setText(0, QString::fromStdString(channel.get_channelName()));
    pWidget->setText(1, QString::number(channel.get_DLC()));
    pWidget->setText(2, QString("0x") + QString::number(channel.get_valueType().getHexValue(), 16));
    pWidget->setText(3, QString::number(channel.get_multiplier()));
    pWidget->setText(4, QString::number(channel.get_divider()));
    pWidget->setText(5, QString::number(channel.get_offset()));
    pWidget->setText(6, QString::fromStdString(channel.get_unitName()));
    pWidget->setText(7, QString::fromStdString(channel.get_comment()));

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

void ConfigureLoggerSDDialog::on_framesTreeWidget_itemDoubleClicked(QTreeWidgetItem *clickedItem, int column)
{
    QTreeWidgetItem *parent = clickedItem->parent();

    editGivenItem(clickedItem, parent);
}

void ConfigureLoggerSDDialog::editGivenItem(QTreeWidgetItem *clickedItem, QTreeWidgetItem* parent){

    if (!parent){

        int frameIndex = ui->framesTreeWidget->indexOfTopLevelItem(clickedItem);
        ConfigFrame& fr = config.get_frame_by_position(frameIndex);

        NewFrameDialog frameDialog(fr.get_ID(), QString::fromStdString(fr.get_moduleName()), config, this);

        if (frameDialog.exec() == QDialog::Accepted){
            fr.set_ID(frameDialog.getID());
            fr.set_moduleName(frameDialog.getModuleName().toStdString());
        }

        prepareFrameWidget(fr, clickedItem);

    } else {

        int frameIndex = ui->framesTreeWidget->indexOfTopLevelItem(parent);
        ConfigFrame& fr = config.get_frame_by_position(frameIndex);

        int channelIndex = parent->indexOfChild(clickedItem);
        ConfigChannel& ch = fr.get_channel_by_position(channelIndex);

        NewChannelDialog channelDialog(ch.get_valueType(), ch.get_multiplier(), ch.get_divider(), ch.get_offset(),
                                       QString::fromStdString(ch.get_channelName()), QString::fromStdString(ch.get_unitName()),
                                       QString::fromStdString(ch.get_comment()), fr, &ch, this);

        if (channelDialog.exec() == QDialog::Accepted){
            ValueType vt(channelDialog.getIsSigned(),
                         channelDialog.getIs16Bit(),
                         channelDialog.getIsOnOff(),
                         channelDialog.getIsFlag(),
                         channelDialog.getIsCustom());
            ch.set_valueType(vt);
            ch.set_multiplier(channelDialog.getMultiplier());
            ch.set_divider(channelDialog.getDivider());
            ch.set_offset(channelDialog.getOffset());
            ch.set_channelName(channelDialog.getChannelName().toStdString());
            ch.set_unitName(channelDialog.getUnit().toStdString());
            ch.set_comment(channelDialog.getComment().toStdString());
        }

        prepareFrameWidget(fr, parent);

        prepareChannelWidget(ch, clickedItem);

    }

}

void ConfigureLoggerSDDialog::on_actionDelete_triggered()
{
    QTreeWidgetItem *clickedItem = ui->framesTreeWidget->itemAt(ui->actionDelete->data().toPoint());

    QTreeWidgetItem *parent = clickedItem->parent();

    if (!parent){
        int frameIndex = ui->framesTreeWidget->indexOfTopLevelItem(clickedItem);

        if (QMessageBox::question(this, "Delete frame", "Do you relly want to delete this frame?") == QMessageBox::StandardButton::Yes){
            config.remove_frame_by_position(frameIndex);
            ui->framesTreeWidget->takeTopLevelItem(frameIndex);
        }
    } else {
        int frameIndex = ui->framesTreeWidget->indexOfTopLevelItem(parent);
        ConfigFrame& fr = config.get_frame_by_position(frameIndex);

        int channelIndex = parent->indexOfChild(clickedItem);

        if (QMessageBox::question(this, "Delete channel", "Do you relly want to delete this channel?") == QMessageBox::StandardButton::Yes){
            fr.remove_channel_by_position(channelIndex);
            parent->removeChild(clickedItem);

            prepareFrameWidget(fr, parent);
        }
    }

}

void ConfigureLoggerSDDialog::on_resetButton_clicked()
{
    if (QMessageBox::question(this, "Reset", "Do you really want to reset all frames and channels?") == QMessageBox::Yes){
        ui->prototypeFileComboBox->setCurrentText("");
        config.reset();
    }
    reloadFramesTreeWidget();
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
    config.write_to_bin(writer);

    QMessageBox::information(this, "Config export completed", "Your config has ben exported to \"" + filePath + "\" file.");
}
