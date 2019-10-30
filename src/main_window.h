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

#include "altitude_widget.h"
#include "channel_table_model.h"
#include "dop_widget.h"
#include "gnss_synchro.pb.h"
#include "monitor_pvt.pb.h"
#include "monitor_pvt_wrapper.h"
#include "telecommand_widget.h"
#include <QAbstractTableModel>
#include <QChart>
#include <QChartView>
#include <QMainWindow>
#include <QQuickWidget>
#include <QSettings>
#include <QTimer>
#include <QXYSeries>
#include <QtNetwork/QUdpSocket>

namespace Ui
{
class MainWindow;
}

namespace QtCharts
{
class QChart;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    gnss_sdr::Observables readGnssSynchro(char buff[], int bytes);
    gnss_sdr::MonitorPvt readMonitorPvt(char buff[], int bytes);
    void saveSettings();
    void loadSettings();

public slots:
    void toggleCapture();
    void receiveGnssSynchro();
    void receiveMonitorPvt();
    void clearEntries();
    void quit();
    void showPreferences();
    void setPort();
    void expandPlot(const QModelIndex &index);
    void closePlots();
    void deletePlots();
    void about();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    void updateChart(QtCharts::QChart *chart, QtCharts::QXYSeries *series, const QModelIndex &index);

    Ui::MainWindow *ui;

    QDockWidget *m_mapDockWidget;
    QDockWidget *m_telecommandDockWidget;
    QDockWidget *m_altitudeDockWidget;
    QDockWidget *m_DOPDockWidget;

    QQuickWidget *m_mapWidget;
    TelecommandWidget *m_telecommandWidget;
    AltitudeWidget *m_altitudeWidget;
    DOPWidget *m_DOPWidget;

    ChannelTableModel *m_model;
    QUdpSocket *m_socketGnssSynchro;
    QUdpSocket *m_socketMonitorPvt;
    gnss_sdr::Observables m_stocks;
    MonitorPvtWrapper *m_monitorPvtWrapper;
    gnss_sdr::MonitorPvt m_monitorPvt;
    std::vector<int> m_channels;
    quint16 m_portGnssSynchro;
    quint16 m_portMonitorPvt;
    QSettings m_settings;
    QTimer m_updateTimer;

    QAction *m_start;
    QAction *m_stop;
    QAction *m_clear;
    QAction *m_closePlotsAction;

    int m_bufferSize;

    std::map<int, QtCharts::QChartView *> m_plotsConstellation;
    std::map<int, QtCharts::QChartView *> m_plotsCn0;
    std::map<int, QtCharts::QChartView *> m_plotsDoppler;
};

#endif  // GNSS_SDR_MONITOR_MAIN_WINDOW_H_
