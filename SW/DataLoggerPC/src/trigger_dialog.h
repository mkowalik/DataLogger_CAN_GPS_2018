#ifndef TRIGGER_DIALOG_H
#define TRIGGER_DIALOG_H

#include <QDialog>

#include "AGHConfig/Config.h"
#include "AGHConfig/ConfigFrame.h"
#include "AGHConfig/ConfigTrigger.h"

namespace Ui {
class TriggerDialog;
}

class TriggerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TriggerDialog(Config& config, ConfigTrigger* pTriggerPrototype, QWidget *parent = nullptr);


    ConfigFrame*                            getSelectedFrame();
    ConfigSignal*                           getSelectedSignal();
    ConfigTrigger::TriggerCompareOperator   getCompareOperator();
    unsigned int                            getConstCompareValue();
    QString                                 getTriggerName();
    QString                                 getFormulaRenderValue();

    ~TriggerDialog();

    static QString prepareFormulaRender(const ConfigFrame* pFrame, const ConfigSignal* pSignal, ConfigTrigger::TriggerCompareOperator oper, unsigned long rawConstCompareVal);
private slots:
    void on_signal_comboBox_currentIndexChanged(int index);
    void on_frame_comboBox_currentIndexChanged(int index);
    void on_symbolicConstValue_doubleSpinBox_valueChanged(double val);
    void on_rawConstValue_spinBox_valueChanged(int val);
    void on_compareOperator_comboBox_currentIndexChanged(int index);

private:
    Ui::TriggerDialog *ui;
    Config& config;
    ConfigTrigger* pTriggerPrototype;
    bool constValueChangeInProgress;

    void disableSignalReltiveWidgets(bool diableSignalNameWidget);
    void enableSignalReltiveWidgets(const ConfigSignal* pSignal);
    void reloadFormulaRender();
};

Q_DECLARE_METATYPE(ConfigFrame*)
Q_DECLARE_METATYPE(ConfigSignal*)
Q_DECLARE_METATYPE(ConfigTrigger::TriggerCompareOperator)

#endif // TRIGGER_DIALOG_H
