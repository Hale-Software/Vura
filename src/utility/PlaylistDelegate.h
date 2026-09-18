#pragma once

#include <QStyledItemDelegate>

// Draws a whole row in one paint(): artwork with a duration badge, title,
// status line and metadata. Much cheaper than a QWidget per row, and it gives
// full control over the rounded selection pill.
class PlaylistDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit PlaylistDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override;

private:
    static constexpr int kRowHeight = 91;
    static constexpr int kRowSpacing = 4;
    static constexpr int kThumbWidth = 96;
    static constexpr int kThumbHeight = 54;
    static constexpr int kPadding = 8;
    static constexpr int kTextGap = 12;
};
