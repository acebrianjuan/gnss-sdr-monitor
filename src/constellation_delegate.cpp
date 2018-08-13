#include "constellation_delegate.h"

#include <QPainter>
#include <QApplication>
#include <QMouseEvent>
#include <QDebug>
#include <QTimer>
#include <QModelIndex>

#define SPARKLINE_MIN_EM_WIDTH 10

Constellation_Delegate::Constellation_Delegate(QWidget *parent) : QStyledItemDelegate(parent)
{
    qDebug() << "Constellation_Delegate" << "\t" << "Constructed";

    timer = new QTimer(this);
    timer->start(1000);
}

Constellation_Delegate::~Constellation_Delegate()
{

}

void Constellation_Delegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
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
        plot->graph(0)->setLineStyle(QCPGraph::lsNone);
        plot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 2));

        plot->xAxis->setLabel("I prompt");
        plot->xAxis->setLabelColor(text_color);
        plot->xAxis->setSubTicks(false);

        plot->yAxis->setLabel("Q prompt");
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
        QString title = index.model()->headerData(index.column(), Qt::Horizontal, Qt::DisplayRole).toString();
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
    plot->rescaleAxes();
    plot->replot();


    double min_x = 0;
    double max_x = 1;
    double min_y = 0;
    double max_y = 1;
    int em_w = option.fontMetrics.height();
    int content_w = option.rect.width() - (3.2 * em_w);
    int content_h = option.fontMetrics.height();//option.fontMetrics.ascent();

    int cell_origin_x = option.rect.x();
    int cell_origin_y = option.rect.y();

    int offset_x = cell_origin_x + (em_w / 2) + 0.5;
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
        fpoints.append(QPointF((qreal) content_w * (p.x() - min_x) / (max_x - min_x),
                               (qreal) content_h - (content_h * (p.y() - min_y) / (max_y - min_y))));
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


    // I/Q Buttons.
    QStyleOptionButton button;
    QRect r = option.rect; //getting the rect of the cell
    int x, y, w, h;
    x = r.left() + r.width() - 40; //the X coordinate
    y = r.top() + ((option.rect.height() - content_h) / 2); //the Y coordinate
    w = button_w; //button width
    h = button_h; //button height
    button.rect = QRect(x, y, w, h);
    button.text = "I";
    button.state = QStyle::State_Enabled;

    QStyleOptionButton button2;
    button2.rect = QRect(x + 20, y, w, h);
    button2.text = "Q";
    button2.state = QStyle::State_Enabled;

    QApplication::style()->drawControl(QStyle::CE_PushButton, &button, painter);
    QApplication::style()->drawControl(QStyle::CE_PushButton, &button2, painter);


    /*
    // DEBUG: Paint borders.
    painter->setPen(Qt::red); // Red pen.

    painter->drawRect(option.rect);  // Cell border.

    painter->drawRect(offset_x, offset_y,
                      content_w, content_h);  // Constellation border.

    painter->drawRect(button.rect);  // I button border.
    painter->drawRect(button2.rect); // Q button border.

    painter->setPen(Qt::cyan); // cyan pen.
    painter->drawLine(option.rect.x(), option.rect.y() + option.rect.height() / 2,
                      option.rect.x() + option.rect.width(), option.rect.y() + option.rect.height() / 2); // Centerline.
    */

}

QSize Constellation_Delegate::sizeHint(const QStyleOptionViewItem &option,
                                       const QModelIndex &index) const
{
    return QSize(option.fontMetrics.height() * SPARKLINE_MIN_EM_WIDTH, QStyledItemDelegate::sizeHint(option, index).height());
}

bool Constellation_Delegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if ( event->type() == QEvent::MouseButtonRelease )
    {
        QMouseEvent * e = (QMouseEvent *)event;
        int click_x = e->x();
        int click_y = e->y();

        int em_w = option.fontMetrics.height();
        int content_w = option.rect.width() - (3.2 * em_w);
        int content_h = option.fontMetrics.height();//option.fontMetrics.ascent();

        int cell_origin_x = option.rect.x();
        int cell_origin_y = option.rect.y();

        int offset_x = cell_origin_x + (em_w / 2) + 0.5;
        int offset_y = cell_origin_y + ((option.rect.height() - content_h) / 2) + 0.5;

        int x, y, w, h;
        x = offset_x;
        y = offset_y;
        w = content_w;
        h = content_h;

        if ( click_x > x && click_x < x + w )
        {
            if ( click_y > y && click_y < y + h )
            {
                qDebug() << "Click : " << index.row();

                QCustomPlot *plot = plots.at(index.row());
                if (!plot->isVisible())
                {
                    plot->setGeometry(0, 0, 300, 200);
                    plot->setVisible(true);
                }
            }
        }
    }
    return true;
}

bool Constellation_Delegate::helpEvent(QHelpEvent *event, QAbstractItemView *view, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if (event->type() == QEvent::ToolTip)
    {
        return true;
    }
}
