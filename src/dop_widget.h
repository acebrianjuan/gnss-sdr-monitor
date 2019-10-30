/*!
 * \file dop_widget.h
 * \brief Interface of a widget that shows the dilution of precision
 * in a chart as reported by the receiver.
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


#ifndef GNSS_SDR_MONITOR_DOP_WIDGET_H_
#define GNSS_SDR_MONITOR_DOP_WIDGET_H_

#include <boost/circular_buffer.hpp>
#include <QChartView>
#include <QLineSeries>
#include <QWidget>

class DOPWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DOPWidget(QWidget *parent = nullptr);

public slots:
    void addData(qreal tow, qreal gdop, qreal pdop, qreal hdop, qreal vdop);
    void redraw();
    void clear();
    void setBufferSize(size_t size);

private:
    void populateSeries(boost::circular_buffer<QPointF> queue, QtCharts::QLineSeries *series);

    size_t m_bufferSize;

    boost::circular_buffer<QPointF> m_gdopBuffer;
    boost::circular_buffer<QPointF> m_pdopBuffer;
    boost::circular_buffer<QPointF> m_hdopBuffer;
    boost::circular_buffer<QPointF> m_vdopBuffer;

    QtCharts::QChartView *m_chartView = nullptr;

    QtCharts::QLineSeries *m_gdopSeries = nullptr;
    QtCharts::QLineSeries *m_pdopSeries = nullptr;
    QtCharts::QLineSeries *m_hdopSeries = nullptr;
    QtCharts::QLineSeries *m_vdopSeries = nullptr;

    double min_x;
    double min_y;

    double max_x;
    double max_y;
};

#endif  // GNSS_SDR_MONITOR_DOP_WIDGET_H_
