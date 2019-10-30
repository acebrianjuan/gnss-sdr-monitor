/*!
 * \file constellation_delegate.cpp
 * \brief Implementation of a delegate that draws a constellation diagram on
 * the view using the information from the model.
 *
 * \author Álvaro Cebrián Juan, 2018. acebrianjuan(at)gmail.com
 *
 * -----------------------------------------------------------------------
 *
 * Copyright (C) 2010-2019  (see AUTHORS file for a list of contributors)
 *
 * GNSS-SDR is a software defined Global Navigation
 *      Satellite Systems receiver
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
 * -----------------------------------------------------------------------
 */


#include "constellation_delegate.h"
#include <QApplication>
#include <QDebug>
#include <QPainter>

#define SPARKLINE_MIN_EM_WIDTH 10

ConstellationDelegate::ConstellationDelegate(QWidget *parent) : QStyledItemDelegate(parent)
{
}

ConstellationDelegate::~ConstellationDelegate()
{
}

void ConstellationDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
    const QModelIndex &index) const
{
    QList<QPointF> points;
    QVector<double> x_data, y_data;
    QList<QVariant> var = index.data(Qt::DisplayRole).toList();
    for (int i = 0; i < var.size(); i++)
    {
        points << var.at(i).toPointF();
        x_data << var.at(i).toPointF().x();
        y_data << var.at(i).toPointF().y();
    }


    double min_x = 0;
    double max_x = 1;
    double min_y = 0;
    double max_y = 1;
    int em_w = option.fontMetrics.height();
    int content_w = option.rect.width() - (2 * em_w);
    int content_h = option.fontMetrics.height();  //option.fontMetrics.ascent();

    int cell_origin_x = option.rect.x();
    int cell_origin_y = option.rect.y();

    int offset_x = cell_origin_x + em_w + 0.5;
    int offset_y = cell_origin_y + ((option.rect.height() - content_h) / 2) + 0.5;

    int button_h = option.fontMetrics.height();
    int button_w = button_h;

    // Paint using default painter.
    QStyledItemDelegate::paint(painter, option, index);

    // If any of this occurs, don't continue.
    if (points.isEmpty() /* || points.size() < 4 */ || content_h <= 0)
    {
        return;
    }

    QPointF p;
    foreach (p, points)
    {
        if (p.x() < min_x)
        {
            min_x = p.x();
        }

        if (p.x() > max_x)
        {
            max_x = p.x();
        }

        if (p.y() < min_y)
        {
            min_y = p.y();
        }

        if (p.y() > max_y)
        {
            max_y = p.y();
        }
    }

    QVector<QPointF> fpoints;
    foreach (p, points)
    {
        fpoints.append(QPointF((qreal)content_w * (p.x() - min_x) / (max_x - min_x),
            (qreal)content_h - (content_h * (p.y() - min_y) / (max_y - min_y))));
    }

    QStyleOptionViewItem option_vi = option;
    QStyledItemDelegate::initStyleOption(&option_vi, index);

    painter->save();

    if (QApplication::style()->objectName().contains("vista"))
    {
        // QWindowsVistaStyle::drawControl does this internally. Unfortunately there
        // doesn't appear to be a more general way to do this.
        option_vi.palette.setColor(QPalette::All, QPalette::HighlightedText, option_vi.palette.color(QPalette::Active, QPalette::Text));
    }

    QPalette::ColorGroup cg = option_vi.state & QStyle::State_Enabled
                                  ? QPalette::Normal
                                  : QPalette::Disabled;
    if (cg == QPalette::Normal && !(option_vi.state & QStyle::State_Active))
        cg = QPalette::Inactive;
#if defined(Q_OS_WIN)
    if (option_vi.state & QStyle::State_Selected)
    {
#else
    if ((option_vi.state & QStyle::State_Selected) && !(option_vi.state & QStyle::State_MouseOver))
    {
#endif
        painter->setPen(option_vi.palette.color(cg, QPalette::HighlightedText));
    }
    else
    {
        QPen pen;

        //pen.setBrush(Qt::gray);
        pen.setColor(QColor(160, 160, 164, 150));
        pen.setWidth(2);
        pen.setCapStyle(Qt::RoundCap);
        pen.setJoinStyle(Qt::RoundJoin);

        painter->setPen(pen);
        //painter->setPen(option_vi.palette.color(cg, QPalette::Text));
    }

    painter->setRenderHint(QPainter::Antialiasing, true);

    painter->translate(offset_x, offset_y);

    painter->drawPoints(QPolygonF(fpoints));

    painter->setPen(Qt::NoPen);
    painter->setBrush(QBrush(QColor("#FF4136"), Qt::SolidPattern));
    painter->drawEllipse(fpoints.last(), 2, 2);

    painter->restore();


    /*
    // DEBUG: Paint borders.
    painter->setPen(Qt::red); // Red pen.

    painter->drawRect(option.rect);  // Cell border.

    painter->drawRect(offset_x, offset_y,
                      content_w, content_h);  // Constellation border.

    painter->setPen(Qt::cyan); // cyan pen.
    painter->drawLine(option.rect.x(), option.rect.y() + option.rect.height() / 2,
                      option.rect.x() + option.rect.width(), option.rect.y() + option.rect.height() / 2); // Centerline.
    */
}

QSize ConstellationDelegate::sizeHint(const QStyleOptionViewItem &option,
    const QModelIndex &index) const
{
    return QSize(option.fontMetrics.height() * SPARKLINE_MIN_EM_WIDTH, QStyledItemDelegate::sizeHint(option, index).height());
}
