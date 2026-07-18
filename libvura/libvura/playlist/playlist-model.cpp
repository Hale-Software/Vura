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
    if (role == Qt::UserRole) return item.progress;
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
    m_items.removeAt(row);
    m_playOrder.removeOne(row);

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
        std::iota(m_playOrder.begin(), m_playOrder.end(), 0);
    }
}

void PlaylistModel::resetPlayOrder()
{
    m_playOrder.clear();
    for (int i = 0; i < m_items.size(); ++i) {
        m_playOrder.append(i);
    }

    if (m_isShuffle) {
        std::shuffle(m_playOrder.begin(), m_playOrder.end(), std::random_device());
    }
}

int PlaylistModel::getNextIndex() const
{
    const int next = m_currentIndex + 1;
    if (next >= m_playOrder.size()) {
        return m_isLoop ? 0 : -1;
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
    if (m_items.isEmpty()) return;

    beginRemoveRows(QModelIndex(), 0, m_items.size() - 1);
    m_items.clear();
    m_playOrder.clear();
    m_currentIndex = -1;
    endRemoveRows();

    emit requestStopPlayback();
}

PlaylistItem PlaylistModel::getItemAt(const int row) const
{
    if (row < 0 || row >= m_items.size()) {
        return PlaylistItem();
    }
    return m_items.at(row);
}

QString PlaylistModel::currentURL(const int row) const
{
    if (row < 0 || row >= m_items.size()) {
        return "";
    }
    PlaylistItem item = m_items.at(row);
    return item.filePath;
}

Qt::ItemFlags PlaylistModel::flags(const QModelIndex &index) const
{
    const Qt::ItemFlags defaultFlags = QAbstractListModel::flags(index);

    if (index.isValid()) {
        return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | defaultFlags;
    } else {
        return Qt::ItemIsDropEnabled | defaultFlags;
    }
}

Qt::DropActions PlaylistModel::supportedDropActions() const
{
    return Qt::MoveAction;
}

bool PlaylistModel::moveRows(const QModelIndex &sourceParent, const int sourceRow, const int count, const QModelIndex &destinationParent, const int destinationChild)
{
    if (sourceRow == destinationChild || sourceRow == destinationChild - 1 || count != 1) {
        return false;
    }

    beginMoveRows(sourceParent, sourceRow, sourceRow + count - 1, destinationParent, destinationChild);

    const auto itemToMove = m_items.takeAt(sourceRow);
    int insertIndex = destinationChild;
    if (sourceRow < destinationChild) {
        insertIndex--;
    }

    m_items.insert(insertIndex, itemToMove);
    endMoveRows();

    return true;
}
