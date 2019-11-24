#ifndef NEWFRAME_DIALOG_H
#define NEWFRAME_DIALOG_H

#include <QDialog>
#include <AGHConfig/Config.h>

namespace Ui {
class NewFrameDialog;
}

class NewFrameDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewFrameDialog(Config& config, QWidget *parent = nullptr);
    explicit NewFrameDialog(unsigned int id, QString moduleName, Config& config, QWidget *parent = nullptr);
    ~NewFrameDialog();
    unsigned int getID();
    QString getModuleName();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::NewFrameDialog* ui;
    Config&             config;
    unsigned int        editingId;
    bool                isNewFrame;
};

#endif // NEWFRAME_DIALOG_H
