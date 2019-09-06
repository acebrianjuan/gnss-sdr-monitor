#ifndef GNSS_SDR_MONITOR_ALTITUDE_WIDGET_H_
#define GNSS_SDR_MONITOR_ALTITUDE_WIDGET_H_

#include <QWidget>
#include <QChartView>
#include <QLineSeries>
#include <QQueue>

class AltitudeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AltitudeWidget(QWidget *parent = nullptr);

public slots:
    void enqueueNewData(qreal tow, qreal altitude);
    void redraw();
    void clear();

private:
    QQueue<QPointF> *m_queue = nullptr;
    QtCharts::QChartView *m_chartView = nullptr;
    QtCharts::QLineSeries *m_series = nullptr;

    double min_x;
    double min_y;

    double max_x;
    double max_y;
};

#endif // GNSS_SDR_MONITOR_ALTITUDE_WIDGET_H_
