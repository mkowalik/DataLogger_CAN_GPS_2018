#ifndef NEWCHANNEL_DIALOG_H
#define NEWCHANNEL_DIALOG_H

#include <QDialog>

namespace Ui {
class NewChannelDialog;
}

class NewChannelDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewChannelDialog(QWidget *parent = nullptr);
    ~NewChannelDialog();

private:
    Ui::NewChannelDialog *ui;
};

#endif // NEWCHANNEL_DIALOG_H
