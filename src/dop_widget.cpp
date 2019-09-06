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
#include <float.h>

DOPWidget::DOPWidget(QWidget *parent) : QWidget(parent)
{
    m_gdopQueue = new QQueue<QPointF>();
    m_pdopQueue = new QQueue<QPointF>();
    m_hdopQueue = new QQueue<QPointF>();
    m_vdopQueue = new QQueue<QPointF>();

    m_gdopSeries = new QtCharts::QLineSeries();
    m_gdopSeries->setName("GDOP");

    m_pdopSeries = new QtCharts::QLineSeries();
    m_pdopSeries->setName("PDOP");

    m_hdopSeries = new QtCharts::QLineSeries();
    m_hdopSeries->setName("HDOP");

    m_vdopSeries = new QtCharts::QLineSeries();
    m_vdopSeries->setName("VDOP");

    chartView = new QtCharts::QChartView(this);
    QVBoxLayout *layout = new QVBoxLayout(this);
    this->setLayout(layout);
    layout->addWidget(chartView);

    QtCharts::QChart *chart = chartView->chart();
    chart->addSeries(m_gdopSeries);
    chart->addSeries(m_pdopSeries);
    chart->addSeries(m_hdopSeries);
    chart->addSeries(m_vdopSeries);

    chart->setTitle("DOP vs Time");
    chart->createDefaultAxes();
    chart->axisX()->setTitleText("TOW [s]");
    chart->axisY()->setTitleText("DOP");
    chart->layout()->setContentsMargins(0, 0, 0, 0);
    chart->setContentsMargins(-18, -18, -14, -16);

    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setContentsMargins(0, 0, 0, 0);

    min_x = DBL_MAX;
    min_y = DBL_MAX;

    max_x = -DBL_MAX;
    max_y = -DBL_MAX;
}

void DOPWidget::enqueueNewData(qreal tow, qreal gdop, qreal pdop, qreal hdop, qreal vdop)
{
    m_gdopQueue->enqueue(QPointF(tow, gdop));
    m_pdopQueue->enqueue(QPointF(tow, pdop));
    m_hdopQueue->enqueue(QPointF(tow, hdop));
    m_vdopQueue->enqueue(QPointF(tow, vdop));
}

void DOPWidget::redraw()
{
    populateSeries(m_gdopQueue, m_gdopSeries);
    populateSeries(m_pdopQueue, m_pdopSeries);
    populateSeries(m_hdopQueue, m_hdopSeries);
    populateSeries(m_vdopQueue, m_vdopSeries);
}

void DOPWidget::clear()
{
    m_gdopQueue->clear();
    m_pdopQueue->clear();
    m_hdopQueue->clear();
    m_vdopQueue->clear();

    m_gdopSeries->clear();
    m_pdopSeries->clear();
    m_hdopSeries->clear();
    m_vdopSeries->clear();
}

void DOPWidget::populateSeries(QQueue<QPointF> *queue, QtCharts::QLineSeries *series)
{
    if (!queue->isEmpty())
    {
        QtCharts::QChart *chart = chartView->chart();
        QPointF p;

        for (int i = 0; i < queue->size(); i++)
        {
            p = queue->at(i);

            min_x = std::min(min_x, p.x());
            min_y = std::min(min_y, p.y());

            max_x = std::max(max_x, p.x());
            max_y = std::max(max_y, p.y());
        }

        series->append(*queue);

        chart->axisX()->setRange(min_x, max_x);
        chart->axisY()->setRange(min_y, max_y);

        queue->clear();
    }
}
