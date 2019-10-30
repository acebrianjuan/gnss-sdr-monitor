/*!
 * \file telecommand_widget.h
 * \brief Interface of a widget for sending commands to the receiver
 * through telnet.
 *
 * \author Álvaro Cebrián Juan, 2019. acebrianjuan(at)gmail.com
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


#ifndef GNSS_SDR_MONITOR_TELECOMMAND_WIDGET_H_
#define GNSS_SDR_MONITOR_TELECOMMAND_WIDGET_H_

#include "telnet_manager.h"
#include <QHostAddress>
#include <QSettings>
#include <QTcpSocket>
#include <QTimer>
#include <QWidget>

namespace Ui
{
class TelecommandWidget;
}

class TelecommandWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TelecommandWidget(QWidget *parent = nullptr);
    ~TelecommandWidget();

signals:
    void inputsChanged();
    void receivedData(QByteArray data);

    // Forwarded from TelnetManager.
    void connected();
    void disconnected();
    void resetClicked();

public slots:
    void onAddressEditTextchanged();
    void onPortEditTextchanged();
    void handleInputsChanged();
    void clear();

    void onResetClicked();
    void onStandbyClicked();
    void onColdstartClicked();
    void onWarmstartClicked();
    void onHotstartClicked();
    void onStatusClicked();
    void onConnectClicked();
    void reconnect();

    void printText(QByteArray data);

    void setAddress(QString ip);
    void setPort(QString port);

    void statusConnected();
    void statusDisconnected();

    void saveSettings();
    void loadSettings();

    QString getArgs();

    void printError(QAbstractSocket::SocketError socketError);

private:
    Ui::TelecommandWidget *ui;

    TelnetManager m_telnetManager;
    QSettings m_settings;
    QTimer m_timer;
};

#endif  // GNSS_SDR_MONITOR_TELECOMMAND_WIDGET_H_
