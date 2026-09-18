#pragma once

#include "types.h"

#include <QAbstractListModel>
#include <QUrl>
#include <QVector>

/// One queue entry. Duration and title start empty and are filled in as the
/// engine reports them, so the list improves as items are played.
struct PlaylistItem {
    QUrl url;
    QString title;
    media::Msec duration = -1;

    QString displayTitle() const;
};

/// Qt 6 deleted QMediaPlaylist, so the queue is ours to write.
///
/// Backing it with a QAbstractListModel rather than a QVector is worth the
/// small extra effort: views, drag-reorder and selection all come free, and
/// the UI never needs a manual "refresh the list" path.
class Playlist : public QAbstractListModel
{
    Q_OBJECT

public:
    enum RepeatMode {
        RepeatNone,
        RepeatOne,
        RepeatAll,
    };
    Q_ENUM(RepeatMode)

    enum Roles {
        UrlRole = Qt::UserRole + 1,
        TitleRole,
        StatusRole,
        MetaRole,
        DurationRole,
        ThumbnailRole,
        IsCurrentRole,
    };

    explicit Playlist(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool removeRows(int row, int count, const QModelIndex &parent = {}) override;

    void append(const QList<QUrl> &urls);
    void insertNext(const QList<QUrl> &urls);
    void clear();
    void move(int from, int to);

    int count() const { return int(m_items.size()); }
    bool isEmpty() const { return m_items.isEmpty(); }
    PlaylistItem itemAt(int index) const;

    int currentIndex() const { return m_currentIndex; }
    void setCurrentIndex(int index);
    PlaylistItem currentItem() const { return itemAt(m_currentIndex); }

    /// Index of the next/previous item, or -1 when the queue is exhausted.
    /// `manual` distinguishes a user pressing Next from playback ending,
    /// because RepeatOne should loop on its own but not trap the Next button.
    int nextIndex(bool manual = false) const;
    int previousIndex() const;

    RepeatMode repeatMode() const { return m_repeat; }
    void setRepeatMode(RepeatMode mode);

    bool isShuffled() const { return m_shuffle; }
    void setShuffled(bool shuffle);

    /// Fills in metadata discovered during playback.
    void updateItemInfo(int index, const QString &title, media::Msec duration);

signals:
    void currentIndexChanged(int index);
    void repeatModeChanged(Playlist::RepeatMode mode);
    void shuffleChanged(bool shuffled);

private:
    void rebuildShuffleOrder();
    int shufflePositionOf(int index) const;

    QVector<PlaylistItem> m_items;
    int m_currentIndex = -1;
    RepeatMode m_repeat = RepeatNone;
    bool m_shuffle = false;

    /// A permutation, not a random pick per advance. Picking randomly each
    /// time replays the same track back to back and never guarantees
    /// coverage, which users read as a broken shuffle.
    QVector<int> m_shuffleOrder;
};
