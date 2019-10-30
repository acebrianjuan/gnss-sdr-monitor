/*!
 * \file telnet_manager.h
 * \brief Interface of a connection manager class for sending
 * commands to the receiver through telnet.
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


#ifndef GNSS_SDR_MONITOR_TELNET_MANAGER_H_
#define GNSS_SDR_MONITOR_TELNET_MANAGER_H_

#include <QHostAddress>
#include <QObject>
#include <QTcpSocket>

class TelnetManager : public QObject
{
    Q_OBJECT

public:
    enum class Command
    {
        Reset,
        Standby,
        ColdStart,
        WarmStart,
        HotStart,
        Status,
        Exit
    };

    explicit TelnetManager(QObject *parent = nullptr);

    void setAddress(QString addr_str);
    void setPort(QString port_str);

    QHostAddress getAddress() const;
    quint16 getPort() const;
    QAbstractSocket::SocketState getState() const;

signals:
    void txData(QByteArray data);
    void rxData(QByteArray data);

    // Forwarded from QAbstractSocket.
    void connected();
    void disconnected();
    void stateChanged(QAbstractSocket::SocketState socketState);
    void error(QAbstractSocket::SocketError socketError);

public slots:
    void connectTcp();
    void disconnectTcp();
    bool sendCommand(Command cmd, QString args = "");
    void readResponse();

private:
    QTcpSocket *m_tcpSocket;
    QHostAddress m_address;
    quint16 m_port;
};

#endif  // GNSS_SDR_MONITOR_TELNET_MANAGER_H_
