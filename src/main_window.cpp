/*!
 * \file main_window.cpp
 * \brief Implementation of the main window of the gui.
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


#include "main_window.h"
#include "cn0_delegate.h"
#include "constellation_delegate.h"
#include "doppler_delegate.h"
#include "led_delegate.h"
#include "preferences_dialog.h"
#include "ui_main_window.h"
#include <QDebug>
#include <QQmlContext>
#include <QtCharts>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QNetworkDatagram>
#include <iostream>
#include <sstream>

#define DEFAULT_PORT 1337

Main_Window::Main_Window(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::Main_Window)
{
    // Use a timer to delay updating the model to a fixed amount of times per
    // second
    m_updateTimer.setInterval(500);
    m_updateTimer.setSingleShot(true);
    connect(&m_updateTimer, &QTimer::timeout, [this] { m_model->update(); });

    ui->setupUi(this);

    // Monitor_Pvt_Wrapper.
    m_monitorPvtWrapper = new Monitor_Pvt_Wrapper();

    // Map: QQuickWidget.
    m_mapDock = new QDockWidget("Map", this);
    m_mapWidget = new QQuickWidget(this);
    m_mapWidget->rootContext()->setContextProperty("m_monitor_pvt_wrapper",
                                                  m_monitorPvtWrapper);
    m_mapWidget->setSource(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    m_mapWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    m_mapDock->setWidget(m_mapWidget);
    addDockWidget(Qt::TopDockWidgetArea, m_mapDock);

    // QMenuBar.
    ui->actionQuit->setIcon(QIcon::fromTheme("application-exit"));
    ui->actionQuit->setShortcuts(QKeySequence::Quit);

    ui->actionPreferences->setIcon(QIcon::fromTheme("preferences-desktop"));
    ui->actionPreferences->setShortcuts(QKeySequence::Preferences);

    connect(ui->actionQuit, &QAction::triggered, qApp, &QApplication::quit);
    connect(ui->actionPreferences, &QAction::triggered, this,
            &Main_Window::show_preferences);

    // QToolbar.
    m_start = ui->mainToolBar->addAction("Start");
    m_stop = ui->mainToolBar->addAction("Stop");
    m_clear = ui->mainToolBar->addAction("Clear");
    ui->mainToolBar->addSeparator();
    m_closePlotsAction = ui->mainToolBar->addAction("Close Plots");
    m_start->setEnabled(false);
    m_stop->setEnabled(true);
    m_clear->setEnabled(false);
    connect(m_start, &QAction::triggered, this, &Main_Window::toggle_capture);
    connect(m_stop, &QAction::triggered, this, &Main_Window::toggle_capture);
    connect(m_clear, &QAction::triggered, this, &Main_Window::clear_entries);
    connect(m_closePlotsAction, &QAction::triggered, this,
            &Main_Window::close_plots);

    // Model.
    m_model = new Channel_Table_Model();

    // QTableView.
    // Tie the model to the view.
    ui->tableView->setModel(m_model);
    ui->tableView->setShowGrid(false);
    ui->tableView->verticalHeader()->hide();
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setItemDelegateForColumn(5, new Constellation_Delegate());
    ui->tableView->setItemDelegateForColumn(6, new Cn0_Delegate());
    ui->tableView->setItemDelegateForColumn(7, new Doppler_Delegate());
    ui->tableView->setItemDelegateForColumn(9, new LED_Delegate());
    // ui->tableView->setAlternatingRowColors(true);
    // ui->tableView->setSelectionBehavior(QTableView::SelectRows);

    // Sockets.
    m_socketGnssSynchro = new QUdpSocket(this);
    m_socketMonitorPvt = new QUdpSocket(this);

    // Connect Signals & Slots.
    connect(m_socketGnssSynchro, &QUdpSocket::readyRead, this,
            &Main_Window::receive_gnss_synchro);
    connect(m_socketMonitorPvt, &QUdpSocket::readyRead, this,
            &Main_Window::receive_monitor_pvt);
    connect(qApp, &QApplication::aboutToQuit, this, &Main_Window::quit);
    connect(ui->tableView, &QTableView::clicked, this, &Main_Window::expand_plot);
    connect(ui->actionAbout, &QAction::triggered, this, &Main_Window::about);
    connect(ui->actionAboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);

    // Load settings from last session.
    load_settings();
}

Main_Window::~Main_Window() { delete ui; }

void Main_Window::closeEvent(QCloseEvent *event)
{
    delete_plots();

    QMainWindow::closeEvent(event);
}

void Main_Window::toggle_capture()
{
    if (m_start->isEnabled())
    {
        m_start->setEnabled(false);
        m_stop->setEnabled(true);
    }
    else
    {
        m_start->setEnabled(true);
        m_stop->setEnabled(false);
    }
}

void Main_Window::receive_gnss_synchro()
{
    bool newData = false;
    while (m_socketGnssSynchro->hasPendingDatagrams())
    {
        newData = true;
        QNetworkDatagram datagram = m_socketGnssSynchro->receiveDatagram();
        m_stocks = read_gnss_synchro(datagram.data().data(), datagram.data().size());

        if (m_stop->isEnabled())
        {
            m_model->populate_channels(&m_stocks);
            m_clear->setEnabled(true);
        }
    }
    if (newData && !m_updateTimer.isActive())
    {
        m_updateTimer.start();
    }
}

void Main_Window::receive_monitor_pvt()
{
    while (m_socketMonitorPvt->hasPendingDatagrams())
    {
        QNetworkDatagram datagram = m_socketMonitorPvt->receiveDatagram();
        m_monitorPvt =
                read_monitor_pvt(datagram.data().data(), datagram.data().size());

        if (m_stop->isEnabled())
        {
            m_monitorPvtWrapper->add_monitor_pvt(m_monitorPvt);
            // clear->setEnabled(true);
        }
    }
}

void Main_Window::clear_entries()
{
    m_model->clear_channels();
    m_model->update();
    m_clear->setEnabled(false);
}

void Main_Window::quit() { save_settings(); }

gnss_sdr::Observables Main_Window::read_gnss_synchro(char buff[], int bytes)
{
    std::string data(buff, bytes);
    m_stocks.ParseFromString(data);
    try
    {
        std::string data(buff, bytes);
        m_stocks.ParseFromString(data);
    }
    catch (std::exception &e)
    {
        qDebug() << e.what();
    }

    return m_stocks;
}

gnss_sdr::MonitorPvt Main_Window::read_monitor_pvt(char buff[], int bytes)
{
    try
    {
        std::string data(buff, bytes);
        m_monitorPvt.ParseFromString(data);
    }
    catch (std::exception &e)
    {
        qDebug() << e.what();
    }

    return m_monitorPvt;
}

void Main_Window::save_settings()
{
    m_settings.beginGroup("Main_Window");
    m_settings.setValue("pos", pos());
    m_settings.setValue("size", size());
    m_settings.endGroup();

    m_settings.beginGroup("tableView");
    m_settings.beginWriteArray("column");
    for (int i = 0; i < m_model->get_columns(); i++)
    {
        m_settings.setArrayIndex(i);
        m_settings.setValue("width", ui->tableView->columnWidth(i));
    }
    m_settings.endArray();
    m_settings.endGroup();

    qDebug() << "Settings Saved";
}

void Main_Window::load_settings()
{
    m_settings.beginGroup("Main_Window");
    move(m_settings.value("pos", QPoint(0, 0)).toPoint());
    resize(m_settings.value("size", QSize(1400, 600)).toSize());
    m_settings.endGroup();

    m_settings.beginGroup("tableView");
    m_settings.beginReadArray("column");
    for (int i = 0; i < m_model->get_columns(); i++)
    {
        m_settings.setArrayIndex(i);
        ui->tableView->setColumnWidth(i, m_settings.value("width", 100).toInt());
    }
    m_settings.endArray();
    m_settings.endGroup();

    set_port();

    qDebug() << "Settings Loaded";
}

void Main_Window::show_preferences()
{
    Preferences_Dialog *preferences = new Preferences_Dialog(this);
    connect(preferences, &Preferences_Dialog::accepted, m_model,
            &Channel_Table_Model::set_buffer_size);
    connect(preferences, &Preferences_Dialog::accepted, this,
            &Main_Window::set_port);
    preferences->exec();
}

void Main_Window::set_port()
{
    QSettings settings;
    settings.beginGroup("Preferences_Dialog");
    m_portGnssSynchro = settings.value("port_gnss_synchro", DEFAULT_PORT).toInt();
    m_portMonitorPvt = settings.value("port_monitor_pvt", DEFAULT_PORT).toInt();
    settings.endGroup();

    m_socketGnssSynchro->disconnectFromHost();
    m_socketGnssSynchro->bind(QHostAddress::Any, m_portGnssSynchro);
    m_socketMonitorPvt->bind(QHostAddress::Any, m_portMonitorPvt);
}

void Main_Window::expand_plot(const QModelIndex &index)
{
    qDebug() << index;

    int channel_id = m_model->get_channel_id(index.row());

    QChartView *chartView = nullptr;

    if (index.column() == 5)  // Constellation
    {
        if (m_plotsConstellation.find(index.row()) == m_plotsConstellation.end())
        {
            QChart *chart = new QChart();  // has no parent!
            chart->setTitle("Constellation CH " + QString::number(channel_id));
            chart->legend()->hide();

            QScatterSeries *series = new QScatterSeries(chart);
            series->setMarkerSize(8);
            chart->addSeries(series);
            chart->createDefaultAxes();
            chart->axisX()->setTitleText("I prompt");
            chart->axisY()->setTitleText("Q prompt");
            chart->layout()->setContentsMargins(0, 0, 0, 0);
            chart->setContentsMargins(-18, -18, -14, -16);

            chartView = new QChartView(chart);
            chartView->setRenderHint(QPainter::Antialiasing);
            chartView->setContentsMargins(0, 0, 0, 0);

            connect(this, &QMainWindow::destroyed, chartView, &QObject::deleteLater);

            // Remove element from map when dialog is closed.
            connect(chartView, &QObject::destroyed,
                    [this, index]() { m_plotsConstellation.erase(index.row()); });

            // Update chart on timer timeout.
            connect(&m_updateTimer, &QTimer::timeout, chart, [chart, series, index]() {
                QPointF p;
                QVector<QPointF> points;

                double min_x = DBL_MAX;
                double min_y = DBL_MAX;

                double max_x = -DBL_MAX;
                double max_y = -DBL_MAX;

                QList<QVariant> var = index.data(Qt::DisplayRole).toList();
                for (int i = 0; i < var.size(); i++)
                {
                    p = var.at(i).toPointF();
                    points << p;

                    min_x = std::min(min_x, p.x());
                    min_y = std::min(min_y, p.y());

                    max_x = std::max(max_x, p.x());
                    max_y = std::max(max_y, p.y());
                }

                series->replace(points);

                chart->axisX()->setRange(min_x, max_x);
                chart->axisY()->setRange(min_y, max_y);
            });

            m_plotsConstellation[index.row()] = chartView;
        }
        else
        {
            chartView = m_plotsConstellation.at(index.row());
        }
    }
    else if (index.column() == 6)  // CN0
    {
        if (m_plotsCn0.find(index.row()) == m_plotsCn0.end())
        {
            QChart *chart = new QChart();  // has no parent!
            chart->setTitle("CN0 CH " + QString::number(channel_id));
            chart->legend()->hide();

            QLineSeries *series = new QLineSeries(chart);
            chart->addSeries(series);
            chart->createDefaultAxes();
            chart->axisX()->setTitleText("TOW [s]");
            chart->axisY()->setTitleText("C/N0 [db-Hz]");
            chart->layout()->setContentsMargins(0, 0, 0, 0);
            chart->setContentsMargins(-18, -18, -14, -16);

            chartView = new QChartView(chart);
            chartView->setRenderHint(QPainter::Antialiasing);
            chartView->setContentsMargins(0, 0, 0, 0);

            connect(this, &QMainWindow::destroyed, chartView, &QObject::deleteLater);

            // Remove element from map when dialog is closed.
            connect(chartView, &QObject::destroyed,
                    [this, index]() { m_plotsCn0.erase(index.row()); });

            // Update chart on timer timeout.
            connect(&m_updateTimer, &QTimer::timeout, chart, [chart, series, index]() {
                QPointF p;
                QVector<QPointF> points;

                double min_x = DBL_MAX;
                double min_y = DBL_MAX;

                double max_x = -DBL_MAX;
                double max_y = -DBL_MAX;

                QList<QVariant> var = index.data(Qt::DisplayRole).toList();
                for (int i = 0; i < var.size(); i++)
                {
                    p = var.at(i).toPointF();
                    points << p;

                    min_x = std::min(min_x, p.x());
                    min_y = std::min(min_y, p.y());

                    max_x = std::max(max_x, p.x());
                    max_y = std::max(max_y, p.y());
                }

                series->replace(points);

                chart->axisX()->setRange(min_x, max_x);
                chart->axisY()->setRange(min_y, max_y);
            });

            m_plotsCn0[index.row()] = chartView;
        }
        else
        {
            chartView = m_plotsCn0.at(index.row());
        }
    }
    else if (index.column() == 7)  // Doppler
    {
        if (m_plotsDoppler.find(index.row()) == m_plotsDoppler.end())
        {
            QChart *chart = new QChart();  // has no parent!
            chart->setTitle("Doppler CH " + QString::number(channel_id));
            chart->legend()->hide();

            QLineSeries *series = new QLineSeries(chart);
            chart->addSeries(series);
            chart->createDefaultAxes();
            chart->axisX()->setTitleText("TOW [s]");
            chart->axisY()->setTitleText("Doppler [Hz]");
            chart->layout()->setContentsMargins(0, 0, 0, 0);
            chart->setContentsMargins(-18, -18, -14, -16);

            chartView = new QChartView(chart);
            chartView->setRenderHint(QPainter::Antialiasing);
            chartView->setContentsMargins(0, 0, 0, 0);

            connect(this, &QMainWindow::destroyed, chartView, &QObject::deleteLater);

            // Remove element from map when dialog is closed.
            connect(chartView, &QObject::destroyed,
                    [this, index]() { m_plotsDoppler.erase(index.row()); });

            // Update chart on timer timeout.
            connect(&m_updateTimer, &QTimer::timeout, chart, [chart, series, index]() {
                QPointF p;
                QVector<QPointF> points;

                double min_x = DBL_MAX;
                double min_y = DBL_MAX;

                double max_x = -DBL_MAX;
                double max_y = -DBL_MAX;

                QList<QVariant> var = index.data(Qt::DisplayRole).toList();
                for (int i = 0; i < var.size(); i++)
                {
                    p = var.at(i).toPointF();
                    points << p;

                    min_x = std::min(min_x, p.x());
                    min_y = std::min(min_y, p.y());

                    max_x = std::max(max_x, p.x());
                    max_y = std::max(max_y, p.y());
                }

                series->replace(points);

                chart->axisX()->setRange(min_x, max_x);
                chart->axisY()->setRange(min_y, max_y);
            });

            m_plotsDoppler[index.row()] = chartView;
        }
        else
        {
            chartView = m_plotsDoppler.at(index.row());
        }
    }

    if (!chartView)  // similar: if (chartView == nullptr)
    {
        return;
    }

    chartView->resize(400, 180);
    chartView->show();
}

void Main_Window::close_plots()
{
    for (auto const &ch : m_plotsConstellation)
    {
        auto const &chartView = ch.second;
        chartView->close();
    }

    for (auto const &ch : m_plotsCn0)
    {
        auto const &chartView = ch.second;
        chartView->close();
    }

    for (auto const &ch : m_plotsDoppler)
    {
        auto const &chartView = ch.second;
        chartView->close();
    }
}

void Main_Window::delete_plots()
{
    for (auto const &ch : m_plotsConstellation)
    {
        auto const &chartView = ch.second;
        chartView->deleteLater();
    }
    m_plotsConstellation.clear();

    for (auto const &ch : m_plotsCn0)
    {
        auto const &chartView = ch.second;
        chartView->deleteLater();
    }
    m_plotsCn0.clear();

    for (auto const &ch : m_plotsDoppler)
    {
        auto const &chartView = ch.second;
        chartView->deleteLater();
    }
    m_plotsDoppler.clear();
}

void Main_Window::about()
{
    const QString text = "<h3>gnss-sdr-monitor</h3>"
                         "A graphical user interface to monitor the GNSS-SDR status in real time."
                         "<p>Written by Álvaro Cebrián Juan and licensed under GNU GPLv3 license.</p>"
                         "<p>Report bugs and suggestions to acebrianjuan@gmail.com</p>";

    QMessageBox::about(this, "About gnss-sdr-monitor", text);
}
