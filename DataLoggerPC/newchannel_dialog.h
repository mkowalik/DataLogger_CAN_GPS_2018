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
    explicit NewChannelDialog(QWidget *parent = nullptr);
    explicit NewChannelDialog(ValueType valueType, int multipilier, int divider, int offset,
                              QString channelName, QString unitName, QString comment,
                              QWidget *parent = nullptr);
    ~NewChannelDialog();

    bool getIsSigned();
    bool getIs16Bit();
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

private:
    Ui::NewChannelDialog *ui;
};

#endif // NEWCHANNEL_DIALOG_H
