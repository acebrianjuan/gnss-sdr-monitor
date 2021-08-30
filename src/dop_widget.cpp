/*!
 * \file dop_widget.cpp
 * \brief Implementation of a widget that shows the dilution of precision
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


#include "dop_widget.h"
#include <QChart>
#include <QGraphicsLayout>
#include <QLayout>

/*!
 Constructs a dillution of precision widget.
 */
DOPWidget::DOPWidget(QWidget *parent) : QWidget(parent)
{
    // Default buffer size.
    m_bufferSize = 100;

    m_gdopBuffer.resize(m_bufferSize);
    m_gdopBuffer.clear();

    m_pdopBuffer.resize(m_bufferSize);
    m_pdopBuffer.clear();

    m_hdopBuffer.resize(m_bufferSize);
    m_hdopBuffer.clear();

    m_vdopBuffer.resize(m_bufferSize);
    m_vdopBuffer.clear();

    m_gdopSeries = new QtCharts::QLineSeries();
    m_gdopSeries->setName("GDOP");

    m_pdopSeries = new QtCharts::QLineSeries();
    m_pdopSeries->setName("PDOP");

    m_hdopSeries = new QtCharts::QLineSeries();
    m_hdopSeries->setName("HDOP");

    m_vdopSeries = new QtCharts::QLineSeries();
    m_vdopSeries->setName("VDOP");

    m_chartView = new QtCharts::QChartView(this);
    QVBoxLayout *layout = new QVBoxLayout(this);
    this->setLayout(layout);
    layout->addWidget(m_chartView);

    QtCharts::QChart *chart = m_chartView->chart();
    chart->addSeries(m_gdopSeries);
    chart->addSeries(m_pdopSeries);
    chart->addSeries(m_hdopSeries);
    chart->addSeries(m_vdopSeries);

    chart->setTitle("DOP vs Time");
    chart->createDefaultAxes();
    chart->axes(Qt::Horizontal).back()->setTitleText("TOW [s]");
    chart->axes(Qt::Vertical).back()->setTitleText("DOP");
    chart->layout()->setContentsMargins(0, 0, 0, 0);
    chart->setContentsMargins(-18, -18, -14, -16);

    m_chartView->setRenderHint(QPainter::Antialiasing);
    m_chartView->setContentsMargins(0, 0, 0, 0);

    min_x = std::numeric_limits<double>::max();
    max_x = -std::numeric_limits<double>::max();

    min_y = std::numeric_limits<double>::max();
    max_y = -std::numeric_limits<double>::max();
}

/*!
 Adds the \a gdop, \a pdop, \a hdop, \a vdop and associated \a tow to the widget's internal data structures.
 */
void DOPWidget::addData(qreal tow, qreal gdop, qreal pdop, qreal hdop, qreal vdop)
{
    m_gdopBuffer.push_back(QPointF(tow, gdop));
    m_pdopBuffer.push_back(QPointF(tow, pdop));
    m_hdopBuffer.push_back(QPointF(tow, hdop));
    m_vdopBuffer.push_back(QPointF(tow, vdop));
}

/*!
 Redraws the chart by calling populateSeries() on all series objects.
 */
void DOPWidget::redraw()
{
    populateSeries(m_gdopBuffer, m_gdopSeries);
    populateSeries(m_pdopBuffer, m_pdopSeries);
    populateSeries(m_hdopBuffer, m_hdopSeries);
    populateSeries(m_vdopBuffer, m_vdopSeries);
}

/*!
 Clears all the data from the widget's internal data structures.
 */
void DOPWidget::clear()
{
    m_gdopBuffer.clear();
    m_pdopBuffer.clear();
    m_hdopBuffer.clear();
    m_vdopBuffer.clear();

    m_gdopSeries->clear();
    m_pdopSeries->clear();
    m_hdopSeries->clear();
    m_vdopSeries->clear();
}

/*!
 Sets the size of the internal circular buffers that store the widget's data.
 */
void DOPWidget::setBufferSize(size_t size)
{
    m_bufferSize = size;

    m_gdopBuffer.resize(m_bufferSize);
    m_pdopBuffer.resize(m_bufferSize);
    m_hdopBuffer.resize(m_bufferSize);
    m_vdopBuffer.resize(m_bufferSize);
}

/*!
 Replaces the old data in the \a series object with the new data from the \a buffer, casuing the chart to repaint.
 */
void DOPWidget::populateSeries(boost::circular_buffer<QPointF> buffer, QtCharts::QLineSeries *series)
{
    if (!buffer.empty())
    {
        double min_x = std::numeric_limits<double>::max();
        double max_x = -std::numeric_limits<double>::max();

        /*
        double min_y = std::numeric_limits<double>::max();
        double max_y = -std::numeric_limits<double>::max();
        */

        QtCharts::QChart *chart = m_chartView->chart();
        QPointF p;
        QVector<QPointF> vec;

        for (size_t i = 0; i < buffer.size(); i++)
        {
            p = buffer.at(i);
            vec << p;

            min_x = std::min(min_x, p.x());
            min_y = std::min(min_y, p.y());

            max_x = std::max(max_x, p.x());
            max_y = std::max(max_y, p.y());
        }

        series->replace(vec);

        chart->axes(Qt::Horizontal).back()->setRange(min_x, max_x);
        chart->axes(Qt::Vertical).back()->setRange(min_y, max_y);
    }
}
