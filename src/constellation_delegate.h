#ifndef CONSTELLATION_DELEGATE_H
#define CONSTELLATION_DELEGATE_H

#include <QStyledItemDelegate>
#include <QTimer>
#include "qcustomplot.h"

class Constellation_Delegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    Constellation_Delegate(QWidget *parent = nullptr);
    ~Constellation_Delegate();

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;

    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const;

    bool editorEvent(QEvent *event, QAbstractItemModel *model,
                     const QStyleOptionViewItem &option, const QModelIndex &index);

    bool helpEvent(QHelpEvent *event, QAbstractItemView *view,
                   const QStyleOptionViewItem &option, const QModelIndex &index);

private:
    mutable std::vector<int> channels_id;
    mutable std::map<int, QCustomPlot*> plots;

    QTimer *timer;
    int button_x, button_y;
    //int button_w, button_h;
    //QRect button_r;
    //QVector<double> x_data, y_data;
    QPushButton *button_i;

signals:

public slots:

};

#endif // CONSTELLATION_DELEGATE_H
