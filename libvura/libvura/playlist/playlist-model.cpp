/*******************************************************************************
     Copyright (c) 2026 by Andrew Hale <halea2196@gmail.com>

     This program is free software: you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation, either version 3 of the License, or
     (at your option) any later version.

     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.

     You should have received a copy of the GNU General Public License
     along with this program.  If not, see <http://www.gnu.org/licenses/>.

 ******************************************************************************/

#include "playlist-model.h"


int PlaylistModel::rowCount(const QModelIndex &parent) const
{
    return m_items.count();
}

QVariant PlaylistModel::data(const QModelIndex &index, const int role) const
{
    if (!index.isValid()) return QVariant();
    const auto &item = m_items[index.row()];

    if (role == Qt::DisplayRole) return item.title;
    if (role == Qt::UserRole) return item.progress; // Used by delegate
    return QVariant();
}

void PlaylistModel::addItem(const PlaylistItem &item)
{
    const int newIndex = m_items.size();
    beginInsertRows(QModelIndex(), m_items.count(), m_items.count());
    m_items.append(item);
    m_playOrder.append(newIndex);
    endInsertRows();
}

void PlaylistModel::removeItem(const int row)
{
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

void PlaylistModel::setShuffle(const bool enable)
{
    m_isShuffle = enable;
    if (m_isShuffle) {
        std::shuffle(m_playOrder.begin(), m_playOrder.end(), std::random_device());
    } else {
        std::iota(m_playOrder.begin(), m_playOrder.end(), 0); // Reset to 0, 1, 2...
    }
}

void PlaylistModel::resetPlayOrder()
{
    m_playOrder.clear();
    for (int i = 0; i < m_items.size(); ++i) {
        m_playOrder.append(i);
    }

    // If shuffle is active, re-shuffle the fresh list
    if (m_isShuffle) {
        std::shuffle(m_playOrder.begin(), m_playOrder.end(), std::random_device());
    }
}

int PlaylistModel::getNextIndex() const
{
    const int next = m_currentIndex + 1;
    if (next >= m_playOrder.size()) {
        return m_isLoop ? 0 : -1; // Return -1 if end of playlist
    }
    return m_playOrder[next];
}

int PlaylistModel::getPreviousIndex() const
{
    const int prev = m_currentIndex - 1;
    if (prev < 0) {
        return m_playOrder[0];
    }
    return m_playOrder[prev];
}

void PlaylistModel::clear()
{
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

PlaylistItem PlaylistModel::getItemAt(const int row) const
{
    // Basic bounds checking is essential
    if (row < 0 || row >= m_items.size()) {
        return PlaylistItem(); // Return an empty/default object
    }
    return m_items.at(row);
}

QString PlaylistModel::currentURL(const int row) const
{
    if (row < 0 || row >= m_items.size()) {
        return ""; // Return an empty/default object
    }
    PlaylistItem item = m_items.at(row);
    return item.filePath;
}
