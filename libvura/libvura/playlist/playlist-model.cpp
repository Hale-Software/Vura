/*******************************************************************************
     Copyright (c) 2026. by Andrew Hale <halea2196@gmail.com>

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


PlaylistModel::PlaylistModel(QObject *parent) : QAbstractTableModel(parent), m_playlist(std::make_unique<Playlist>())
{
    connect(m_playlist.get(), &Playlist::mediaAboutToBeInserted, this, &PlaylistModel::beginInsertItems);
    connect(m_playlist.get(), &Playlist::mediaInserted, this, &PlaylistModel::endInsertItems);
    connect(m_playlist.get(), &Playlist::mediaAboutToBeRemoved, this, &PlaylistModel::beginRemoveItems);
    connect(m_playlist.get(), &Playlist::mediaRemoved, this, &PlaylistModel::endRemoveItems);
    connect(m_playlist.get(), &Playlist::mediaChanged, this, &PlaylistModel::changeItems);
}

PlaylistModel::~PlaylistModel() = default;

int PlaylistModel::rowCount(const QModelIndex &parent) const
{
    return m_playlist && !parent.isValid() ? m_playlist->mediaCount() : 0;
}

int PlaylistModel::columnCount(const QModelIndex &parent) const
{
    return !parent.isValid() ? ColumnCount : 0;
}

QModelIndex PlaylistModel::index(const int row, const int column, const QModelIndex &parent) const
{
    return m_playlist && !parent.isValid() && row >= 0 && row < m_playlist->mediaCount()
                    && column >= 0 && column < ColumnCount
            ? createIndex(row, column)
            : QModelIndex();
}

QModelIndex PlaylistModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child);

    return {};
}

QVariant PlaylistModel::data(const QModelIndex &index, const int role) const
{
    if (index.isValid() && role == Qt::DisplayRole) {
        const Track &track = m_media[index.row()];

        if (index.column() == 0)
            return track.name;

        if (index.column() == 1)
            return track.duration;

        QVariant value = m_data[index];
        if (!value.isValid() && index.column() == Title) {
            const QUrl location = m_playlist->media(index.row());
            return QFileInfo(location.path()).fileName();
        }

        return value;
    }
    return {};
}

Playlist *PlaylistModel::playlist() const
{
    return m_playlist.get();
}

bool PlaylistModel::setData(const QModelIndex &index, const QVariant &value, const int role)
{
    Q_UNUSED(role);
    m_data[index] = value;
    emit dataChanged(index, index);
    return true;
}

void PlaylistModel::beginInsertItems(const int start, const int end)
{
    m_data.clear();
    beginInsertRows(QModelIndex(), start, end);
}

void PlaylistModel::endInsertItems()
{
    endInsertRows();
}

void PlaylistModel::beginRemoveItems(const int start, const int end)
{
    m_data.clear();
    beginRemoveRows(QModelIndex(), start, end);
}

void PlaylistModel::endRemoveItems()
{
    endInsertRows();
}

void PlaylistModel::changeItems(const int start, const int end)
{
    m_data.clear();
    emit dataChanged(index(start, 0), index(end, ColumnCount));
}

Qt::ItemFlags PlaylistModel::flags(const QModelIndex &index) const {
    Qt::ItemFlags defaultFlags = QAbstractTableModel::flags(index);
    if (index.isValid())
        return defaultFlags | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled; // For specific items
    return defaultFlags | Qt::ItemIsDropEnabled; // For empty space in the table
}

QVariant PlaylistModel::headerData(const int section, const Qt::Orientation orientation, const int role) const {
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
            case 0:
                return QString(tr("Title"));

            case 1:
                return QString(tr("Duration"));

            default:
                return QVariant();
        }
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}
