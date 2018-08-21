/*!
 * \file cn0_delegate.cpp
 * \brief Implementation of a delegate that draws a CN0 vs. time graph on
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


#include "cn0_delegate.h"

#include <QPainter>
#include <QApplication>
#include <QDebug>

#define SPARKLINE_MIN_EM_WIDTH 10

Cn0_Delegate::Cn0_Delegate(QWidget *parent) : QStyledItemDelegate(parent)
{
    qDebug() << "Cn0_Delegate" << "\t" << "Constructed";
}

Cn0_Delegate::~Cn0_Delegate()
{

}

void Cn0_Delegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
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


    QCustomPlot *plot;

    if (plots.find(index.row()) == plots.end())
    {
        QColor text_color = QColor("#EFF0F1");
        QColor grid_color = QColor("#4D4D4D");
        QColor background_color = QColor("#333333");
        QColor marker_color = QColor("#FFAA00");

        plot = new QCustomPlot();
        plots[index.row()] = plot;

        plot->setVisible(false);
        plot->addGraph();

        QPen marker_pen;
        marker_color.setAlpha(150);
        marker_pen.setColor(marker_color);
        plot->graph(0)->setPen(marker_pen);
        plot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 2));

        plot->xAxis->setLabel("RX Time [s]");
        plot->xAxis->setLabelColor(text_color);
        plot->xAxis->setTickLabelRotation(30);
        plot->xAxis->setSubTicks(false);

        plot->yAxis->setLabel("C/N0 [db-Hz]");
        plot->yAxis->setLabelColor(text_color);
        plot->yAxis->setSubTicks(false);

        plot->xAxis2->setVisible(true);
        plot->xAxis2->setTicks(false);
        plot->xAxis2->setTickLabels(false);
        plot->xAxis2->setBasePen(Qt::NoPen);

        plot->yAxis2->setVisible(true);
        plot->yAxis2->setTicks(false);
        plot->yAxis2->setTickLabels(false);
        plot->yAxis2->setBasePen(Qt::NoPen);

        // add title layout element:
        QString title = "C/N0";
        title.append(" CH ").append(index.model()->data(index.siblingAtColumn(0), Qt::DisplayRole).toString());
        index.model()->headerData(index.column(), Qt::Horizontal, Qt::DisplayRole).toString();
        plot->xAxis2->setLabel(title);
        plot->xAxis2->setLabelFont(QFont("sans", 10, QFont::Bold));
        plot->xAxis2->setLabelColor(text_color);

        // set some pens, brushes and backgrounds:
        plot->xAxis->setBasePen(QPen(text_color, 1));
        plot->yAxis->setBasePen(QPen(text_color, 1));

        plot->xAxis->setTickPen(QPen(text_color, 1));
        plot->yAxis->setTickPen(QPen(text_color, 1));

        plot->xAxis->setSubTickPen(QPen(text_color, 1));
        plot->yAxis->setSubTickPen(QPen(text_color, 1));

        plot->xAxis->setTickLabelColor(text_color);
        plot->yAxis->setTickLabelColor(text_color);

        plot->xAxis->grid()->setPen(QPen(grid_color, 1, Qt::SolidLine));
        plot->yAxis->grid()->setPen(QPen(grid_color, 1, Qt::SolidLine));

        plot->xAxis->grid()->setZeroLinePen(Qt::NoPen);
        plot->yAxis->grid()->setZeroLinePen(Qt::NoPen);

        QCPLineEnding le = QCPLineEnding::esHalfBar;
        le.setInverted(true);
        plot->xAxis->setUpperEnding(le);
        plot->yAxis->setUpperEnding(QCPLineEnding::esHalfBar);

        plot->setBackground(QBrush(background_color, Qt::SolidPattern));
        plot->axisRect()->setBackground(QBrush(background_color, Qt::SolidPattern));
    }
    else
    {
        plot = plots.at(index.row());
    }

    plot->graph(0)->setData(x_data, y_data);
    plot->graph(0)->rescaleAxes(true);
    plot->xAxis->setRange(x_data.first(), x_data.last());
    plot->replot();


    double min_y = 0;
    double max_y = 1;
    int em_w = option.fontMetrics.height();
    int content_w = option.rect.width() - (em_w / 4);
    int content_h = option.fontMetrics.height();
    QPointF val;
    qreal idx = 0.0;
    qreal step_w = em_w / 10.0;
    qreal steps = content_w / step_w;
    QVector<QPointF> fpoints;


    QStyledItemDelegate::paint(painter, option, index);

    if (points.isEmpty() || steps < 1.0 || content_h <= 0)
    {
        return;
    }

    while((qreal) points.length() > steps)
    {
        points.removeFirst();
    }

    foreach (val, points)
    {
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
        fpoints.append(QPointF(idx, (qreal) content_h - (content_h * (val.y() - min_y) / (max_y - min_y))));
        idx = idx + step_w;
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

QSize Cn0_Delegate::sizeHint(const QStyleOptionViewItem &option,
                                   const QModelIndex &index) const
{
    return QSize(option.fontMetrics.height() * SPARKLINE_MIN_EM_WIDTH, QStyledItemDelegate::sizeHint(option, index).height());
}

bool Cn0_Delegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if ( event->type() == QEvent::MouseButtonRelease )
    {
        qDebug() << "Click : " << index.row();

        QCustomPlot *plot = plots.at(index.row());
        if (!plot->isVisible())
        {
            plot->setGeometry(0, 0, 300, 200);
            plot->setVisible(true);
        }
    }
    return true;
}
