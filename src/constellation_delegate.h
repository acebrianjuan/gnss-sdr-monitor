/*!
 * \file constellation_delegate.h
 * \brief Interface of a delegate that draws a constellation diagram on
 * the view using the information from the model.
 *
 * \author Álvaro Cebrián Juan, 2018. acebrianjuan(at)gmail.com
 *
 * -------------------------------------------------------------------------
 *
 * Copyright (C) 2010-2018  (see AUTHORS file for a list of contributors)
 *
 * GNSS-SDR is a software defined Global Navigation
 *          Satellite Systems receiver
 *
 * This file is part of GNSS-SDR.
 *
 * GNSS-SDR is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GNSS-SDR is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNSS-SDR. If not, see <https://www.gnu.org/licenses/>.
 *
 * -------------------------------------------------------------------------
 */


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
