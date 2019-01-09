#include <QThread>
#include <QtCore>
#include "thread.h"
#include <QMessageBox>

Thread::Thread(QObject *parent) : QThread(parent)
{

}

//ta funkcja ma zwracac procent przetworzonych danych
int getPercantage(){
    static int i=0;
    return (i++)%100;
}

//ta funkcja ma zwracac nazwe pliku
QString getName(){
    return QString("dupa");
}

void Thread::jakasMagicznaFunkcja(){
    int i = 0;
    for (int j=0; i<30; j++){
        i++;
        QThread::msleep(100);
        qDebug() << "Running" << i;
        emit dataChanged(getPercantage(), getName());
    }
}

void Thread::run(){
    while(!this->Cancel){
        jakasMagicznaFunkcja();
    }
}

/*Thread::~Thread(){
    //tutaj sprzatamy
}*/
