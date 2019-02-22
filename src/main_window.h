/*!
 * \file main_window.h
 * \brief Interface of the main window of the gui.
 *
 * \author Álvaro Cebrián Juan, 2018. acebrianjuan(at)gmail.com
 *
 * -------------------------------------------------------------------------
 *
 * Copyright (C) 2010-2018  (see AUTHORS file for a list of contributors)
 *
 * GNSS-SDR is a software defined Global Navigation
 *          Satellite Systems receiver
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
 * -------------------------------------------------------------------------
 */


#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QAbstractTableModel>
#include <QtNetwork/QUdpSocket>
#include <QSettings>
#include <QQmlApplicationEngine>
#include <QQuickWidget>
#include <QQuickView>
#include <QQuickItem>
#include <QGeoCoordinate>

#include "gnss_synchro.h"
#include "channel_table_model.h"
#include "qcustomplot.h"

namespace Ui {
class Main_Window;
}

class Main_Window : public QMainWindow
{
    Q_OBJECT
    //Q_PROPERTY(QGeoCoordinate position READ position WRITE set_position NOTIFY position_changed)

public:
    explicit Main_Window(QWidget *parent = nullptr);
    ~Main_Window();
    std::vector<Gnss_Synchro> read_gnss_synchro(char buff[], int bytes);
    void save_settings();
    void load_settings();

private:
    Ui::Main_Window *ui;
    QDockWidget *map_dock;
    QQuickWidget *map_widget;
    QQuickItem *item;
    Channel_Table_Model *model;
    QUdpSocket *socket_gnss_synchro;
    QUdpSocket *socket_monitor_pvt;
    std::vector<Gnss_Synchro> stocks;
    std::vector<int> channels;
    unsigned int port;
    QSettings settings;

    QAction *start;
    QAction *stop;
    QAction *clear;

    int buffer_size;

public slots:
    void toggle_capture();
    void add_entry();
    void clear_entries();
    void quit();
    void show_preferences();
    void set_port();

};

#endif // MAIN_WINDOW_H
