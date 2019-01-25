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
    static const WritableToCSV::FileTimingMode DEFAULT_TIMING_MODE = WritableToCSV::EventMode;
    bool                            Cancel;
    QList<QFileInfo>                filesList;
    QString                         destinationDirectory;
    RawDataParser                   rawDataParser;
    char                            decimalSeparator;
    WritableToCSV::FileTimingMode   timingMode;
public:
    explicit ConvertFileThread(RawDataParser& rawDataParser, QObject *parent = nullptr);
    void run();
    void addFilesToConvert(QList<QFileInfo> filesList);
    void setDestinationDirectory(QString destination);
    void setDecimaleSeparator(char decimalSeparaotr);
    void setFileTimingMode(WritableToCSV::FileTimingMode mode);
signals:
    void actualProgress(int percentageProgress);
    void actualFileConverting(QString name);
public slots:
    void cancel();
};

#endif // THREAD_H
