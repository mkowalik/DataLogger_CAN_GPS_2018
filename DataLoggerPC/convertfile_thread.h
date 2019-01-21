#ifndef THREAD_H
#define THREAD_H

#include <QThread>
#include <QFileInfo>
#include <AGHData/raw_data_parser.h>

class ConvertFileThread : public QThread
{
    Q_OBJECT
private:
    bool Cancel;
    QList<QFileInfo> filesList;
    QString destinationDirectory;
    RawDataParser rawDataParser;
public:
    explicit ConvertFileThread(RawDataParser& rawDataParser, QObject *parent = nullptr);
    void run();
    void addFilesToConvert(QList<QFileInfo> filesList);
    void setDestinationDirectory(QString destination);
signals:
    void actualProgress(int percentageProgress);
    void actualFileConverting(QString name);
public slots:
    void cancel();
};

#endif // THREAD_H
