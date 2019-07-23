/*!
 * \file monitor_pvt_wrapper.h
 * \brief Interface of a wrapper class for MonitorPvt objects.
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


#ifndef GNSS_SDR_MONITOR_MONITOR_PVT_WRAPPER_H_
#define GNSS_SDR_MONITOR_MONITOR_PVT_WRAPPER_H_

#include "monitor_pvt.pb.h"
#include <QObject>
#include <QVariant>

class Monitor_Pvt_Wrapper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariant position READ position NOTIFY dataChanged)
    Q_PROPERTY(QVariantList path READ path NOTIFY dataChanged)

public:
    explicit Monitor_Pvt_Wrapper(QObject *parent = nullptr);
    void add_monitor_pvt(const gnss_sdr::MonitorPvt &monitor_pvt);
    QVariant position() const;
    QVariantList path() const;

signals:
    void dataChanged();

public slots:
    void clear_data();

private:
    QList<gnss_sdr::MonitorPvt> m_list_monitor_pvt;
    QVariantList m_path;
};

#endif  // GNSS_SDR_MONITOR_MONITOR_PVT_WRAPPER_H_
