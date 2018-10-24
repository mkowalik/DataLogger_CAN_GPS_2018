#include <QThread>
#include <QtCore>
#include "thread.h"

Thread::Thread(QObject *parent) : QThread(parent)
{

}

void Thread::run(){
    int i = 0;
    while(!this->Cancel){
        qDebug() << "Running" << i;
        i++;
    }
    emit Changed();
}
