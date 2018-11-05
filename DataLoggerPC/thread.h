#ifndef THREAD_H
#define THREAD_H

#include <QThread>

class Thread : public QThread
{
    Q_OBJECT
public:
    explicit Thread(QObject *parent = nullptr);
    void run();
    bool Cancel;
signals:
    void getData(const int& number, QString name);
    void Changed();
public slots:

private slots:
    void on_click();
};

#endif // THREAD_H
