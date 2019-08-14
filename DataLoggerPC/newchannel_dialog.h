#ifndef NEWCHANNEL_DIALOG_H
#define NEWCHANNEL_DIALOG_H

#include <QDialog>
#include "AGHConfig/Config.h"

namespace Ui {
class NewChannelDialog;
}

class NewChannelDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewChannelDialog(ConfigFrame& frame, ConfigChannel* pPreviousChannel, QWidget *parent = nullptr);
    explicit NewChannelDialog(ValueType valueType, int multipilier, int divider, int offset,
                              QString channelName, QString unitName, QString comment,
                              ConfigFrame& frame, ConfigChannel* pPreviousChannel, QWidget *parent = nullptr);
    ~NewChannelDialog();

    bool getIsSigned();
    bool getIs16Bit();
    bool getIsBigEndian();
    bool getIsInteger();
    bool getIsOnOff();
    bool getIsFlag();
    bool getIsCustom();

    int getMultiplier();
    int getDivider();
    int getOffset();
    QString getChannelName();
    QString getUnit();
    QString getComment();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::NewChannelDialog *ui;
    ConfigFrame& frame;
    ConfigChannel* pPreviousChannel;
};

#endif // NEWCHANNEL_DIALOG_H
