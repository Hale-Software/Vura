#include "MarkerItemDelegate.h"

#include <libvura/models/marker-list-model.h>

#include <QPainter>
#include <QPainterPath>
#include <QPixmap>

namespace {
constexpr int kPad = 6;
constexpr int kStripWidth = 5;
constexpr int kThumbHeight = 96;
constexpr int kThumbWidth = kThumbHeight * 16 / 9;   // 170
constexpr int kLabelWidth = 44;                      // "Name:", "In:", "Out:"
constexpr int kMinTextWidth = 110;
constexpr int kMinCommentWidth = 80;
constexpr int kMaxCommentWidth = 140;
}

MarkerItemDelegate::MarkerItemDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

void MarkerItemDelegate::setFrameRate(double fps)
{
    m_fps = fps > 0.0 ? fps : 30.0;
}

QString MarkerItemDelegate::formatTimecode(qint64 ms, double fps)
{
    if (ms < 0)
        ms = 0;
    const qint64 totalSeconds = ms / 1000;
    const int frames = int(double(ms % 1000) * fps / 1000.0);
    const QChar zero(u'0');
    return QStringLiteral("%1:%2:%3:%4")
        .arg(totalSeconds / 3600, 2, 10, zero)
        .arg((totalSeconds / 60) % 60, 2, 10, zero)
        .arg(totalSeconds % 60, 2, 10, zero)
        .arg(frames, 2, 10, zero);
}

QSize MarkerItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &) const
{
    return {option.rect.width(), kThumbHeight + 2 * kPad};
}

void MarkerItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                               const QModelIndex &index) const
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setRenderHint(QPainter::SmoothPixmapTransform);

    const QPalette &pal = option.palette;
    const QRect r = option.rect;

    // Background: selected > hover > normal.
    QColor bg = pal.color(QPalette::Base);
    if (option.state & QStyle::State_Selected)
        bg = pal.color(QPalette::Base).lighter(160);
    else if (option.state & QStyle::State_MouseOver)
        bg = pal.color(QPalette::Base).lighter(125);
    painter->fillRect(r, bg);

    // Row separator.
    painter->setPen(pal.color(QPalette::Mid));
    painter->drawLine(r.left() + kPad, r.bottom(), r.right() - kPad, r.bottom());

    const QRect content = r.adjusted(kPad, kPad, -kPad, -kPad);

    // 1. Colour strip.
    const QColor typeColor = index.data(MarkerListModel::ColorRole).value<QColor>();
    const QRect strip(content.left(), content.top(), kStripWidth, content.height());
    painter->fillRect(strip, typeColor);

    // 2. Thumbnail (letterboxed onto black).
    const QRect thumbRect(strip.right() + 1, content.top(), kThumbWidth, content.height());
    painter->fillRect(thumbRect, Qt::black);
    const QPixmap thumb = index.data(MarkerListModel::ThumbnailRole).value<QPixmap>();
    if (!thumb.isNull()) {
        const QSize scaled = thumb.size().scaled(thumbRect.size(), Qt::KeepAspectRatio);
        QRect target(QPoint(), scaled);
        target.moveCenter(thumbRect.center());
        painter->drawPixmap(target, thumb);
    } else {
        painter->setPen(pal.color(QPalette::PlaceholderText));
        painter->drawText(thumbRect, Qt::AlignCenter, QStringLiteral("…"));
    }

    // 3. Comment box on the right (dropped when the dock is too narrow).
    const int remaining = content.right() - thumbRect.right() - kPad;
    int commentWidth = qBound(kMinCommentWidth, remaining - kLabelWidth - kMinTextWidth, kMaxCommentWidth);
    if (remaining - commentWidth - kPad < kLabelWidth + kMinTextWidth / 2)
        commentWidth = 0;

    if (commentWidth > 0) {
        const QRectF box(content.right() - commentWidth + 1, content.top(), commentWidth, content.height());
        QPainterPath path;
        path.addRoundedRect(box.adjusted(0.5, 0.5, -0.5, -0.5), 4, 4);
        painter->fillPath(path, pal.color(QPalette::Base).darker(140));
        painter->setPen(pal.color(QPalette::Mid));
        painter->drawPath(path);

        const QString comments = index.data(MarkerListModel::CommentsRole).toString();
        if (!comments.isEmpty()) {
            QFont small = option.font;
            small.setPointSizeF(small.pointSizeF() * 0.9);
            painter->setFont(small);
            painter->setPen(pal.color(QPalette::Text));
            painter->drawText(box.adjusted(6, 4, -6, -4),
                              Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, comments);
            painter->setFont(option.font);
        }
    }

    // 4. Name / In / Out column.
    const int textLeft = thumbRect.right() + kPad * 2;
    const int textRight = (commentWidth > 0 ? content.right() - commentWidth - kPad : content.right());
    const int rowHeight = content.height() / 3;

    const qint64 posMs = index.data(MarkerListModel::PositionMsRole).toLongLong();
    const QString timecode = formatTimecode(posMs, m_fps);
    const QString name = index.data(MarkerListModel::NameRole).toString();

    // Until the record has an out point, In and Out are the same (as in Premiere
    // for zero-duration markers).
    const struct { QString label; QString value; bool isTime; } rows[] = {
        {tr("Name:"), name, false},
        {tr("In:"), timecode, true},
        {tr("Out:"), timecode, true},
    };

    const QColor labelColor = pal.color(QPalette::PlaceholderText);
    const QColor nameColor = pal.color(QPalette::Text);
    const QColor timeColor = pal.color(QPalette::Link);

    for (int i = 0; i < 3; ++i) {
        const int y = content.top() + i * rowHeight;
        const QRect labelRect(textLeft, y, kLabelWidth, rowHeight);
        const QRect valueRect(labelRect.right() + kPad, y, textRight - labelRect.right() - kPad, rowHeight);

        painter->setPen(labelColor);
        painter->drawText(labelRect, Qt::AlignRight | Qt::AlignVCenter, rows[i].label);

        painter->setPen(rows[i].isTime ? timeColor : nameColor);
        const QString elided = option.fontMetrics.elidedText(rows[i].value, Qt::ElideRight, valueRect.width());
        painter->drawText(valueRect, Qt::AlignLeft | Qt::AlignVCenter, elided);
    }

    painter->restore();
}
