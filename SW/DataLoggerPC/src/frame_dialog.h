#ifndef NEWFRAME_DIALOG_H
#define NEWFRAME_DIALOG_H

#include <QDialog>
#include <AGHConfig/Config.h>

namespace Ui {
class FrameDialog;
}

class FrameDialog : public QDialog
{
    Q_OBJECT

public:
    FrameDialog(const Config& config, const ConfigFrame* pFrame, QWidget *parent);
    ~FrameDialog();
    unsigned int    getFrameID();
    QString         getModuleName();

private:
    Ui::FrameDialog*    ui;
    const Config&       config;
    const ConfigFrame*  pFrame;
};

#endif // NEWFRAME_DIALOG_H
