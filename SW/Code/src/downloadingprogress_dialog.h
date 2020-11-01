#ifndef FILESDOWNLOAD_DIALOG_H
#define FILESDOWNLOAD_DIALOG_H

#include "convertfile_thread.h"
#include "QStringIntMap.h"

#include <QDialog>
#include <QListWidgetItem>
#include <QMap>
#include <QString>

namespace Ui {
class FilesDownloadDialog;
}
/*
class WarningWidgetItem : public QListWidgetItem {
    QStringIntMap warngs;
public:
    WarningWidgetItem(const QString &text, QListWidget *parent = nullptr, int type = Type) : QListWidgetItem(text, parent, type){}
    virtual void setData(int role, const QVariant &value) override {
        if (role == Qt::UserRole){
            warngs = value.value<QStringIntMap>();
        } else {
            QListWidgetItem::setData(role, value);
        }
    }

    virtual QVariant data(int role) const override {
        if (role == Qt::UserRole){
            QVariant v;
            v.setValue<QStringIntMap>(warngs);
            return v;
        } else {
            return QListWidgetItem::data(role);
        }
    }

    void setWarnings(QStringIntMap _warngs){
        warngs = _warngs;
    }
    QStringIntMap warnings(){
        return warngs;
    }
    virtual ~WarningWidgetItem(){
    }
};*/

class DownloadInProgressDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DownloadInProgressDialog(QWidget *parent = nullptr);
    ~DownloadInProgressDialog() override;
private:
    Ui::FilesDownloadDialog *ui;
public slots:
    void updateProgressBar(int);
    void addFileToList(QString sourceFileName, QString destinationFileName);
    void errorInLastFile(QString reasonDescription);
    void warningsInLastFile(QStringIntMap warnings);
    void downloadingComplete();
    void convertingThreadStarted();
    void cleanUp();
private slots:
    void on_filesListWidget_itemClicked(QListWidgetItem *item);
};

#endif // FILESDOWNLOAD_DIALOG_H


