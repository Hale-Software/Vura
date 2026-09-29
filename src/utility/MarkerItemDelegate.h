#pragma once

#include <QStyledItemDelegate>

/// Paints one marker row:  | colour strip | thumbnail | Name/In/Out | comment box |
class MarkerItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit MarkerItemDelegate(QObject *parent = nullptr);

    void setFrameRate(double fps);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    static QString formatTimecode(qint64 ms, double fps);

private:
    double m_fps = 30.0;
};
