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
