/*!
 * \file preferences_dialog.cpp
 * \brief Implementation of the preferences dialog of the gui.
 *
 * \author Álvaro Cebrián Juan, 2018. acebrianjuan(at)gmail.com
 *
 * -----------------------------------------------------------------------
 *
 * Copyright (C) 2010-2019  (see AUTHORS file for a list of contributors)
 *
 * GNSS-SDR is a software defined Global Navigation
 *      Satellite Systems receiver
 *
 * This file is part of GNSS-SDR.
 *
 * GNSS-SDR is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GNSS-SDR is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNSS-SDR. If not, see <https://www.gnu.org/licenses/>.
 *
 * -----------------------------------------------------------------------
 */


#include "preferences_dialog.h"
#include "ui_preferences_dialog.h"
#include <QDebug>
#include <QSettings>

PreferencesDialog::PreferencesDialog(QWidget *parent) : QDialog(parent),
                                                        ui(new Ui::PreferencesDialog)
{
    ui->setupUi(this);

    QSettings settings;
    settings.beginGroup("Preferences_Dialog");
    ui->buffer_size_spinBox->setValue(settings.value("buffer_size", 1000).toInt());
    ui->port_gnss_synchro_spinBox->setValue(settings.value("port_gnss_synchro", 1111).toInt());
    ui->port_monitor_pvt_spinBox->setValue(settings.value("port_monitor_pvt", 1112).toInt());
    settings.endGroup();

    connect(this, &PreferencesDialog::accepted, this, &PreferencesDialog::onAccept);
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

void PreferencesDialog::onAccept()
{
    QSettings settings;
    settings.beginGroup("Preferences_Dialog");
    settings.setValue("buffer_size", ui->buffer_size_spinBox->value());
    settings.setValue("port_gnss_synchro", ui->port_gnss_synchro_spinBox->value());
    settings.setValue("port_monitor_pvt", ui->port_monitor_pvt_spinBox->value());
    settings.endGroup();

    qDebug() << "Preferences Saved";
}
