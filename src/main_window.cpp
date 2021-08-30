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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    // Use a timer to delay updating the model to a fixed amount of times per
    // second.
    m_updateTimer.setInterval(500);
    m_updateTimer.setSingleShot(true);
    connect(&m_updateTimer, &QTimer::timeout, [this] { m_model->update(); });

    ui->setupUi(this);

    // Monitor_Pvt_Wrapper.
    m_monitorPvtWrapper = new MonitorPvtWrapper();

    // Telecommand widget.
    m_telecommandDockWidget = new QDockWidget("Telecommand", this);
    m_telecommandWidget = new TelecommandWidget(m_telecommandDockWidget);
    m_telecommandDockWidget->setWidget(m_telecommandWidget);
    addDockWidget(Qt::TopDockWidgetArea, m_telecommandDockWidget);
    connect(m_telecommandWidget, &TelecommandWidget::resetClicked, this, &MainWindow::clearEntries);

    // Map widget.
    m_mapDockWidget = new QDockWidget("Map", this);
    m_mapWidget = new QQuickWidget(this);
    m_mapWidget->rootContext()->setContextProperty("m_monitor_pvt_wrapper", m_monitorPvtWrapper);
    m_mapWidget->setSource(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    m_mapWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    m_mapDockWidget->setWidget(m_mapWidget);
    addDockWidget(Qt::TopDockWidgetArea, m_mapDockWidget);

    // Altitude widget.
    m_altitudeDockWidget = new QDockWidget("Altitude", this);
    m_altitudeWidget = new AltitudeWidget(m_altitudeDockWidget);
    m_altitudeDockWidget->setWidget(m_altitudeWidget);
    addDockWidget(Qt::TopDockWidgetArea, m_altitudeDockWidget);
    connect(m_monitorPvtWrapper, &MonitorPvtWrapper::altitudeChanged, m_altitudeWidget, &AltitudeWidget::addData);
    connect(&m_updateTimer, &QTimer::timeout, m_altitudeWidget, &AltitudeWidget::redraw);

    // Dilution of precision widget.
    m_DOPDockWidget = new QDockWidget("DOP", this);
    m_DOPWidget = new DOPWidget(m_DOPDockWidget);
    m_DOPDockWidget->setWidget(m_DOPWidget);
    addDockWidget(Qt::TopDockWidgetArea, m_DOPDockWidget);
    connect(m_monitorPvtWrapper, &MonitorPvtWrapper::dopChanged, m_DOPWidget, &DOPWidget::addData);
    connect(&m_updateTimer, &QTimer::timeout, m_DOPWidget, &DOPWidget::redraw);

    // QMenuBar.
    ui->actionQuit->setIcon(QIcon::fromTheme("application-exit"));
    ui->actionQuit->setShortcuts(QKeySequence::Quit);

    ui->actionPreferences->setIcon(QIcon::fromTheme("preferences-desktop"));
    ui->actionPreferences->setShortcuts(QKeySequence::Preferences);

    connect(ui->actionQuit, &QAction::triggered, qApp, &QApplication::quit);
    connect(ui->actionPreferences, &QAction::triggered, this, &MainWindow::showPreferences);

    // QToolbar.
    m_start = ui->mainToolBar->addAction("Start");
    m_stop = ui->mainToolBar->addAction("Stop");
    m_clear = ui->mainToolBar->addAction("Clear");
    ui->mainToolBar->addSeparator();
    m_closePlotsAction = ui->mainToolBar->addAction("Close Plots");
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(m_telecommandDockWidget->toggleViewAction());
    ui->mainToolBar->addAction(m_mapDockWidget->toggleViewAction());
    ui->mainToolBar->addAction(m_altitudeDockWidget->toggleViewAction());
    ui->mainToolBar->addAction(m_DOPDockWidget->toggleViewAction());
    m_start->setEnabled(false);
    m_stop->setEnabled(true);
    m_clear->setEnabled(false);
    connect(m_start, &QAction::triggered, this, &MainWindow::toggleCapture);
    connect(m_stop, &QAction::triggered, this, &MainWindow::toggleCapture);
    connect(m_clear, &QAction::triggered, this, &MainWindow::clearEntries);
    connect(m_closePlotsAction, &QAction::triggered, this, &MainWindow::closePlots);

    // Model.
    m_model = new ChannelTableModel();

    // QTableView.
    // Tie the model to the view.
    ui->tableView->setModel(m_model);
    ui->tableView->setShowGrid(false);
    ui->tableView->verticalHeader()->hide();
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setItemDelegateForColumn(5, new ConstellationDelegate());
    ui->tableView->setItemDelegateForColumn(6, new Cn0Delegate());
    ui->tableView->setItemDelegateForColumn(7, new DopplerDelegate());
    ui->tableView->setItemDelegateForColumn(9, new LedDelegate());
    // ui->tableView->setAlternatingRowColors(true);
    // ui->tableView->setSelectionBehavior(QTableView::SelectRows);

    // Sockets.
    m_socketGnssSynchro = new QUdpSocket(this);
    m_socketMonitorPvt = new QUdpSocket(this);

    // Connect Signals & Slots.
    connect(m_socketGnssSynchro, &QUdpSocket::readyRead, this, &MainWindow::receiveGnssSynchro);
    connect(m_socketMonitorPvt, &QUdpSocket::readyRead, this, &MainWindow::receiveMonitorPvt);
    connect(qApp, &QApplication::aboutToQuit, this, &MainWindow::quit);
    connect(ui->tableView, &QTableView::clicked, this, &MainWindow::expandPlot);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::about);
    connect(ui->actionAboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);

    // Load settings from last session.
    loadSettings();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::closeEvent(QCloseEvent *event)
{
    deletePlots();

    QMainWindow::closeEvent(event);
}

