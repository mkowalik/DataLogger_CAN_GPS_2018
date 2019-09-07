#ifndef THREAD_H
#define THREAD_H

#include <QThread>
#include <QFileInfo>
#include "AGHData/raw_data_parser.h"
#include "AGHConfig/WritingClass.h"

class ConvertFileThread : public QThread
{
    Q_OBJECT
private:
    static const char               DEFAULT_DECIMAL_SEPARATOR = ',';
    static const WritableToCSV::FileTimingMode DEFAULT_TIMING_MODE = WritableToCSV::FileTimingMode::EventMode;
    bool                            Cancel;
    QList<QFileInfo>                filesList;
    QString                         destinationDirectory;
    RawDataParser                   rawDataParser;
    char                            decimalSeparator;
    WritableToCSV::FileTimingMode   timingMode;
    bool                            writeOnlyChangedValues;
public:
    explicit ConvertFileThread(RawDataParser& rawDataParser, QObject *parent = nullptr);
    void run();
    void addFilesToConvert(QList<QFileInfo> filesList);
    void setDestinationDirectory(QString destination);
    void setDecimaleSeparator(char decimalSeparaotr);
    void setFileTimingMode(WritableToCSV::FileTimingMode mode);
    void setWriteOnlyChangedValues();
    void setWriteOnlyChangedValues(bool val);
signals:
    void actualProgress(int percentageProgress);
    void actualFileConverting(QString sourceName, QString destinationName);
    void errorWhileConvertingPreviousFile(QString reasonDescription);
    void fatalErrorSignal();
public slots:
    void cancelExecution();
};

#endif // THREAD_H
