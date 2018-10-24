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
    void Changed();
public slots:
};

#endif // THREAD_H
