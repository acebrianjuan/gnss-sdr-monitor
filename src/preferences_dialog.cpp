#include "preferences_dialog.h"
#include "ui_preferences_dialog.h"
#include <QSettings>
#include <QDebug>

Preferences_Dialog::Preferences_Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Preferences_Dialog)
{
    ui->setupUi(this);

    QSettings settings;
    settings.beginGroup("Preferences_Dialog");
    ui->buffer_size_spinBox->setValue(settings.value("buffer_size", 1000).toInt());
    ui->port_spinBox->setValue(settings.value("port", 1337).toInt());
    settings.endGroup();

    connect(this, &Preferences_Dialog::accepted, this, &Preferences_Dialog::on_accept);
}

Preferences_Dialog::~Preferences_Dialog()
{
    delete ui;
}

void Preferences_Dialog::on_accept()
{
    QSettings settings;
    settings.beginGroup("Preferences_Dialog");
    settings.setValue("buffer_size", ui->buffer_size_spinBox->value());
    settings.setValue("port", ui->port_spinBox->value());
    settings.endGroup();

    qDebug() << "Preferences Saved";
    //this->accept();
}
