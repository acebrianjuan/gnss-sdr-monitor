#include "altitude_widget.h"
#include <QChart>
#include <QGraphicsLayout>
#include <QLayout>
#include <float.h>

AltitudeWidget::AltitudeWidget(QWidget *parent) : QWidget(parent)
{
    m_queue = new QQueue<QPointF>();

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
    chart->axisX()->setTitleText("TOW [s]");
    chart->axisY()->setTitleText("Altitude [m]");
    chart->layout()->setContentsMargins(0, 0, 0, 0);
    chart->setContentsMargins(-18, -18, -14, -16);

    m_chartView->setRenderHint(QPainter::Antialiasing);
    m_chartView->setContentsMargins(0, 0, 0, 0);

    min_x = DBL_MAX;
    min_y = DBL_MAX;

    max_x = -DBL_MAX;
    max_y = -DBL_MAX;
}

void AltitudeWidget::enqueueNewData(qreal tow, qreal altitude)
{
    m_queue->enqueue(QPointF(tow, altitude));
}

void AltitudeWidget::redraw()
{
    if (!m_queue->isEmpty())
    {
        QtCharts::QChart *chart = m_chartView->chart();
        QPointF p;

        for (int i = 0; i < m_queue->size(); i++)
        {
            p = m_queue->at(i);

            min_x = std::min(min_x, p.x());
            min_y = std::min(min_y, p.y());

            max_x = std::max(max_x, p.x());
            max_y = std::max(max_y, p.y());
        }

        m_series->append(*m_queue);

        chart->axisX()->setRange(min_x, max_x);
        chart->axisY()->setRange(min_y, max_y);

        m_queue->clear();
    }
}

void AltitudeWidget::clear()
{
    m_queue->clear();
    m_series->clear();
}
