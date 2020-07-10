#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "senderthread.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(SenderThread* thr, QWidget *parent = 0);

    ~Widget();

private slots:
    void on_Widget_destroyed(QObject *arg1);

private:
    Ui::Widget *ui;
    SenderThread* thr;
};

#endif // WIDGET_H
