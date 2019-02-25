/*!
 * \file main_window.cpp
 * \brief Implementation of the main window of the gui.
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


#include "main_window.h"
#include "ui_main_window.h"
#include "constellation_delegate.h"
#include "cn0_delegate.h"
#include "doppler_delegate.h"
#include "led_delegate.h"
#include "preferences_dialog.h"

#include <QtNetwork/QHostAddress>
#include <QtNetwork/QNetworkDatagram>

#include <QGeoLocation>
#include <QGeoCoordinate>

#include <QStringList>
#include <QDebug>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <sstream>
#include <iostream>

#define DEFAULT_PORT 1337

Main_Window::Main_Window(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Main_Window)
{
    ui->setupUi(this);


    // Map: QQuickWidget.
    map_dock = new QDockWidget("Map", this);
    map_widget = new QQuickWidget(this);
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
    connect(ui->actionPreferences, &QAction::triggered, this, &Main_Window::show_preferences);


    // QToolbar.
    start = ui->mainToolBar->addAction("Start");
    stop = ui->mainToolBar->addAction("Stop");
    clear = ui->mainToolBar->addAction("Clear");
    ui->mainToolBar->addSeparator();
    start->setEnabled(false);
    stop->setEnabled(true);
    clear->setEnabled(false);
    connect(start, &QAction::triggered, this, &Main_Window::toggle_capture);
    connect(stop, &QAction::triggered, this, &Main_Window::toggle_capture);
    connect(clear, &QAction::triggered, this, &Main_Window::clear_entries);


    // Model.
    model = new Channel_Table_Model();


    // QTableView.
    // Tie the model to the view.
    ui->tableView->setModel(model);
    ui->tableView->setShowGrid(false);
    ui->tableView->verticalHeader()->hide();
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setItemDelegateForColumn(4, new Constellation_Delegate());
    ui->tableView->setItemDelegateForColumn(5, new Cn0_Delegate());
    ui->tableView->setItemDelegateForColumn(6, new Doppler_Delegate());
    ui->tableView->setItemDelegateForColumn(8, new LED_Delegate());
    //ui->tableView->setAlternatingRowColors(true);
    //ui->tableView->setSelectionBehavior(QTableView::SelectRows);


    // Sockets.
    socket_gnss_synchro = new QUdpSocket(this);
    socket_monitor_pvt = new QUdpSocket(this);


    // QStautsBar
    //statusBar()->showMessage("Listening on UDP port " + QString::number(port));


    // StyleSheet.
    //setStyleSheet( styleSheet().append("QWidget {background-color: #333333; color: #EFF0F1;}"));
    //setStyleSheet( styleSheet().append("QStatusBar{border-top: 1px outset grey;}"));
    //setStyleSheet( styleSheet().append("QToolTip { color: #fff; background-color: #000; border: none; }"));


    // Connect Signals & Slots.
    connect(socket_gnss_synchro, &QUdpSocket::readyRead, this, &Main_Window::receive_gnss_synchro);
    connect(qApp, &QApplication::aboutToQuit, this, &Main_Window::quit);


    // Load settings from last session.
    load_settings();
}

Main_Window::~Main_Window()
{
    delete ui;
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
    while (socket_gnss_synchro->hasPendingDatagrams())
    {
        QNetworkDatagram datagram = socket_gnss_synchro->receiveDatagram();
        stocks = read_gnss_synchro(datagram.data().data(), datagram.data().size());

        if(stop->isEnabled())
        {
            model->populate_channels(stocks);
            clear->setEnabled(true);
        }
    }
}

void Main_Window::clear_entries()
{
    model->clear_channels();
    clear->setEnabled(false);
}

void Main_Window::quit()
{
    save_settings();
}

std::vector<Gnss_Synchro> Main_Window::read_gnss_synchro(char buff[], int bytes)
{
    try
    {
        std::string archive_data(&buff[0], bytes);
        std::istringstream archive_stream(archive_data);
        boost::archive::binary_iarchive archive(archive_stream);
        archive >> stocks;
    }
    catch (std::exception& e)
    {
        qDebug() << e.what();
    }

    return stocks;
}

void Main_Window::save_settings()
{
    settings.beginGroup("Main_Window");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
    settings.endGroup();

    settings.beginGroup("tableView");
    settings.beginWriteArray("column");
    for (int i = 0; i < model->get_columns(); i++) {
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
    for (int i = 0; i < model->get_columns(); i++) {
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
    connect(preferences, &Preferences_Dialog::accepted, model, &Channel_Table_Model::set_buffer_size);
    connect(preferences, &Preferences_Dialog::accepted, this, &Main_Window::set_port);
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
    socket_gnss_synchro->bind(QHostAddress::LocalHost, port_gnss_synchro);
    socket_monitor_pvt->bind(QHostAddress::LocalHost, port_monitor_pvt);
}
