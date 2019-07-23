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

Monitor_Pvt_Wrapper::Monitor_Pvt_Wrapper(QObject *parent) : QObject(parent) {}

void Monitor_Pvt_Wrapper::add_monitor_pvt(
    const gnss_sdr::MonitorPvt &monitor_pvt)
{
    m_list_monitor_pvt << monitor_pvt;
    m_path.append(QVariant::fromValue(
        QGeoCoordinate(monitor_pvt.latitude(), monitor_pvt.longitude())));

    emit dataChanged();
}

void Monitor_Pvt_Wrapper::clear_data()
{
    m_list_monitor_pvt.clear();
    m_path.clear();

    emit dataChanged();
}

QVariant Monitor_Pvt_Wrapper::position() const
{
    if (!m_list_monitor_pvt.isEmpty())
        {
            return QVariant::fromValue(
                QGeoCoordinate(m_list_monitor_pvt.last().latitude(),
                    m_list_monitor_pvt.last().longitude()));
        }
    else
        {
            return QVariant();
        }
}

QVariantList Monitor_Pvt_Wrapper::path() const
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
