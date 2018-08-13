#ifndef CN0_DELEGATE_H
#define CN0_DELEGATE_H

#include <QStyledItemDelegate>
#include "qcustomplot.h"

class Cn0_Delegate : public QStyledItemDelegate
{
public:
    Cn0_Delegate(QWidget *parent = nullptr);
    ~Cn0_Delegate();

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

#endif // CN0_DELEGATE_H
