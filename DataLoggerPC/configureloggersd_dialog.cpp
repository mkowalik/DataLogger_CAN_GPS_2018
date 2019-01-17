#include "configureloggersd_dialog.h"
#include "ui_configureloggersd_dialog.h"

#include <QFileDialog>
#include <QDebug>
#include <QComboBox>
#include <QDir>
#include <QRegExp>
#include <QAction>
#include <QMenu>

#include <iostream>

ConfigureLoggerSDDialog::ConfigureLoggerSDDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigureLoggerSDDialog),
    rawDataParser(RawDataParser::LittleEndian)
{
    ui->setupUi(this);

}

ConfigureLoggerSDDialog::~ConfigureLoggerSDDialog()
{
    delete ui;
}

void ConfigureLoggerSDDialog::on_newFrameButton_clicked()
{
    NewFrameDialog newFrameDialog(this);
    newFrameDialog.exec();
}

void ConfigureLoggerSDDialog::on_addChannelButton_clicked()
{
    NewChannelDialog newChannelDialog(this);
    newChannelDialog.exec();
}

void ConfigureLoggerSDDialog::on_selectOutputFileButton_clicked()
{
    QString outputFilePath = QFileDialog::getSaveFileName(this, "Choose Output File");

    int position = outputFilePath.lastIndexOf(QRegExp("/.+..+"), -1);
    outputFilePath.remove(position, outputFilePath.length());
    outputFilePath.append("/logger.aghconf");

    ui->selectOutputFileComboBox->addItem(outputFilePath);
    ui->selectOutputFileComboBox->setCurrentText(outputFilePath);

}

void ConfigureLoggerSDDialog::on_selectPrototypeFileButton_clicked()
{
    QString prototypeFilePath = QFileDialog::getOpenFileName(this, "Choose Prototype File", "", "AGH Log (*.aghlog);AGH Config (*.aghconf)");

    ui->prototypeFileComboBox->addItem(prototypeFilePath);
    ui->prototypeFileComboBox->setCurrentText(prototypeFilePath);

    ReadingClass reader(prototypeFilePath.toStdString(), rawDataParser);
    config.read_from_bin(reader);   //TODO sprawdzic czy sukces

    reloadFramesTreeWidget();
}

void ConfigureLoggerSDDialog::reloadFramesTreeWidget(){

    const static int baseForFrameID = 16;

    ui->framesTreeWidget->clear();

    for (auto it = config.get_frames_begin_citerator(); it!=config.get_frames_end_citerator(); it++){

        QString text = QString::fromStdString("0x");
        text.append(QString::number(it->get_ID(), baseForFrameID));
        text.append(" ");
        text.append(QString::fromStdString(it->get_moduleName()));

        QTreeWidgetItem* item = new QTreeWidgetItem(ui->framesTreeWidget);

        item->setText(0, text);

        ui->framesTreeWidget->addTopLevelItem(item);

        for (auto itCh = it->get_channels_begin_iterator(); itCh != it->get_channels_end_iterator(); itCh++){

            QTreeWidgetItem* itemInner = new QTreeWidgetItem(item);

            itemInner->setText(0, QString::fromStdString(itCh->get_channelName()));
            itemInner->setText(1, QString::number(itCh->get_multiplier()));
            itemInner->setText(2, QString::number(itCh->get_divider()));
            itemInner->setText(3, QString::number(itCh->get_offset()));
            itemInner->setText(4, QString::fromStdString(itCh->get_unitName()));
            itemInner->setText(5, QString::fromStdString(itCh->get_comment()));
        }
    }

}

void ConfigureLoggerSDDialog::on_framesTreeWidget_customContextMenuRequested(const QPoint &pos)
{
    QMenu menu(this); // add menu items
    menu.addAction(ui->actionEdit);
    menu.addAction(ui->actionDelete);

    ui->actionDelete->setData(QVariant(pos)); // if you will need the position data save it to the action
    ui->actionEdit->setData(QVariant(pos)); // if you will need the position data save it to the action

    menu.exec( ui->framesTreeWidget->mapToGlobal(pos) );
}

void ConfigureLoggerSDDialog::on_actionEdit_triggered()
{
    QTreeWidgetItem *clickedItem = ui->framesTreeWidget->itemAt(ui->actionDelete->data().toPoint());

    QTreeWidgetItem *parent = clickedItem->parent();

    if (!parent){

        int frameIndex = ui->framesTreeWidget->indexOfTopLevelItem(clickedItem);
        ConfigFrame& fr = config.get_frame_by_position(frameIndex);

        NewFrameDialog frameDialog(fr.get_ID(), QString::fromStdString(fr.get_moduleName()), this);

        if (frameDialog.exec() == QDialog::Accepted){
            fr.set_ID(frameDialog.getID());
            fr.set_moduleName(frameDialog.getModuleName().toStdString());
        }
    } else {

        int frameIndex = ui->framesTreeWidget->indexOfTopLevelItem(parent);
        ConfigFrame& fr = config.get_frame_by_position(frameIndex);

        int channelIndex = parent->indexOfChild(clickedItem);
        ConfigChannel& ch = fr.get_channel_by_position(channelIndex);

        NewChannelDialog channelDialog(ch.get_valueType(), ch.get_multiplier(), ch.get_divider(), ch.get_offset(),
                                       QString::fromStdString(ch.get_channelName()), QString::fromStdString(ch.get_unitName()),
                                       QString::fromStdString(ch.get_comment()), this);
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
    }

    reloadFramesTreeWidget();

}
