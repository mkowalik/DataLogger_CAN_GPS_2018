#include <QThread>
#include <QtCore>
#include "thread.h"
#include <QMessageBox>

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
//ta funkcja ma zwracac procent przetworzonych danych
int getPercantage(){
    return 30;
}

//ta funkcja ma zwracac nazwe pliku
QString getName(){
    return QString("dupa");
}

//ten slot powinnien byc w pewnym momencie uruchamiany ale jak?
void Thread::on_click(){
    int number(getPercantage());
    QString name(getName());
    emit getData(number,name);
}



