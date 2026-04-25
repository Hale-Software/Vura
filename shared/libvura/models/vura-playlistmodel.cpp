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

#include "vura-playlistmodel.h"


QVariant VuraPlaylistModel::data(const QModelIndex &index, const int role) const {
    if (role != Qt::DisplayRole)
        return QVariant();

    const Track &track = m_media[index.row()];

    if (index.column() == 0)
        return track.name;

    if (index.column() == 1)
        return track.duration;

    return QVariant();
}

void VuraPlaylistModel::addMedia(const QUrl &url) {
    beginInsertRows(QModelIndex(), m_media.size(), m_media.size());
    m_media.append({QFileInfo(url.toLocalFile()).fileName(), url});
    endInsertRows();
}

bool VuraPlaylistModel::moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild) {
    // Check if the move is a no-op (moving to same spot)
    if (!beginMoveRows(sourceParent, sourceRow, sourceRow + count - 1,
                       destinationParent, destinationChild)) {
        return false;
                       }

    for (int i = 0; i < count; ++i) {
        // Calculate real destination based on whether moving up or down
        int from = sourceRow + i;
        int to = destinationChild + (sourceRow > destinationChild ? i : i - 1);
        m_media.move(from, to);
    }

    endMoveRows();
    return true;
}


Qt::ItemFlags VuraPlaylistModel::flags(const QModelIndex &index) const {
    Qt::ItemFlags defaultFlags = QAbstractTableModel::flags(index);
    if (index.isValid())
        return defaultFlags | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled; // For specific items
    return defaultFlags | Qt::ItemIsDropEnabled; // For empty space in the table
}

void VuraPlaylistModel::updateDuration(const int row, const QString &duration)
{
    m_media[row].duration = duration;
    emit dataChanged(index(row, 1), index(row, 1), {Qt::DisplayRole});
}
