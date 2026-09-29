#pragma once

#include <QAbstractListModel>
#include <QHash>
#include <QList>
#include <QPixmap>

#include "video-marker-record.h"

/// Flat list of the current file's markers plus a thumbnail cache.
///
/// NOTE: VideoMarkerRecord::timestampMs currently holds a slider *fraction*
/// (0..1), not milliseconds, so the model needs the media duration to turn
/// it into a real position.
class MarkerListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles {
        RecordRole = Qt::UserRole + 1,
        IdRole,
        TypeRole,
        NameRole,
        CommentsRole,
        FractionRole,
        PositionMsRole,
        ColorRole,
        ThumbnailRole,
    };

    explicit MarkerListModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void setMarkers(const QList<VideoMarkerRecord> &markers);
    const QList<VideoMarkerRecord> &markers() const { return m_markers; }

    void setDurationMs(qint64 ms);
    qint64 durationMs() const { return m_durationMs; }
    qint64 positionMs(const VideoMarkerRecord &marker) const;

    void setThumbnail(int markerId, qint64 positionMs, const QImage &image);
    bool hasThumbnail(int markerId, qint64 positionMs) const;
    void clearThumbnails();

private:
    int rowForId(int id) const;

    struct Thumb
    {
        qint64 positionMs = -1;
        QPixmap pixmap;
    };

    QList<VideoMarkerRecord> m_markers;
    QHash<int, Thumb> m_thumbnails;
    qint64 m_durationMs = 0;
};
