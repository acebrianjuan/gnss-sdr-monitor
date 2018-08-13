#ifndef DOPPLER_DELEGATE_H
#define DOPPLER_DELEGATE_H

#include <QStyledItemDelegate>
#include "qcustomplot.h"

class Doppler_Delegate : public QStyledItemDelegate
{
public:
    Doppler_Delegate(QWidget *parent = nullptr);
    ~Doppler_Delegate();

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const;

    bool editorEvent(QEvent *event, QAbstractItemModel *model,
                     const QStyleOptionViewItem &option, const QModelIndex &index);

private:
    mutable std::vector<int> channels_id;
    mutable std::map<int, QCustomPlot*> plots;

signals:

public slots:

};

#endif // DOPPLER_DELEGATE_H
