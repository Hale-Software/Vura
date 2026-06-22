#pragma once
#include <QStyledItemDelegate>
#include <QPainter>


class PlaylistDelegate : public QStyledItemDelegate {
public:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);

        // 1. Background Logic
        if (option.state & QStyle::State_Selected) {
            painter->fillRect(option.rect, QColor("#1E90FF"));
        } else if (option.state & QStyle::State_MouseOver) {
            painter->fillRect(option.rect, QColor("#333333"));
        }

        // 2. Custom Progress Bar (The "Modern" Touch)
        qint64 progress = index.data(Qt::UserRole).toLongLong();
        int barHeight = 4;
        QRect progressRect(option.rect.left(), option.rect.bottom() - barHeight,
                           option.rect.width() * (progress / 100.0), barHeight);
        painter->fillRect(progressRect, QColor("#00FF00"));

        // 3. Text
        painter->setPen(Qt::white);
        painter->drawText(option.rect.adjusted(20, 0, 0, 0), Qt::AlignVCenter, index.data(Qt::DisplayRole).toString());

        painter->restore();
    }

    // Crucial: Set the row height
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        return QSize(option.rect.width(), 40); // 60px height feels very "modern"
    }
};