void MainWindow::updateChart(QtCharts::QChart *chart, QtCharts::QXYSeries *series, const QModelIndex &index)
{
    QPointF p;
    QVector<QPointF> points;

    double min_x = std::numeric_limits<double>::max();
    double max_x = -std::numeric_limits<double>::max();

    double min_y = std::numeric_limits<double>::max();
    double max_y = -std::numeric_limits<double>::max();

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

    chart->axes(Qt::Horizontal).back()->setRange(min_x, max_x);
    chart->axes(Qt::Vertical).back()->setRange(min_y, max_y);
}

void MainWindow::toggleCapture()
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

void MainWindow::receiveGnssSynchro()
{
    bool newData = false;
    while (m_socketGnssSynchro->hasPendingDatagrams())
    {
        newData = true;
        QNetworkDatagram datagram = m_socketGnssSynchro->receiveDatagram();
        m_stocks = readGnssSynchro(datagram.data().data(), datagram.data().size());

        if (m_stop->isEnabled())
        {
            m_model->populateChannels(&m_stocks);
            m_clear->setEnabled(true);
        }
    }
    if (newData && !m_updateTimer.isActive())
    {
        m_updateTimer.start();
    }
}

void MainWindow::receiveMonitorPvt()
{
    while (m_socketMonitorPvt->hasPendingDatagrams())
    {
        QNetworkDatagram datagram = m_socketMonitorPvt->receiveDatagram();
        m_monitorPvt =
            readMonitorPvt(datagram.data().data(), datagram.data().size());

        if (m_stop->isEnabled())
        {
            m_monitorPvtWrapper->addMonitorPvt(m_monitorPvt);
            // clear->setEnabled(true);
        }
    }
}

void MainWindow::clearEntries()
{
    m_model->clearChannels();
    m_model->update();

    m_altitudeWidget->clear();
    m_DOPWidget->clear();

    m_clear->setEnabled(false);
}

void MainWindow::quit() { saveSettings(); }

