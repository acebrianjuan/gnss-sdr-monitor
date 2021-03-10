/*!
 * \file monitor_pvt_wrapper.cpp
 * \brief Implementation of a wrapper class for MonitorPvt objects.
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


#include "monitor_pvt_wrapper.h"
#include <QDebug>
#include <QGeoCoordinate>

/*!
 Constructs a MonitorPvtWrapper object.
 */
MonitorPvtWrapper::MonitorPvtWrapper(QObject *parent) : QObject(parent)
{
    m_bufferSize = 100;

    m_bufferMonitorPvt.resize(m_bufferSize);
    m_bufferMonitorPvt.clear();

    m_path.resize(m_bufferSize);
    m_path.clear();
}

/*!
 Populates the internal data structures with the data of the \a monitor_pvt MonitorPvt object.
 */
void MonitorPvtWrapper::addMonitorPvt(const gnss_sdr::MonitorPvt &monitor_pvt)
{
    m_bufferMonitorPvt.push_back(monitor_pvt);

    Coordinates coord;
    coord.latitude = monitor_pvt.latitude();
    coord.longitude = monitor_pvt.longitude();
    m_path.push_back(coord);

    emit dataChanged();
    emit altitudeChanged(monitor_pvt.tow_at_current_symbol_ms(), monitor_pvt.height());
    emit dopChanged(monitor_pvt.tow_at_current_symbol_ms(), monitor_pvt.gdop(), monitor_pvt.pdop(), monitor_pvt.hdop(), monitor_pvt.vdop());
}

/*!
 Gets the last MonitorPvt object.
 */
gnss_sdr::MonitorPvt MonitorPvtWrapper::getLastMonitorPvt()
{
    return m_bufferMonitorPvt.back();
}

/*!
 Clears all the data from the internal data structures.
 */
void MonitorPvtWrapper::clearData()
{
    m_bufferMonitorPvt.clear();
    m_path.clear();

    emit dataChanged();
}

/*!
 Sets the size of the internal circular buffers that store the data.
 */
void MonitorPvtWrapper::setBufferSize(size_t size)
{
    m_bufferSize = size;
    m_bufferMonitorPvt.resize(m_bufferSize);
}

/*!
 Returns the last known position.
 */
QVariant MonitorPvtWrapper::position() const
{
    if (!m_bufferMonitorPvt.empty())
    {
        gnss_sdr::MonitorPvt mpvt = m_bufferMonitorPvt.back();
        return QVariant::fromValue(QGeoCoordinate(mpvt.latitude(), mpvt.longitude()));
    }
    else
    {
        return QVariant::fromValue(QGeoCoordinate());
    }
}

/*!
 Returns the path formed by the history of recorded positions.
 */
QVariantList MonitorPvtWrapper::path() const
{
    if (!m_path.empty())
    {
        QVariantList list;
        for (size_t i = 0; i < m_path.size(); i++)
        {
            Coordinates coord = m_path.at(i);
            list << QVariant::fromValue(QGeoCoordinate(coord.latitude, coord.longitude));
        }
        return list;
    }
    else
    {
        return QVariantList();
    }
}
