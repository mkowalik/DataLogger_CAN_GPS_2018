#include <QThread>
#include <QtCore>
#include "convertfile_thread.h"
#include <QMessageBox>

#include <AGHData/data_file_class.h>
#include <AGHConfig/WritingClass.h>

ConvertFileThread::ConvertFileThread(RawDataParser& parser, QObject *parent) :
    QThread(parent),
    Cancel(false),
    rawDataParser(parser),
    decimalSeparator(DEFAULT_DECIMAL_SEPARATOR),
    timingMode(DEFAULT_TIMING_MODE)
{

}

void ConvertFileThread::run(){

    Cancel = false;

    QFileInfo file(destinationDirectory);

    if(destinationDirectory.isEmpty() || !file.exists()){
        emit fatalErrorSignal();
        return;
    }

    int filesToConvert = filesList.length();
    int filesConverted = 0;

    while(!filesList.isEmpty() && !this->Cancel){

        QFileInfo actualFile = filesList.front();

        QString csvFilename = actualFile.fileName();
        csvFilename = csvFilename.remove(QRegExp("aghlog\\b"));
        csvFilename = csvFilename.append("csv");
        csvFilename = destinationDirectory + "/" + csvFilename;

        emit actualFileConverting(actualFile.absoluteFilePath(), csvFilename);

        qDebug() << csvFilename;

        try {
            DataFileClass dataFile;
            ReadingClass reader(actualFile.absoluteFilePath().toStdString(), rawDataParser);
            dataFile.read_from_bin(reader);

            WritingClass writer(csvFilename.toStdString(), rawDataParser);
            dataFile.write_to_csv(this->timingMode, writer, decimalSeparator);

            qDebug() << "DONE!";
        } catch (exception e){
            emit errorWhileConvertingPreviousFile();
            qDebug() << "EXCEPTION!";
        }
        filesList.pop_front();
        filesConverted++;

        emit actualProgress((filesConverted * 100) / filesToConvert);
    }
}

void ConvertFileThread::cancelExecution(){
    this->Cancel = true;
}

void ConvertFileThread::addFilesToConvert(QList<QFileInfo> filesList){
    this->filesList.append(filesList);
}

void ConvertFileThread::setDestinationDirectory(QString destination){
    this->destinationDirectory = destination;
}

void ConvertFileThread::setDecimaleSeparator(char aDecimalSeparaotr){
    if (aDecimalSeparaotr != ',' && aDecimalSeparaotr != '.'){
        throw invalid_argument("Decimal separator must be ',' or '.'.");
    }
    decimalSeparator = aDecimalSeparaotr;
}

void ConvertFileThread::setFileTimingMode(WritableToCSV::FileTimingMode mode){
    this->timingMode = mode;
}

/*Thread::~Thread(){
    //tutaj sprzatamy
}*/
