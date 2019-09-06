#include "altitude_widget.h"
#include "ui_altitude_widget.h"

#include "float.h"

AltitudeWidget::AltitudeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AltitudeWidget)
{
    ui->setupUi(this);

    QtCharts::QChart *chart = ui->graphicsView->chart();
    QtCharts::QChartView *chartView = ui->graphicsView;

    chart->setTitle("Altitude vs Time");
    chart->legend()->hide();

    chart->addSeries(&m_series);
    chart->createDefaultAxes();
    chart->axisX()->setTitleText("TOW [s]");
    chart->axisY()->setTitleText("Altitude [m]");
    chart->layout()->setContentsMargins(0, 0, 0, 0);
    chart->setContentsMargins(-18, -18, -14, -16);

    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setContentsMargins(0, 0, 0, 0);

    min_x = DBL_MAX;
    min_y = DBL_MAX;

    max_x = -DBL_MAX;
    max_y = -DBL_MAX;
}

AltitudeWidget::~AltitudeWidget()
{
    delete ui;
}

void AltitudeWidget::enqueueNewData(qreal tow, qreal altitude)
{
    m_queue.enqueue(QPointF(tow, altitude));
}

void AltitudeWidget::redraw()
{
    if (!m_queue.isEmpty())
    {
        QtCharts::QChart *chart = ui->graphicsView->chart();
        QtCharts::QChartView *chartView = ui->graphicsView;

        QPointF p;

        for (int i = 0; i < m_queue.size(); i++)
        {
            p = m_queue.at(i);

            min_x = std::min(min_x, p.x());
            min_y = std::min(min_y, p.y());

            max_x = std::max(max_x, p.x());
            max_y = std::max(max_y, p.y());
        }

        m_series.append(m_queue);

        chart->axisX()->setRange(min_x, max_x);
        chart->axisY()->setRange(min_y, max_y);

        m_queue.clear();
    }
}

void AltitudeWidget::clear()
{
    m_queue.clear();
    m_series.clear();
}
