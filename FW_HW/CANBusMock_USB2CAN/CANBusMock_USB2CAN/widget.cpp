#include "widget.h"
#include "ui_widget.h"
#include <QSignalMapper>

Widget::Widget(SenderThread* thr, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget),
    thr(thr)
{
    ui->setupUi(this);

}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_Widget_destroyed(QObject *arg1)
{
    thr->terminate();
}
