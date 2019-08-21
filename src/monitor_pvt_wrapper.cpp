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

MonitorPvtWrapper::MonitorPvtWrapper(QObject *parent) : QObject(parent) {}

void MonitorPvtWrapper::addMonitorPvt(
    const gnss_sdr::MonitorPvt &monitor_pvt)
{
    m_listMonitorPvt << monitor_pvt;
    m_path.append(QVariant::fromValue(
        QGeoCoordinate(monitor_pvt.latitude(), monitor_pvt.longitude())));

    emit dataChanged();
    emit altitudeChanged(monitor_pvt.tow_at_current_symbol_ms(), monitor_pvt.height());
}

void MonitorPvtWrapper::clearData()
{
    m_listMonitorPvt.clear();
    m_path.clear();

    emit dataChanged();
}

QVariant MonitorPvtWrapper::position() const
{
    if (!m_listMonitorPvt.isEmpty())
        {
            return QVariant::fromValue(
                QGeoCoordinate(m_listMonitorPvt.last().latitude(),
                    m_listMonitorPvt.last().longitude()));
        }
    else
        {
            return QVariant();
        }
}

QVariantList MonitorPvtWrapper::path() const
{
    if (!m_path.isEmpty())
        {
            return m_path;
        }
    else
        {
            return QVariantList();
        }
}
