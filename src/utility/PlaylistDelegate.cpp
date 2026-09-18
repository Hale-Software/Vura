#include "PlaylistDelegate.h"

#include <libvura/media/playlist.h>

#include <QApplication>
#include <QFontMetrics>
#include <QPainter>
#include <QPainterPath>

namespace {

const QColor kAccent("#8b5cf6");
const QColor kTitle("#f3f1f8");
const QColor kStatus("#9b97a8");
const QColor kMeta("#74707f");
const QColor kBadgeBackground(8, 6, 16, 195);

QFont scaledFont(const QFont &base, int pixelSize, QFont::Weight weight)
{
    QFont f(base);
    f.setPixelSize(pixelSize);
    f.setWeight(weight);
    return f;
}

} // namespace

PlaylistDelegate::PlaylistDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

QSize PlaylistDelegate::sizeHint(const QStyleOptionViewItem &option,
                                 const QModelIndex &index) const
{
    Q_UNUSED(index)
    return QSize(option.rect.width(), kRowHeight);
}

void PlaylistDelegate::paint(QPainter *painter,
                             const QStyleOptionViewItem &option,
                             const QModelIndex &index) const
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setRenderHint(QPainter::SmoothPixmapTransform, true);

    const QRect row = option.rect.adjusted(0, kRowSpacing / 2, 0, -kRowSpacing / 2);
    const bool isCurrent = index.data(Playlist::IsCurrentRole).toBool();
    const bool isSelected = option.state & QStyle::State_Selected;
    const bool isHovered = option.state & QStyle::State_MouseOver;

    // Row background: the active track keeps a tinted pill even when the
    // selection moves elsewhere, matching how the mock highlights playback.
    QPainterPath pill;
    pill.addRoundedRect(row, 10, 10);
    if (isCurrent || isSelected) {
        QColor fill = kAccent;
        fill.setAlpha(isCurrent ? 46 : 30);
        painter->fillPath(pill, fill);

        QColor edge = kAccent;
        edge.setAlpha(isCurrent ? 130 : 80);
        painter->setPen(QPen(edge, 1));
        painter->drawPath(pill);
    } else if (isHovered) {
        painter->fillPath(pill, QColor(255, 255, 255, 14));
    }

    // ---- artwork -------------------------------------------------------
    const QRect thumb(row.left() + kPadding,
                      row.top() + (row.height() - kThumbHeight) / 2,
                      kThumbWidth,
                      kThumbHeight);

    painter->save();
    QPainterPath clip;
    clip.addRoundedRect(thumb, 6, 6);
    painter->setClipPath(clip);

    const QPixmap art = index.data(Playlist::ThumbnailRole).value<QPixmap>();
    if (art.isNull()) {
        painter->fillRect(thumb, QColor("#15131b"));
    } else {
        // Cover-fit: fill the box, then crop the overflow from the centre.
        const QPixmap scaled =
            art.scaled(thumb.size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        const QRect source((scaled.width() - thumb.width()) / 2,
                           (scaled.height() - thumb.height()) / 2,
                           thumb.width(),
                           thumb.height());
        painter->drawPixmap(thumb, scaled, source);
    }
    painter->restore();

    painter->setPen(QPen(QColor(255, 255, 255, 22), 1));
    painter->drawPath(clip);

    // ---- duration badge ------------------------------------------------
    const QString duration = index.data(Playlist::DurationRole).toString();
    if (!duration.isEmpty()) {
        const QFont badgeFont = scaledFont(option.font, 10, QFont::DemiBold);
        const QFontMetrics fm(badgeFont);
        const int badgeWidth = fm.horizontalAdvance(duration) + 10;
        const QRect badge(thumb.right() - 4 - badgeWidth,
                          thumb.bottom() - 4 - 16,
                          badgeWidth,
                          16);

        QPainterPath badgePath;
        badgePath.addRoundedRect(badge, 4, 4);
        painter->fillPath(badgePath, kBadgeBackground);

        painter->setFont(badgeFont);
        painter->setPen(QColor("#ffffff"));
        painter->drawText(badge, Qt::AlignCenter, duration);
    }

    // ---- text column ---------------------------------------------------
    const int textLeft = thumb.right() + kTextGap;
    const int textWidth = row.right() - kPadding - textLeft;
    if (textWidth <= 0) {
        painter->restore();
        return;
    }

    const QFont titleFont = scaledFont(option.font, 14, QFont::DemiBold);
    const QFont statusFont = scaledFont(option.font, 12, QFont::Normal);
    const QFont metaFont = scaledFont(option.font, 11, QFont::Normal);

    const QFontMetrics titleFm(titleFont);
    const QFontMetrics statusFm(statusFont);
    const QFontMetrics metaFm(metaFont);

    const int blockHeight = titleFm.height() + 4 + statusFm.height() + 3 + metaFm.height();
    int y = row.top() + (row.height() - blockHeight) / 2;

    painter->setFont(titleFont);
    painter->setPen(kTitle);
    painter->drawText(QRect(textLeft, y, textWidth, titleFm.height()),
                      Qt::AlignLeft | Qt::AlignVCenter,
                      titleFm.elidedText(index.data(Playlist::TitleRole).toString(),
                                         Qt::ElideRight, textWidth));
    y += titleFm.height() + 4;

    painter->setFont(statusFont);
    painter->setPen(isCurrent ? kAccent.lighter(118) : kStatus);
    painter->drawText(QRect(textLeft, y, textWidth, statusFm.height()),
                      Qt::AlignLeft | Qt::AlignVCenter,
                      statusFm.elidedText(index.data(Playlist::StatusRole).toString(),
                                          Qt::ElideRight, textWidth));
    y += statusFm.height() + 3;

    painter->setFont(metaFont);
    painter->setPen(kMeta);
    painter->drawText(QRect(textLeft, y, textWidth, metaFm.height()),
                      Qt::AlignLeft | Qt::AlignVCenter,
                      metaFm.elidedText(index.data(Playlist::MetaRole).toString(),
                                        Qt::ElideRight, textWidth));

    painter->restore();
}
