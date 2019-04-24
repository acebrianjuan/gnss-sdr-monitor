#include "monitor_pvt_wrapper.h"
#include <QDebug>
#include <QGeoCoordinate>

Monitor_Pvt_Wrapper::Monitor_Pvt_Wrapper(QObject *parent) : QObject(parent) {}

void Monitor_Pvt_Wrapper::add_monitor_pvt(
    const gnss_sdr::MonitorPvt &monitor_pvt)
{
    m_list_monitor_pvt << monitor_pvt;
    m_path.append(QVariant::fromValue(
        QGeoCoordinate(monitor_pvt.latitude(), monitor_pvt.longitude())));

    emit dataChanged();
}

void Monitor_Pvt_Wrapper::clear_data()
{
    m_list_monitor_pvt.clear();
    m_path.clear();

    emit dataChanged();
}

QVariant Monitor_Pvt_Wrapper::position() const
{
    if (!m_list_monitor_pvt.isEmpty())
        {
            return QVariant::fromValue(
                QGeoCoordinate(m_list_monitor_pvt.last().latitude(),
                    m_list_monitor_pvt.last().longitude()));
        }
    else
        {
            return QVariant();
        }
}

QVariantList Monitor_Pvt_Wrapper::path() const
{
    if (!m_path.isEmpty())
        {
            return m_path;
        }
    else
        {
            return QVariantList();
        }
}
