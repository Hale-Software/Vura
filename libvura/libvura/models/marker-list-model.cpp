#include "marker-list-model.h"
#include "marker-colors.h"

#include <QImage>
#include <QSet>

namespace {
constexpr QSize kThumbStoreSize(320, 180);
}

MarkerListModel::MarkerListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int MarkerListModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : int(m_markers.size());
}

QVariant MarkerListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_markers.size())
        return {};

    const VideoMarkerRecord &m = m_markers.at(index.row());

    switch (role) {
    case Qt::DisplayRole:
    case NameRole:       return m.markerName;
    case Qt::ToolTipRole:return m.comments.isEmpty() ? m.markerName : m.comments;
    case RecordRole:     return QVariant::fromValue(m);
    case IdRole:         return m.id;
    case TypeRole:       return m.markerType;
    case CommentsRole:   return m.comments;
    case FractionRole:   return m.timestampMs;
    case PositionMsRole: return positionMs(m);
    case ColorRole:      return MarkerColors::forType(m.markerType);
    case ThumbnailRole: {
        const auto it = m_thumbnails.constFind(m.id);
        if (it != m_thumbnails.cend() && it->positionMs == positionMs(m))
            return it->pixmap;
        return {};
    }
    default:
        return {};
    }
}

void MarkerListModel::setMarkers(const QList<VideoMarkerRecord> &markers)
{
    beginResetModel();
    m_markers = markers;

    // Drop cached thumbnails for markers that no longer exist.
    QSet<int> alive;
    for (const VideoMarkerRecord &m : m_markers)
        alive.insert(m.id);
    for (auto it = m_thumbnails.begin(); it != m_thumbnails.end();) {
        if (alive.contains(it.key()))
            ++it;
        else
            it = m_thumbnails.erase(it);
    }
    endResetModel();
}

void MarkerListModel::setDurationMs(qint64 ms)
{
    if (ms == m_durationMs)
        return;
    m_durationMs = ms;
    if (!m_markers.isEmpty())
        emit dataChanged(index(0), index(int(m_markers.size()) - 1));
}

qint64 MarkerListModel::positionMs(const VideoMarkerRecord &marker) const
{
    return qRound64(marker.timestampMs * double(m_durationMs));
}

void MarkerListModel::setThumbnail(int markerId, qint64 positionMs, const QImage &image)
{
    Thumb thumb;
    thumb.positionMs = positionMs;
    thumb.pixmap = QPixmap::fromImage(
        image.scaled(kThumbStoreSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    m_thumbnails.insert(markerId, thumb);

    const int row = rowForId(markerId);
    if (row >= 0)
        emit dataChanged(index(row), index(row), {ThumbnailRole});
}

bool MarkerListModel::hasThumbnail(int markerId, qint64 positionMs) const
{
    const auto it = m_thumbnails.constFind(markerId);
    return it != m_thumbnails.cend() && it->positionMs == positionMs;
}

void MarkerListModel::clearThumbnails()
{
    m_thumbnails.clear();
    if (!m_markers.isEmpty())
        emit dataChanged(index(0), index(int(m_markers.size()) - 1), {ThumbnailRole});
}

int MarkerListModel::rowForId(int id) const
{
    for (int i = 0; i < m_markers.size(); ++i)
        if (m_markers.at(i).id == id)
            return i;
    return -1;
}
