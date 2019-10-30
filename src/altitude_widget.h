/*!
 * \file altitude_widget.h
 * \brief Interface of a widget that shows the altitude in a chart as
 * reported by the receiver.
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


#ifndef GNSS_SDR_MONITOR_ALTITUDE_WIDGET_H_
#define GNSS_SDR_MONITOR_ALTITUDE_WIDGET_H_

#include <boost/circular_buffer.hpp>
#include <QChartView>
#include <QLineSeries>
#include <QWidget>

class AltitudeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AltitudeWidget(QWidget *parent = nullptr);

public slots:
    void addData(qreal tow, qreal altitude);
    void redraw();
    void clear();
    void setBufferSize(size_t size);

private:
    size_t m_bufferSize;
    boost::circular_buffer<QPointF> m_altitudeBuffer;
    QtCharts::QChartView *m_chartView = nullptr;
    QtCharts::QLineSeries *m_series = nullptr;

    double min_x;
    double min_y;

    double max_x;
    double max_y;
};

#endif  // GNSS_SDR_MONITOR_ALTITUDE_WIDGET_H_
