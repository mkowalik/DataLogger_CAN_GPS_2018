#ifndef NEWCHANNEL_DIALOG_H
#define NEWCHANNEL_DIALOG_H

#include <QDialog>
#include "AGHConfig/Config.h"
#include "AGHConfig/ConfigSignal.h"

namespace Ui {
class SignalDialog;
}

class SignalDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SignalDialog(ConfigFrame& frame, ConfigSignal* pPreviousSignal, QWidget *parent = nullptr);
    ~SignalDialog();

    unsigned int getSignalID();
    bool         getAutoSignalID();
    bool         getIsSigned();
    bool         getIsBigEndian();
    int          getMultiplier();
    unsigned int getDivider();
    int          getOffset();
    QString      getSignalName();
    QString      getUnit();
    QString      getComment();
    unsigned int getStartBit();
    unsigned int getLengthBits();

private slots:
    void on_signalIDAuto_checkBox_stateChanged(int state);

    void on_lengthBits_spinBox_valueChanged(int arg1);

private:
    Ui::SignalDialog *ui;
    ConfigFrame& frame;
    ConfigSignal* pPreviousSignal;
};

#endif // NEWCHANNEL_DIALOG_H
