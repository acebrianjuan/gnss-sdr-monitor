/*!
 * \file doppler_delegate.h
 * \brief Interface of a delegate that draws a Doppler vs. time graph on
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


#ifndef GNSS_SDR_MONITOR_DOPPLER_DELEGATE_H_
#define GNSS_SDR_MONITOR_DOPPLER_DELEGATE_H_

#include <QStyledItemDelegate>

class DopplerDelegate : public QStyledItemDelegate
{
public:
    DopplerDelegate(QWidget *parent = nullptr);
    ~DopplerDelegate();

public slots:
    void setBufferSize(int size);

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
        const QModelIndex &index) const;

    QSize sizeHint(const QStyleOptionViewItem &option,
        const QModelIndex &index) const;

private:
    void drawGuides(QPainter *painter, QRect cellRect, QRect sparklineRect, QRect textRect) const;

    int m_bufferSize;
};

#endif  // GNSS_SDR_MONITOR_DOPPLER_DELEGATE_H_
