#include <QThread>
#include <QtCore>
#include "convertfile_thread.h"
#include <QMessageBox>

#include "AGHData/DataFileClass.h"
#include "AGHUtils/WritingClass.h"

ConvertFileThread::ConvertFileThread(RawDataParser& parser, QObject *parent) :
    QThread(parent),
    Cancel(false),
    rawDataParser(parser),
    decimalSeparator(DEFAULT_DECIMAL_SEPARATOR),
    timingMode(DEFAULT_TIMING_MODE),
    writeOnlyChangedValues(true)
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

        QStringIntMap warnings;
        try {
            DataFileClass dataFile;
            ReadingClass reader(actualFile.absoluteFilePath().toStdString(), rawDataParser);
            dataFile.readFromBin(reader);

            WritingClass writer(csvFilename.toStdString(), rawDataParser);
            std::map<std::string, unsigned int> warningsRaw = dataFile.write_to_csv(this->timingMode, writer, decimalSeparator, writeOnlyChangedValues);

            std::for_each(warningsRaw.begin(), warningsRaw.end(), [&](auto p){warnings.insert(QString::fromStdString(p.first), p.second);});

            qDebug() << "DONE!";
        } catch (std::logic_error& e){
            emit errorWhileConvertingPreviousFile(QString(e.what()));
            qDebug() << "EXCEPTION!";
        }

        emit warningsWhileConvertingPreviousFile(warnings);

        filesList.pop_front();
        filesConverted++;

        emit actualProgress((filesConverted * 100) / filesToConvert);
    }
    exit();
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

void ConvertFileThread::setWriteOnlyChangedValues(bool val){
    this->writeOnlyChangedValues = val;
}
