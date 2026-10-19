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

#include "playlist.h"

#include <QFileInfo>
#include <QRandomGenerator>

#include <algorithm>
#include <numeric>


QString PlaylistItem::displayTitle() const
{
    if (!title.isEmpty())
        return title;
    if (url.isLocalFile())
        return QFileInfo(url.toLocalFile()).completeBaseName();
    const QString name = url.fileName();
    return name.isEmpty() ? url.toString() : name;
}

Playlist::Playlist(QObject *parent) : QAbstractListModel(parent) {}

int Playlist::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : static_cast<int>(m_items.size());
}

QVariant Playlist::data(const QModelIndex &index, const int role) const
{
    if (!index.isValid() || index.row() >= m_items.size())
        return {};

    const PlaylistItem &item = m_items.at(index.row());
    switch (role) {
        case Qt::DisplayRole:
            return item.displayTitle();
        case Qt::ToolTipRole:
            return item.url.toDisplayString();
        case UrlRole:
            return item.url;
        case DurationRole:
            return QVariant::fromValue(item.duration);
        case IsCurrentRole:
            return index.row() == m_currentIndex;
        default:
            return {};
    }
}

QHash<int, QByteArray> Playlist::roleNames() const
{
    auto roles = QAbstractListModel::roleNames();
    roles[UrlRole] = "url";
    roles[DurationRole] = "duration";
    roles[IsCurrentRole] = "isCurrent";
    return roles;
}

Qt::ItemFlags Playlist::flags(const QModelIndex &index) const
{
    const auto base = QAbstractListModel::flags(index);
    if (index.isValid())
        return base | Qt::ItemIsDragEnabled;
    return base | Qt::ItemIsDropEnabled;
}

bool Playlist::removeRows(const int row, const int count, const QModelIndex &parent)
{
    if (parent.isValid() || row < 0 || count <= 0 || row + count > m_items.size())
        return false;

    beginRemoveRows({}, row, row + count - 1);
    m_items.remove(row, count);
    endRemoveRows();

    rebuildShuffleOrder();

    if (m_currentIndex >= row + count) {
        m_currentIndex -= count;
        emit currentIndexChanged(m_currentIndex);
    } else if (m_currentIndex >= row) {
        // The playing item was removed. Keep the index in range but do not
        // silently start playing a different file.
        m_currentIndex = -1;
        emit currentIndexChanged(m_currentIndex);
    }
    return true;
}

void Playlist::append(const QList<QUrl> &urls)
{
    if (urls.isEmpty())
        return;

    beginInsertRows({}, static_cast<int>(m_items.size()), static_cast<int>(m_items.size() + urls.size() - 1));
    for (const QUrl &url : urls)
        m_items.append(PlaylistItem{.url = url, .title = {}, .duration = -1});
    endInsertRows();

    rebuildShuffleOrder();
}

void Playlist::insertNext(const QList<QUrl> &urls)
{
    if (urls.isEmpty())
        return;
    if (m_currentIndex < 0) {
        append(urls);
        return;
    }

    const int at = m_currentIndex + 1;
    beginInsertRows({}, at, at + static_cast<int>(urls.size()) - 1);
    for (int i = 0; i < urls.size(); ++i)
        m_items.insert(at + i, PlaylistItem{.url = urls.at(i), .title = {}, .duration = -1});
    endInsertRows();

    rebuildShuffleOrder();
}

void Playlist::clear()
{
    if (m_items.isEmpty())
        return;

    beginResetModel();
    m_items.clear();
    m_shuffleOrder.clear();
    m_currentIndex = -1;
    endResetModel();

    emit currentIndexChanged(m_currentIndex);
}

void Playlist::move(const int from, const int to)
{
    if (from == to || from < 0 || to < 0 || from >= m_items.size() || to >= m_items.size())
        return;

    // beginMoveRows uses an insertion point, which is one past `to` when
    // moving down the list.
    const int destination = to > from ? to + 1 : to;
    if (!beginMoveRows({}, from, from, {}, destination))
        return;
    m_items.move(from, to);
    endMoveRows();

    if (m_currentIndex == from)
        m_currentIndex = to;
    else if (from < m_currentIndex && to >= m_currentIndex)
        --m_currentIndex;
    else if (from > m_currentIndex && to <= m_currentIndex)
        ++m_currentIndex;

    rebuildShuffleOrder();
    emit currentIndexChanged(m_currentIndex);
}

