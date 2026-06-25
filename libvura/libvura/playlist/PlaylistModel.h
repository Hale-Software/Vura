#pragma once
#include <QObject>
#include <QAbstractListModel>
#include <QList>
#include <QVariant>
#include <QDebug>

#include "MediaItem.h"

class PlaylistModel : public QAbstractListModel {
    Q_OBJECT
    QList<MediaItem> m_items;
    int m_currentIndex = 0;
    bool m_isShuffle = false;
    bool m_isLoop = false;
    QList<int> m_playOrder;

public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        return m_items.count();
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
        if (!index.isValid()) return QVariant();
        const auto &item = m_items[index.row()];

        if (role == Qt::DisplayRole) return item.title;
        if (role == Qt::UserRole) return item.progress; // Used by delegate
        return QVariant();
    }

    void addItem(const MediaItem &item) {
        int newIndex = m_items.size();
        beginInsertRows(QModelIndex(), m_items.count(), m_items.count());
        m_items.append(item);
        m_playOrder.append(newIndex);
        endInsertRows();
    }

    void removeItem(int row) {
        if (row < 0 || row >= m_items.size()) return;

        beginRemoveRows(QModelIndex(), row, row);

        // 1. Remove the row from data
        m_items.removeAt(row);

        // 2. Remove the row from playOrder
        m_playOrder.removeOne(row);

        // 3. IMPORTANT: Decrement all indices in playOrder that were greater than 'row'
        for (int &idx : m_playOrder) {
            if (idx > row) idx--;
        }

        endRemoveRows();
    }

    void setShuffle(bool enable) {
        m_isShuffle = enable;
        if (m_isShuffle) {
            //std::shuffle(m_playOrder.begin(), m_playOrder.end(), std::random_device());
        } else {
            std::iota(m_playOrder.begin(), m_playOrder.end(), 0); // Reset to 0, 1, 2...
        }
    }

    void resetPlayOrder() {
        m_playOrder.clear();
        for (int i = 0; i < m_items.size(); ++i) {
            m_playOrder.append(i);
        }

        // If shuffle is active, re-shuffle the fresh list
        if (m_isShuffle) {
            //std::shuffle(m_playOrder.begin(), m_playOrder.end(), std::random_device());
        }
    }

    int getNextIndex() const {
        int next = m_currentIndex + 1;
        if (next >= m_playOrder.size()) {
            return m_isLoop ? 0 : -1; // Return -1 if end of playlist
        }
        return m_playOrder[next];
    }

    int getPreviousIndex() const
    {
        int prev = m_currentIndex - 1;
        if (prev < 0) {
            return m_playOrder[0];
        }
        return m_playOrder[prev];
    }

    void clear() {
        // 1. Notify the view that all rows are about to be removed
        if (m_items.isEmpty()) return;

        beginRemoveRows(QModelIndex(), 0, m_items.size() - 1);

        // 2. Clear the actual data
        m_items.clear();

        // 3. Clear and reset your playback state
        m_playOrder.clear();
        m_currentIndex = -1;

        // 4. Notify the view that the removal is complete
        endRemoveRows();

        // 5. Signal the engine to stop playback
        emit requestStopPlayback();
    }

    // Assuming MediaItem is your custom struct/class
    MediaItem getItemAt(int row) const {
        // Basic bounds checking is essential
        if (row < 0 || row >= m_items.size()) {
            return MediaItem(); // Return an empty/default object
        }
        return m_items.at(row);
    }

    QString currentURL(int row) const
    {
        if (row < 0 || row >= m_items.size()) {
            return ""; // Return an empty/default object
        }
        MediaItem item = m_items.at(row);
        return item.filePath;
    }

signals:
    void requestStopPlayback();

};
