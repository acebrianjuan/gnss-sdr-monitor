/*!
 * \file telnet_manager.cpp
 * \brief Implementation of a connection manager class for sending
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


#include "telnet_manager.h"

TelnetManager::TelnetManager(QObject *parent) : QObject(parent)
{
    m_tcpSocket = new QTcpSocket(this);
    connect(m_tcpSocket, &QIODevice::readyRead, this, &TelnetManager::readResponse);

    // Forward signals from QAbstractSocket.
    connect(m_tcpSocket, &QAbstractSocket::connected, this, &TelnetManager::connected);
    connect(m_tcpSocket, &QAbstractSocket::disconnected, this, &TelnetManager::disconnected);
    connect(m_tcpSocket, &QAbstractSocket::stateChanged, this, &TelnetManager::stateChanged);
    connect(m_tcpSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &TelnetManager::error);
}

void TelnetManager::setAddress(QString addr_str)
{
    m_address.setAddress(addr_str);
}

void TelnetManager::setPort(QString port_str)
{
    m_port = port_str.toInt();
}

QHostAddress TelnetManager::getAddress() const
{
    return m_address;
}

quint16 TelnetManager::getPort() const
{
    return m_port;
}

QAbstractSocket::SocketState TelnetManager::getState() const
{
    return m_tcpSocket->state();
}

void TelnetManager::connectTcp()
{
    m_tcpSocket->connectToHost(m_address, m_port);
}

void TelnetManager::disconnectTcp()
{
    m_tcpSocket->disconnectFromHost();
}

bool TelnetManager::sendCommand(Command cmd, QString args)
{
    if (m_tcpSocket->state() == QAbstractSocket::ConnectedState)
    {
        QByteArray data;

        switch (cmd)
        {
        case Command::Reset:
            data = "reset\r\n";
            break;
        case Command::Standby:
            data = "standby\r\n";
            break;
        case Command::ColdStart:
            data = "coldstart\r\n";
            break;
        case Command::WarmStart:
            data = QString("%1 %2 %3").arg("warmstart", args, "\r\n").toUtf8();
            break;
        case Command::HotStart:
            data = QString("%1 %2 %3").arg("hotstart", args, "\r\n").toUtf8();
            break;
        case Command::Status:
            data = "status\r\n";
            break;
        case Command::Exit:
            data = "exit\r\n";
            break;
        default:
            data = "";
            break;
        }

        if (!data.isEmpty())
        {
            m_tcpSocket->write(data, data.size());

            if (m_tcpSocket->waitForBytesWritten())
            {
                emit txData(data);
                return true;
            }
        }
    }
    return false;
}

void TelnetManager::readResponse()
{
    QByteArray data = m_tcpSocket->readAll();
    emit rxData(data);
}
