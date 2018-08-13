#include "led_delegate.h"

#include <QPainter>
#include <QDebug>

LED_Delegate::LED_Delegate(QObject *parent)
{
    qDebug() << "LED_Delegate" << "\t" << "Constructed";
}

LED_Delegate::~LED_Delegate()
{

}

void LED_Delegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
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

    /*
    if (option.state & QStyle::State_Selected)
    {
        painter->setBrush(option.palette.highlightedText());
    }
    else
    {
        painter->setBrush(option.palette.text());
    }
    */

    painter->drawEllipse(QRectF(option.rect.x() + option.rect.width() / 2 - radius,
                                option.rect.y() + option.rect.height() / 2 - radius,
                                2 * radius, 2 * radius));

    painter->restore();
}

QSize LED_Delegate::sizeHint(const QStyleOptionViewItem &option,
                             const QModelIndex &index) const
{
    return QSize(QStyledItemDelegate::sizeHint(option, index).height(),
                 QStyledItemDelegate::sizeHint(option, index).height());
}
