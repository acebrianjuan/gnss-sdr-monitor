/*!
 * \file main_window.h
 * \brief Interface of the main window of the gui.
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


#ifndef GNSS_SDR_MONITOR_MAIN_WINDOW_H_
#define GNSS_SDR_MONITOR_MAIN_WINDOW_H_

#include "channel_table_model.h"
#include "gnss_synchro.pb.h"
#include "monitor_pvt.pb.h"
#include "monitor_pvt_wrapper.h"
#include <QAbstractTableModel>
#include <QChartView>
#include <QMainWindow>
#include <QQuickWidget>
#include <QSettings>
#include <QTimer>
#include <QtNetwork/QUdpSocket>

namespace Ui
{
class Main_Window;
}

namespace QtCharts
{
class QChart;
}

class Main_Window : public QMainWindow
{
    Q_OBJECT

public:
    explicit Main_Window(QWidget *parent = nullptr);
    ~Main_Window();

    gnss_sdr::Observables read_gnss_synchro(char buff[], int bytes);
    gnss_sdr::MonitorPvt read_monitor_pvt(char buff[], int bytes);
    void save_settings();
    void load_settings();

private:
    Ui::Main_Window *ui;
    QDockWidget *map_dock;
    QQuickWidget *map_widget;
    Channel_Table_Model *model;
    QUdpSocket *socket_gnss_synchro;
    QUdpSocket *socket_monitor_pvt;
    gnss_sdr::Observables stocks;
    Monitor_Pvt_Wrapper *m_monitor_pvt_wrapper;
    gnss_sdr::MonitorPvt m_monitor_pvt;
    std::vector<int> channels;
    quint16 port_gnss_synchro;
    quint16 port_monitor_pvt;
    QSettings settings;
    QTimer updateTimer;

    QAction *start;
    QAction *stop;
    QAction *clear;
    QAction *close_plots_action;

    int buffer_size;

    std::map<int, QtCharts::QChartView *> plots_constellation;
    std::map<int, QtCharts::QChartView *> plots_cn0;
    std::map<int, QtCharts::QChartView *> plots_doppler;

public slots:
    void toggle_capture();
    void receive_gnss_synchro();
    void receive_monitor_pvt();
    void clear_entries();
    void quit();
    void show_preferences();
    void set_port();
    void expand_plot(const QModelIndex &index);
    void close_plots();
    void delete_plots();
    //void map_contents();

protected:
    void closeEvent(QCloseEvent *event) override;
};

#endif  // GNSS_SDR_MONITOR_MAIN_WINDOW_H_
