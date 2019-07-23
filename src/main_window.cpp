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
    updateTimer.setInterval(500);
    updateTimer.setSingleShot(true);
    connect(&updateTimer, &QTimer::timeout, [this] { model->update(); });

    // connect(&updateTimer, &QTimer::timeout, this, &Main_Window::map_contents);

    ui->setupUi(this);

    // Monitor_Pvt_Wrapper.
    m_monitor_pvt_wrapper = new Monitor_Pvt_Wrapper();

    // Map: QQuickWidget.
    map_dock = new QDockWidget("Map", this);
    map_widget = new QQuickWidget(this);
    map_widget->rootContext()->setContextProperty("m_monitor_pvt_wrapper",
        m_monitor_pvt_wrapper);
    map_widget->setSource(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    map_widget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    map_dock->setWidget(map_widget);
    addDockWidget(Qt::TopDockWidgetArea, map_dock);

    // QMenuBar.
    ui->actionQuit->setIcon(QIcon::fromTheme("application-exit"));
    ui->actionQuit->setShortcuts(QKeySequence::Quit);

    ui->actionPreferences->setIcon(QIcon::fromTheme("preferences-desktop"));
    ui->actionPreferences->setShortcuts(QKeySequence::Preferences);

    connect(ui->actionQuit, &QAction::triggered, qApp, &QApplication::quit);
    connect(ui->actionPreferences, &QAction::triggered, this,
        &Main_Window::show_preferences);

    // QToolbar.
    start = ui->mainToolBar->addAction("Start");
    stop = ui->mainToolBar->addAction("Stop");
    clear = ui->mainToolBar->addAction("Clear");
    ui->mainToolBar->addSeparator();
    close_plots_action = ui->mainToolBar->addAction("Close Plots");
    start->setEnabled(false);
    stop->setEnabled(true);
    clear->setEnabled(false);
    connect(start, &QAction::triggered, this, &Main_Window::toggle_capture);
    connect(stop, &QAction::triggered, this, &Main_Window::toggle_capture);
    connect(clear, &QAction::triggered, this, &Main_Window::clear_entries);
    connect(close_plots_action, &QAction::triggered, this,
        &Main_Window::close_plots);

    // Model.
    model = new Channel_Table_Model();

    // QTableView.
    // Tie the model to the view.
    ui->tableView->setModel(model);
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
    socket_gnss_synchro = new QUdpSocket(this);
    socket_monitor_pvt = new QUdpSocket(this);

    // Connect Signals & Slots.
    connect(socket_gnss_synchro, &QUdpSocket::readyRead, this,
        &Main_Window::receive_gnss_synchro);
    connect(socket_monitor_pvt, &QUdpSocket::readyRead, this,
        &Main_Window::receive_monitor_pvt);
    connect(qApp, &QApplication::aboutToQuit, this, &Main_Window::quit);
    connect(ui->tableView, &QTableView::clicked, this, &Main_Window::expand_plot);

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
    if (start->isEnabled())
        {
            start->setEnabled(false);
            stop->setEnabled(true);
        }
    else
        {
            start->setEnabled(true);
            stop->setEnabled(false);
        }
}

void Main_Window::receive_gnss_synchro()
{
    bool newData = false;
    while (socket_gnss_synchro->hasPendingDatagrams())
        {
            newData = true;
            QNetworkDatagram datagram = socket_gnss_synchro->receiveDatagram();
            stocks = read_gnss_synchro(datagram.data().data(), datagram.data().size());

            if (stop->isEnabled())
                {
                    model->populate_channels(&stocks);
                    clear->setEnabled(true);
                }
        }
    if (newData && !updateTimer.isActive())
        {
            updateTimer.start();
        }
}

void Main_Window::receive_monitor_pvt()
{
    while (socket_monitor_pvt->hasPendingDatagrams())
        {
            QNetworkDatagram datagram = socket_monitor_pvt->receiveDatagram();
            m_monitor_pvt =
                read_monitor_pvt(datagram.data().data(), datagram.data().size());

            if (stop->isEnabled())
                {
                    m_monitor_pvt_wrapper->add_monitor_pvt(m_monitor_pvt);
                    // clear->setEnabled(true);
                }
        }
}

void Main_Window::clear_entries()
{
    model->clear_channels();
    model->update();
    clear->setEnabled(false);
}

void Main_Window::quit() { save_settings(); }

gnss_sdr::Observables Main_Window::read_gnss_synchro(char buff[], int bytes)
{
    std::string data(buff, bytes);
    stocks.ParseFromString(data);
    try
        {
            std::string data(buff, bytes);
            stocks.ParseFromString(data);
        }
    catch (std::exception &e)
        {
            qDebug() << e.what();
        }

    return stocks;
}

gnss_sdr::MonitorPvt Main_Window::read_monitor_pvt(char buff[], int bytes)
{
    try
        {
            std::string data(buff, bytes);
            m_monitor_pvt.ParseFromString(data);
        }
    catch (std::exception &e)
        {
            qDebug() << e.what();
        }

    return m_monitor_pvt;
}

