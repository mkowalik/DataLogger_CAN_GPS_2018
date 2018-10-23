#ifndef NEWFRAME_DIALOG_H
#define NEWFRAME_DIALOG_H

#include <QDialog>

namespace Ui {
class NewFrameDialog;
}

class NewFrameDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewFrameDialog(QWidget *parent = nullptr);
    ~NewFrameDialog();

private:
    Ui::NewFrameDialog *ui;
};

#endif // NEWFRAME_DIALOG_H
