#ifndef MONITOR_PVT_WRAPPER_H
#define MONITOR_PVT_WRAPPER_H

#include "monitor_pvt.h"

#include <QObject>
#include <QVariant>

class Monitor_Pvt_Wrapper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariant position READ position NOTIFY dataChanged)
    Q_PROPERTY(QVariantList path READ path NOTIFY dataChanged)

public:
    explicit Monitor_Pvt_Wrapper(QObject *parent = nullptr);
    void add_monitor_pvt(const Monitor_Pvt &monitor_pvt);
    QVariant position() const;
    QVariantList path() const;

signals:
    void dataChanged();

public slots:
    void clear_data();

private:
    QList<Monitor_Pvt> m_list_monitor_pvt;
    QVariantList m_path;
};

#endif // MONITOR_PVT_WRAPPER_H
