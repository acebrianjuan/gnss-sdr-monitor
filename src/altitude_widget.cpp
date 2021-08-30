/*!
 * \file altitude_widget.cpp
 * \brief Implementation of a widget that shows the altitude in a chart as
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


#include "altitude_widget.h"
#include <QChart>
#include <QGraphicsLayout>
#include <QLayout>

/*!
 Constructs an altitude widget.
 */
AltitudeWidget::AltitudeWidget(QWidget *parent) : QWidget(parent)
{
    // Default buffer size.
    m_bufferSize = 100;

    m_altitudeBuffer.resize(m_bufferSize);
    m_altitudeBuffer.clear();

    m_series = new QtCharts::QLineSeries();
    m_chartView = new QtCharts::QChartView(this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    this->setLayout(layout);
    layout->addWidget(m_chartView);

    QtCharts::QChart *chart = m_chartView->chart();

    chart->addSeries(m_series);
    chart->setTitle("Altitude vs Time");
    chart->legend()->hide();
    chart->createDefaultAxes();
    chart->axes(Qt::Horizontal).back()->setTitleText("TOW [s]");
    chart->axes(Qt::Vertical).back()->setTitleText("Altitude [m]");
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
 Adds the \a alitude and associated \a tow to the widget's internal data structures.
 */
void AltitudeWidget::addData(qreal tow, qreal altitude)
{
    m_altitudeBuffer.push_back(QPointF(tow, altitude));
}

/*!
 Redraws the chart by replacing the old data in the series object with the new data.
 */
void AltitudeWidget::redraw()
{
    if (!m_altitudeBuffer.empty())
    {
        double min_x = std::numeric_limits<double>::max();
        double max_x = -std::numeric_limits<double>::max();

        double min_y = std::numeric_limits<double>::max();
        double max_y = -std::numeric_limits<double>::max();

        QtCharts::QChart *chart = m_chartView->chart();
        QPointF p;
        QVector<QPointF> vec;

        for (size_t i = 0; i < m_altitudeBuffer.size(); i++)
        {
            p = m_altitudeBuffer.at(i);
            vec << p;

            min_x = std::min(min_x, p.x());
            min_y = std::min(min_y, p.y());

            max_x = std::max(max_x, p.x());
            max_y = std::max(max_y, p.y());
        }

        m_series->replace(vec);

        chart->axes(Qt::Horizontal).back()->setRange(min_x, max_x);
        chart->axes(Qt::Vertical).back()->setRange(min_y, max_y);
    }
}

/*!
 Clears all the data from the widget's internal data structures.
 */
void AltitudeWidget::clear()
{
    m_altitudeBuffer.clear();
    m_series->clear();
}

/*!
 Sets the size of the internal circular buffer that stores the widget's data.
 */
void AltitudeWidget::setBufferSize(size_t size)
{
    m_bufferSize = size;
    m_altitudeBuffer.resize(m_bufferSize);
}
