#ifndef TRIGGER_DIALOG_H
#define TRIGGER_DIALOG_H

#include <QDialog>

namespace Ui {
class TriggerDialog;
}

class TriggerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TriggerDialog(QWidget *parent = nullptr);
    ~TriggerDialog();

private:
    Ui::TriggerDialog *ui;
};

#endif // TRIGGER_DIALOG_H
