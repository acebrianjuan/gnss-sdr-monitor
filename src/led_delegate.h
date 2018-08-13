#ifndef LED_DELEGATE_H
#define LED_DELEGATE_H

#include <QStyledItemDelegate>

class LED_Delegate : public QStyledItemDelegate
{
public:
    LED_Delegate(QObject *parent = nullptr);
    ~LED_Delegate();

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

};

#endif // LED_DELEGATE_H