void Main_Window::save_settings()
{
    settings.beginGroup("Main_Window");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
    settings.endGroup();

    settings.beginGroup("tableView");
    settings.beginWriteArray("column");
    for (int i = 0; i < model->get_columns(); i++)
        {
            settings.setArrayIndex(i);
            settings.setValue("width", ui->tableView->columnWidth(i));
        }
    settings.endArray();
    settings.endGroup();

    qDebug() << "Settings Saved";
}

void Main_Window::load_settings()
{
    settings.beginGroup("Main_Window");
    move(settings.value("pos", QPoint(0, 0)).toPoint());
    resize(settings.value("size", QSize(1400, 600)).toSize());
    settings.endGroup();

    settings.beginGroup("tableView");
    settings.beginReadArray("column");
    for (int i = 0; i < model->get_columns(); i++)
        {
            settings.setArrayIndex(i);
            ui->tableView->setColumnWidth(i, settings.value("width", 100).toInt());
        }
    settings.endArray();
    settings.endGroup();

    set_port();

    qDebug() << "Settings Loaded";
}

void Main_Window::show_preferences()
{
    Preferences_Dialog *preferences = new Preferences_Dialog(this);
    connect(preferences, &Preferences_Dialog::accepted, model,
        &Channel_Table_Model::set_buffer_size);
    connect(preferences, &Preferences_Dialog::accepted, this,
        &Main_Window::set_port);
    preferences->exec();
}

void Main_Window::set_port()
{
    QSettings settings;
    settings.beginGroup("Preferences_Dialog");
    port_gnss_synchro = settings.value("port_gnss_synchro", DEFAULT_PORT).toInt();
    port_monitor_pvt = settings.value("port_monitor_pvt", DEFAULT_PORT).toInt();
    settings.endGroup();

    socket_gnss_synchro->disconnectFromHost();
    socket_gnss_synchro->bind(QHostAddress::Any, port_gnss_synchro);
    socket_monitor_pvt->bind(QHostAddress::Any, port_monitor_pvt);
}

void Main_Window::expand_plot(const QModelIndex &index)
{
    qDebug() << index;

    int channel_id = model->get_channel_id(index.row());

    QChartView *chartView = nullptr;

    if (index.column() == 5)  // Constellation
        {
            if (plots_constellation.find(index.row()) == plots_constellation.end())
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
                        [this, index]() { plots_constellation.erase(index.row()); });

                    // Update chart on timer timeout.
                    connect(&updateTimer, &QTimer::timeout, chart, [chart, series, index]() {
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

                    plots_constellation[index.row()] = chartView;
                }
            else
                {
                    chartView = plots_constellation.at(index.row());
                }
        }
    else if (index.column() == 6)  // CN0
        {
            if (plots_cn0.find(index.row()) == plots_cn0.end())
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
                        [this, index]() { plots_cn0.erase(index.row()); });

                    // Update chart on timer timeout.
                    connect(&updateTimer, &QTimer::timeout, chart, [chart, series, index]() {
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

                    plots_cn0[index.row()] = chartView;
                }
            else
                {
                    chartView = plots_cn0.at(index.row());
                }
        }
    else if (index.column() == 7)  // Doppler
        {
            if (plots_doppler.find(index.row()) == plots_doppler.end())
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
                        [this, index]() { plots_doppler.erase(index.row()); });

                    // Update chart on timer timeout.
                    connect(&updateTimer, &QTimer::timeout, chart, [chart, series, index]() {
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

                    plots_doppler[index.row()] = chartView;
                }
            else
                {
                    chartView = plots_doppler.at(index.row());
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
    for (auto const &ch : plots_constellation)
        {
            auto const &chartView = ch.second;
            chartView->close();
        }

    for (auto const &ch : plots_cn0)
        {
            auto const &chartView = ch.second;
            chartView->close();
        }

    for (auto const &ch : plots_doppler)
        {
            auto const &chartView = ch.second;
            chartView->close();
        }
}

void Main_Window::delete_plots()
{
    for (auto const &ch : plots_constellation)
        {
            auto const &chartView = ch.second;
            chartView->deleteLater();
        }
    plots_constellation.clear();

    for (auto const &ch : plots_cn0)
        {
            auto const &chartView = ch.second;
            chartView->deleteLater();
        }
    plots_cn0.clear();

    for (auto const &ch : plots_doppler)
        {
            auto const &chartView = ch.second;
            chartView->deleteLater();
        }
    plots_doppler.clear();
}

/*
void Main_Window::map_contents()
{
    qDebug() << "plots_constellation: " << plots_constellation.size();
    qDebug() << "plots_cn0: " << plots_cn0.size();
    qDebug() << "plots_doppler: " << plots_doppler.size();
}
*/