PlaylistItem Playlist::itemAt(const int index) const
{
    if (index < 0 || index >= m_items.size())
        return {};
    return m_items.at(index);
}

void Playlist::setCurrentIndex(const int index)
{
    if (index < -1 || index >= m_items.size() || index == m_currentIndex)
        return;

    const int previous = m_currentIndex;
    m_currentIndex = index;

    if (previous >= 0 && previous < m_items.size()) {
        const QModelIndex idx = createIndex(previous, 0);
        emit dataChanged(idx, idx, {IsCurrentRole});
    }
    if (m_currentIndex >= 0) {
        const QModelIndex idx = createIndex(m_currentIndex, 0);
        emit dataChanged(idx, idx, {IsCurrentRole});
    }

    emit currentIndexChanged(m_currentIndex);
}

int Playlist::nextIndex(const bool manual) const
{
    if (m_items.isEmpty())
        return -1;
    if (m_currentIndex < 0)
        return m_shuffle && !m_shuffleOrder.isEmpty() ? m_shuffleOrder.first() : 0;

    // Repeat-one loops when a track ends but must not swallow a deliberate
    // press of Next.
    if (m_repeat == RepeatOne && !manual)
        return m_currentIndex;

    if (m_shuffle) {
        const int pos = shufflePositionOf(m_currentIndex);
        if (pos < 0)
            return -1;
        if (pos + 1 < m_shuffleOrder.size())
            return m_shuffleOrder.at(pos + 1);
        return m_repeat == RepeatAll && !m_shuffleOrder.isEmpty() ? m_shuffleOrder.first() : -1;
    }

    if (m_currentIndex + 1 < m_items.size())
        return m_currentIndex + 1;
    return m_repeat == RepeatAll ? 0 : -1;
}

int Playlist::previousIndex() const
{
    if (m_items.isEmpty())
        return -1;
    if (m_currentIndex < 0)
        return 0;

    if (m_shuffle) {
        const int pos = shufflePositionOf(m_currentIndex);
        if (pos <= 0)
            return m_repeat == RepeatAll && !m_shuffleOrder.isEmpty() ? m_shuffleOrder.last() : -1;
        return m_shuffleOrder.at(pos - 1);
    }

    if (m_currentIndex > 0)
        return m_currentIndex - 1;
    return m_repeat == RepeatAll ? static_cast<int>(m_items.size()) - 1 : -1;
}

void Playlist::setRepeatMode(const RepeatMode mode)
{
    if (m_repeat == mode)
        return;
    m_repeat = mode;
    emit repeatModeChanged(m_repeat);
}

void Playlist::setShuffled(const bool shuffle)
{
    if (m_shuffle == shuffle)
        return;
    m_shuffle = shuffle;
    rebuildShuffleOrder();
    emit shuffleChanged(m_shuffle);
}

void Playlist::updateItemInfo(const int index, const QString &title, const media::Msec duration)
{
    if (index < 0 || index >= m_items.size())
        return;

    PlaylistItem &item = m_items[index];
    bool changed = false;
    if (!title.isEmpty() && item.title != title) {
        item.title = title;
        changed = true;
    }
    if (duration > 0 && item.duration != duration) {
        item.duration = duration;
        changed = true;
    }
    if (!changed)
        return;

    const QModelIndex idx = createIndex(index, 0);
    emit dataChanged(idx, idx, {Qt::DisplayRole, DurationRole});
}

void Playlist::rebuildShuffleOrder()
{
    if (!m_shuffle) {
        m_shuffleOrder.clear();
        return;
    }

    m_shuffleOrder.resize(m_items.size());
    std::iota(m_shuffleOrder.begin(), m_shuffleOrder.end(), 0);
    std::ranges::shuffle(m_shuffleOrder, *QRandomGenerator::global());

    // Keep whatever is playing at the head so enabling shuffle mid-track
    // does not jump away from it.
    if (m_currentIndex >= 0) {
        const int pos = shufflePositionOf(m_currentIndex);
        if (pos > 0)
            std::swap(m_shuffleOrder[0], m_shuffleOrder[pos]);
    }
}

int Playlist::shufflePositionOf(const int index) const
{
    const auto it = std::ranges::find(m_shuffleOrder, index);
    return it == m_shuffleOrder.cend() ? -1 : static_cast<int>(it - m_shuffleOrder.cbegin());
}
