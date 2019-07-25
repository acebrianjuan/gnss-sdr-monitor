/*!
 * \file cn0_delegate.cpp
 * \brief Implementation of a delegate that draws a CN0 vs. time graph on
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


#include "cn0_delegate.h"

#include <limits>
#include <QPainter>
#include <QApplication>
#include <QDebug>

#define SPARKLINE_MIN_EM_WIDTH 10

Cn0Delegate::Cn0Delegate(QWidget *parent) : QStyledItemDelegate(parent)
{
    m_numel = 4000;
}

Cn0Delegate::~Cn0Delegate()
{
}

void Cn0Delegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                         const QModelIndex &index) const
{
    QList<QPointF> points;
    QVector<double> x_data, y_data;
    QList<QVariant> var = index.data(Qt::DisplayRole).toList();
    for(int i = 0; i < var.size(); i++)
    {
        points << var.at(i).toPointF();
        x_data << var.at(i).toPointF().x();
        y_data << var.at(i).toPointF().y();
    }

    double min_x = std::numeric_limits<double>::max();
    double max_x = -std::numeric_limits<double>::max();

    double min_y = std::numeric_limits<double>::max();
    double max_y = -std::numeric_limits<double>::max();

    int em_w = option.fontMetrics.height();
    int content_w = option.rect.width() - (em_w / 4);
    int content_h = option.fontMetrics.height();
    QPointF val;
    QVector<QPointF> fpoints;

    QStyledItemDelegate::paint(painter, option, index);

    if (points.isEmpty() || m_numel < 1.0 || content_h <= 0)
    {
        return;
    }

    while((qreal) points.length() > m_numel)
    {
        points.removeFirst();
    }

    foreach (val, points)
    {
        if (val.x() < min_x)
        {
            min_x = val.x();
        }

        if (val.x() > max_x)
        {
            max_x = val.x();
        }

        if (val.y() < min_y)
        {
            min_y = val.y();
        }

        if (val.y() > max_y)
        {
            max_y = val.y();
        }
    }

    foreach (val, points)
    {
        double x = content_w * (val.x() - min_x) / (max_x - min_x);
        double y = content_h - (content_h * (val.y() - min_y) / (max_y - min_y));
        fpoints.append(QPointF(x, y));
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
            ? QPalette::Normal : QPalette::Disabled;
    if (cg == QPalette::Normal && !(option_vi.state & QStyle::State_Active))
        cg = QPalette::Inactive;
#if defined(Q_OS_WIN)
    if (option_vi.state & QStyle::State_Selected)
    {
#else
    if ((option_vi.state & QStyle::State_Selected) && !(option_vi.state & QStyle::State_MouseOver)) {
#endif
        painter->setPen(option_vi.palette.color(cg, QPalette::HighlightedText));
    }
    else
    {
        painter->setPen(option_vi.palette.color(cg, QPalette::Text));
    }

    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->translate(
                option.rect.x() + (em_w / 8) + 0.5,
                option.rect.y() + ((option.rect.height() - option.fontMetrics.height()) / 2));
    painter->drawPolyline(QPolygonF(fpoints));

    painter->restore();


    /*
    // DEBUG: Paint borders.
    painter->setPen(Qt::red); // Red pen.

    painter->drawRect(option.rect);  // Cell border.

    painter->drawRect(option.rect.x() + (em_w / 8) + 0.5,
                      option.rect.y() + ((option.rect.height() - option.fontMetrics.height()) / 2),
                      content_w, content_h);  // Sparkline border.

    painter->setPen(Qt::cyan); // cyan pen.
    painter->drawLine(option.rect.x(), option.rect.y() + option.rect.height() / 2,
                      option.rect.x() + option.rect.width(), option.rect.y() + option.rect.height() / 2); // Centerline.
    */
}

QSize Cn0Delegate::sizeHint(const QStyleOptionViewItem &option,
                             const QModelIndex &index) const
{
    return QSize(option.fontMetrics.height() * SPARKLINE_MIN_EM_WIDTH, QStyledItemDelegate::sizeHint(option, index).height());
}
