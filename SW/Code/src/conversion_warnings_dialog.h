#ifndef CONVERSION_WARNINGS_DIALOG_H
#define CONVERSION_WARNINGS_DIALOG_H

#include <QDialog>

#include "QStringIntMap.h"

namespace Ui {
class conversion_warnings_dialog;
}

class ConversionWarningsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConversionWarningsDialog(QStringIntMap warnings, QWidget *parent = nullptr);
    ~ConversionWarningsDialog();

private slots:
    void on_warningsListWidget_customContextMenuRequested(const QPoint &pos);

private:
    Ui::conversion_warnings_dialog *ui;
};

#endif // CONVERSION_WARNINGS_DIALOG_H
