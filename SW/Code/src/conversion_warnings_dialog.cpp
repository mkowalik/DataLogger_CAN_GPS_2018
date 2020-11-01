#include "conversion_warnings_dialog.h"
#include "ui_conversion_warnings_dialog.h"

#include <QString>
#include <QMenu>
#include <QMessageBox>
#include <QClipboard>

ConversionWarningsDialog::ConversionWarningsDialog(QStringIntMap warnings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::conversion_warnings_dialog)
{
    ui->setupUi(this);
    foreach(QString warn, warnings.keys()){
        QListWidgetItem* pItem = new QListWidgetItem(
                    QString("\"") +
                    warn +
                    QString("\" occured ") +
                    QString::number(warnings[warn]) +
                    QString(" times"));
        pItem->setForeground(QColor(255,165,0));
        ui->warningsListWidget->addItem(pItem);
    }
}

ConversionWarningsDialog::~ConversionWarningsDialog()
{
    delete ui;
}

void ConversionWarningsDialog::on_warningsListWidget_customContextMenuRequested(const QPoint &pos)
{
    try {
        QMenu menu(this); // add menu items
        menu.addAction(ui->actionCopy);

        QListWidgetItem *pClickedItem = ui->warningsListWidget->itemAt(pos);

        if (pClickedItem == nullptr){
            return;
        }

        QClipboard* pClipboard = QApplication::clipboard();
        pClipboard->setText(pClickedItem->text());

    } catch (const std::logic_error& e){
        QMessageBox::warning(this, "Error", e.what());
    } catch (const std::exception& e){
        QMessageBox::warning(this, "Error", QString("Unkonwn error occured: ") + QString(e.what()));
    }

}