gnss_sdr::Observables MainWindow::readGnssSynchro(char buff[], int bytes)
{
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

gnss_sdr::MonitorPvt MainWindow::readMonitorPvt(char buff[], int bytes)
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

void MainWindow::saveSettings()
{
    m_settings.beginGroup("Main_Window");
    m_settings.setValue("pos", pos());
    m_settings.setValue("size", size());
    m_settings.endGroup();

    m_settings.beginGroup("tableView");
    m_settings.beginWriteArray("column");
    for (int i = 0; i < m_model->getColumns(); i++)
    {
        m_settings.setArrayIndex(i);
        m_settings.setValue("width", ui->tableView->columnWidth(i));
    }
    m_settings.endArray();
    m_settings.endGroup();

    qDebug() << "Settings Saved";
}

void MainWindow::loadSettings()
{
    m_settings.beginGroup("Main_Window");
    move(m_settings.value("pos", QPoint(0, 0)).toPoint());
    resize(m_settings.value("size", QSize(1400, 600)).toSize());
    m_settings.endGroup();

    m_settings.beginGroup("tableView");
    m_settings.beginReadArray("column");
    for (int i = 0; i < m_model->getColumns(); i++)
    {
        m_settings.setArrayIndex(i);
        ui->tableView->setColumnWidth(i, m_settings.value("width", 100).toInt());
    }
    m_settings.endArray();
    m_settings.endGroup();

    setPort();

    qDebug() << "Settings Loaded";
}

void MainWindow::showPreferences()
{
    PreferencesDialog *preferences = new PreferencesDialog(this);
    connect(preferences, &PreferencesDialog::accepted, m_model,
        &ChannelTableModel::setBufferSize);
    connect(preferences, &PreferencesDialog::accepted, this,
        &MainWindow::setPort);
    preferences->exec();
}

void MainWindow::setPort()
{
    QSettings settings;
    settings.beginGroup("Preferences_Dialog");
    m_portGnssSynchro = settings.value("port_gnss_synchro", 1111).toInt();
    m_portMonitorPvt = settings.value("port_monitor_pvt", 1112).toInt();
    settings.endGroup();

    m_socketGnssSynchro->disconnectFromHost();
    m_socketGnssSynchro->bind(QHostAddress::Any, m_portGnssSynchro);
    m_socketMonitorPvt->bind(QHostAddress::Any, m_portMonitorPvt);
}

void MainWindow::expandPlot(const QModelIndex &index)
{
    qDebug() << index;

    int channel_id = m_model->getChannelId(index.row());

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
            chart->axes(Qt::Horizontal).back()->setTitleText("I prompt");
            chart->axes(Qt::Vertical).back()->setTitleText("Q prompt");
            chart->layout()->setContentsMargins(0, 0, 0, 0);
            chart->setContentsMargins(-18, -18, -14, -16);

            chartView = new QChartView(chart);
            chartView->setRenderHint(QPainter::Antialiasing);
            chartView->setContentsMargins(0, 0, 0, 0);

            // Draw chart now.
            updateChart(chart, series, index);

            // Delete the chartView object when MainWindow is closed.
            connect(this, &QMainWindow::destroyed, chartView, &QObject::deleteLater);

            // Remove element from map when chartView widget is closed.
            connect(chartView, &QObject::destroyed,
                [this, index]() { m_plotsConstellation.erase(index.row()); });

            // Update chart on timer timeout.
            connect(&m_updateTimer, &QTimer::timeout, chart, [this, chart, series, index]() {
                updateChart(chart, series, index);
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
            chart->axes(Qt::Horizontal).back()->setTitleText("TOW [s]");
            chart->axes(Qt::Vertical).back()->setTitleText("C/N0 [db-Hz]");
            chart->layout()->setContentsMargins(0, 0, 0, 0);
            chart->setContentsMargins(-18, -18, -14, -16);

            chartView = new QChartView(chart);
            chartView->setRenderHint(QPainter::Antialiasing);
            chartView->setContentsMargins(0, 0, 0, 0);

            // Draw chart now.
            updateChart(chart, series, index);

            // Delete the chartView object when MainWindow is closed.
            connect(this, &QMainWindow::destroyed, chartView, &QObject::deleteLater);

            // Remove element from map when chartView widget is closed.
            connect(chartView, &QObject::destroyed,
                [this, index]() { m_plotsCn0.erase(index.row()); });

            // Update chart on timer timeout.
            connect(&m_updateTimer, &QTimer::timeout, chart, [this, chart, series, index]() {
                updateChart(chart, series, index);
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
            chart->axes(Qt::Horizontal).back()->setTitleText("TOW [s]");
            chart->axes(Qt::Vertical).back()->setTitleText("Doppler [Hz]");
            chart->layout()->setContentsMargins(0, 0, 0, 0);
            chart->setContentsMargins(-18, -18, -14, -16);

            chartView = new QChartView(chart);
            chartView->setRenderHint(QPainter::Antialiasing);
            chartView->setContentsMargins(0, 0, 0, 0);

            // Draw chart now.
            updateChart(chart, series, index);

            // Delete the chartView object when MainWindow is closed.
            connect(this, &QMainWindow::destroyed, chartView, &QObject::deleteLater);

            // Remove element from map when chartView widget is closed.
            connect(chartView, &QObject::destroyed,
                [this, index]() { m_plotsDoppler.erase(index.row()); });

            // Update chart on timer timeout.
            connect(&m_updateTimer, &QTimer::timeout, chart, [this, chart, series, index]() {
                updateChart(chart, series, index);
            });

            m_plotsDoppler[index.row()] = chartView;
        }
        else
        {
            chartView = m_plotsDoppler.at(index.row());
        }
    }

    if (!chartView)  // Equivalent to: if (chartView == nullptr)
    {
        return;
    }

    chartView->resize(400, 180);
    chartView->show();
}

void MainWindow::closePlots()
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

void MainWindow::deletePlots()
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

void MainWindow::about()
{
    const QString text =
        "<h3>gnss-sdr-monitor</h3>"
        "A graphical user interface to monitor the GNSS-SDR status in real time."
        "<p>Written by Álvaro Cebrián Juan and licensed under GNU GPLv3 license.</p>"
        "<p>Report bugs and suggestions to acebrianjuan@gmail.com</p>";

    QMessageBox::about(this, "About gnss-sdr-monitor", text);
}
