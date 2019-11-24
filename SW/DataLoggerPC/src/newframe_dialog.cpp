#include "newframe_dialog.h"
#include "ui_newframe_dialog.h"

#include <QMessageBox>

constexpr unsigned int NEW_FRAME_ID = 0;

NewFrameDialog::NewFrameDialog(Config& config, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewFrameDialog),
    config(config),
    editingId(NEW_FRAME_ID),
    isNewFrame(true)
{
    ui->setupUi(this);
}

NewFrameDialog::NewFrameDialog(unsigned int id, QString moduleName, Config& config, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewFrameDialog),
    config(config),
    editingId(id),
    isNewFrame(false)
{
    ui->setupUi(this);
    ui->id_SpinBox->setValue(static_cast<int>(id));
    ui->moduleName_lineEdit->setText(moduleName);
}

unsigned int NewFrameDialog::getID(){
    if (ui->id_SpinBox->value() < 0){
        throw std::invalid_argument("ID must be greater or equal to 0.");
    }
    return static_cast<unsigned int>(ui->id_SpinBox->value());
}

QString NewFrameDialog::getModuleName(){
    return ui->moduleName_lineEdit->text();
}

NewFrameDialog::~NewFrameDialog()
{
    delete ui;
}

void NewFrameDialog::on_buttonBox_accepted()
{
    if (((this->isNewFrame == false) && (editingId == this->getID())) ||
            (!config.hasFrameWithId(this->getID())) ){
        accept();
    } else {
        QMessageBox::warning(this, "Wrong ID Error", "Frame with given ID Exists in configuration. Choose other ID.");
    }
}
