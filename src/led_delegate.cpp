/*!
 * \file led_delegate.cpp
 * \brief Implementation of a delegate that draws an led light indicator on
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


#include "led_delegate.h"
#include <QDebug>
#include <QPainter>

LedDelegate::LedDelegate(QObject *parent)
{
}

LedDelegate::~LedDelegate()
{
}

void LedDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
    const QModelIndex &index) const
{
    if (option.state & QStyle::State_Selected)
        painter->fillRect(option.rect, option.palette.highlight());

    int radius = 0.3 * QStyledItemDelegate::sizeHint(option, index).height();

    int size = qMin(option.rect.width(), option.rect.height());

    bool state = index.model()->data(index, Qt::DisplayRole).toBool();

    painter->save();

    if (state)
    {
        painter->setBrush(QBrush(QColor("#01FF70"), Qt::SolidPattern));
    }
    else
    {
        painter->setBrush(QBrush(QColor("#FF4136"), Qt::SolidPattern));
    }

    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(Qt::NoPen);

    painter->drawEllipse(QRectF(option.rect.x() + option.rect.width() / 2 - radius,
        option.rect.y() + option.rect.height() / 2 - radius,
        2 * radius, 2 * radius));

    painter->restore();
}

QSize LedDelegate::sizeHint(const QStyleOptionViewItem &option,
    const QModelIndex &index) const
{
    return QSize(QStyledItemDelegate::sizeHint(option, index).height(),
        QStyledItemDelegate::sizeHint(option, index).height());
}
