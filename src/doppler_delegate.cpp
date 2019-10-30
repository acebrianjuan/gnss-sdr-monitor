/*!
 * \file doppler_delegate.cpp
 * \brief Implementation of a delegate that draws a Doppler vs. time graph on
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


#include "doppler_delegate.h"
#include <QApplication>
#include <QDebug>
#include <QPainter>
#include <limits>

#define SPARKLINE_MIN_EM_WIDTH 10

DopplerDelegate::DopplerDelegate(QWidget *parent) : QStyledItemDelegate(parent)
{
    // Default buffer size.
    m_bufferSize = 100;
}

DopplerDelegate::~DopplerDelegate()
{
}

void DopplerDelegate::setBufferSize(int size)
{
    m_bufferSize = size;
}

void DopplerDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
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

    double min_x = std::numeric_limits<double>::max();
    double max_x = -std::numeric_limits<double>::max();

    double min_y = std::numeric_limits<double>::max();
    double max_y = -std::numeric_limits<double>::max();

    int em_w = option.fontMetrics.height();

    int hGap = em_w / 4;
    int vGap = (option.rect.height() - option.fontMetrics.height()) / 2;
    int cGap = em_w / 4;

    int cellWidth = option.rect.width();
    int cellHeight = option.rect.height();

    int contentWidth = cellWidth - 2 * hGap;
    int contentHeight = option.fontMetrics.height();

    int usableContentWidth = contentWidth - cGap;
    int textWidth = option.fontMetrics.width("-00000.0");
    int sparklineWidth = usableContentWidth - textWidth;

    // Offset for translating the origin of the painting coordinate system to the top left corner of the cell.
    QPoint offset = option.rect.topLeft();

    // Translated origins.
    QPoint cellOrigin = QPoint(0, 0);
    QPoint sparklineOrigin = QPoint(hGap, vGap);
    QPoint textOrigin = QPoint(hGap + sparklineWidth + cGap, vGap);

    // Translated rectangles.
    QRect cellRect = QRect(cellOrigin, QSize(cellWidth, cellHeight));
    QRect sparklineRect = QRect(sparklineOrigin, QSize(sparklineWidth, contentHeight));
    QRect textRect = QRect(textOrigin, QSize(textWidth, contentHeight));

    QPointF val;
    QVector<QPointF> fpoints;
    QStyledItemDelegate::paint(painter, option, index);

    if (points.isEmpty() || m_bufferSize < 1.0 || contentHeight <= 0)
    {
        return;
    }

    while (points.length() > m_bufferSize)
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
        double x = sparklineWidth * (val.x() - min_x) / (max_x - min_x);
        double y = contentHeight - (contentHeight * (val.y() - min_y) / (max_y - min_y));
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
        painter->setPen(option_vi.palette.color(cg, QPalette::Text));
    }

    // Enable antialiasing.
    painter->setRenderHint(QPainter::Antialiasing, true);

    // Translate painting origin to sparklineOrigin.
    painter->translate(offset.x() + hGap, offset.y() + vGap);

    // Fill area below the Doppler sparkline.
    QPointF startPoint(fpoints.first().x(), contentHeight);
    QPointF endPoint(fpoints.last().x(), contentHeight);
    fpoints.push_front(startPoint);
    fpoints.push_back(endPoint);

    QLinearGradient gradient(QPointF(0, 0), QPointF(0, contentHeight));
    gradient.setColorAt(1, Qt::white);
    gradient.setColorAt(0, Qt::gray);

    painter->setBrush(QBrush(gradient));
    painter->setPen(Qt::NoPen);
    painter->drawPolygon(QPolygonF(fpoints));

    // Draw Doppler sparkline.
    fpoints.removeFirst();
    fpoints.removeLast();
    painter->setPen(Qt::black);
    painter->drawPolyline(QPolygonF(fpoints));

    // Translate painting origin to cellOrigin.
    painter->translate(-hGap, -vGap);

    // Display value of the last Doppler sample next to the sparkline.
    painter->drawText(textRect, QString::number(var.last().toPointF().y(), 'f', 1));

    // Draw visual guides for debugging.
    //drawGuides(painter, cellRect, sparklineRect, textRect);

    painter->restore();
}

QSize DopplerDelegate::sizeHint(const QStyleOptionViewItem &option,
    const QModelIndex &index) const
{
    return QSize(option.fontMetrics.height() * SPARKLINE_MIN_EM_WIDTH, QStyledItemDelegate::sizeHint(option, index).height());
}

/*!
  Draws a set of visual guides to assist in the debugging of the delegate design.
 */
void DopplerDelegate::drawGuides(QPainter *painter, QRect cellRect, QRect sparklineRect, QRect textRect) const
{
    // Set pen color to red.
    painter->setPen(Qt::red);
    painter->setBrush(Qt::NoBrush);

    // Draw cell border.
    painter->drawRect(cellRect);

    // Draw sparkline border.
    painter->drawRect(sparklineRect);

    // Draw text border.
    painter->drawRect(textRect);

    // Draw centerline in cyan.
    painter->setPen(Qt::cyan);
    painter->drawLine(cellRect.x(), cellRect.y() + cellRect.height() / 2,
        cellRect.x() + cellRect.width(), cellRect.y() + cellRect.height() / 2);

    // Create a new pen with increased width for drawing origins.
    QPen pen = QPen();
    pen.setWidth(5);

    // Draw cell origin.
    pen.setColor(Qt::red);
    painter->setPen(pen);
    painter->drawPoint(cellRect.topLeft());

    // Draw sparkline origin.
    pen.setColor(Qt::green);
    painter->setPen(pen);
    painter->drawPoint(sparklineRect.topLeft());

    // Draw text origin.
    pen.setColor(Qt::blue);
    painter->setPen(pen);
    painter->drawPoint(textRect.topLeft());
}
