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
    void jakasMagicznaFunkcja();
signals:
    void dataChanged(const int& number, QString name);
};

#endif // THREAD_H
