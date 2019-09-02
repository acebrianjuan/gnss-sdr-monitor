#ifndef GNSS_SDR_MONITOR_ALTITUDE_WIDGET_H_
#define GNSS_SDR_MONITOR_ALTITUDE_WIDGET_H_

#include <QWidget>
#include <QLineSeries>
#include <QQueue>

namespace Ui {
class AltitudeWidget;
}

class AltitudeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AltitudeWidget(QWidget *parent = nullptr);
    ~AltitudeWidget();

public slots:
    void enqueueNewData(qreal tow, qreal altitude);
    void redraw();
    void clear();

private:
    Ui::AltitudeWidget *ui;
    QtCharts::QLineSeries m_series;
    QQueue<QPointF> m_queue;

    double min_x;
    double min_y;

    double max_x;
    double max_y;
};

#endif // GNSS_SDR_MONITOR_ALTITUDE_WIDGET_H_
