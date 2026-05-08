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

QVariant VuraPlaylistModel::headerData(const int section, const Qt::Orientation orientation, const int role) const {
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

void VuraPlaylistModel::saveJson(const QString &fileName) {
    QJsonArray trackArray;
    for (const Track &track : m_media) {
        QJsonObject trackObj;
        trackObj["name"] = track.name;
        trackObj["url"] = track.url.toString();
        trackObj["duration"] = track.duration;
        trackArray.append(trackObj);
    }

    QJsonDocument doc(trackArray);
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
    }
}

void VuraPlaylistModel::loadJson(const QString &fileName) {
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) return;

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonArray trackArray = doc.array();

    beginResetModel();
    m_media.clear();
    for (const QJsonValue &value : trackArray) {
        QJsonObject obj = value.toObject();
        m_media.append({obj["name"].toString(), QUrl(obj["url"].toString()), obj["duration"].toString()});
    }
    endResetModel();
}

void VuraPlaylistModel::loadFile(const QString &path)
{
    PlaylistType type = identifyPlaylist(path);

    switch(type) {
        case PlaylistType::Binary:
            //loadBinary(path);
            break;
        case PlaylistType::JSON:
            loadJson(path);
            break;
        case PlaylistType::M3U:
            //parseM3U(path); // A simple line-by-line reader
            break;
        default:
            // Trigger QMessageBox error
            break;
    }
}

VuraPlaylistModel::PlaylistType VuraPlaylistModel::identifyPlaylist(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) return PlaylistType::Unknown;

    // 1. Peek at the first few bytes (the "Signature")
    QByteArray header = file.peek(4);

    // Check for Binary Magic Number (0x504C5354)
    if (header.toHex() == "504c5354") return PlaylistType::Binary;

    // Check for M3U Header
    if (header.startsWith("#EXT")) return PlaylistType::M3U;

    // Check for JSON (Starts with '{' or '[')
    if (header.startsWith('{') || header.startsWith('[')) return PlaylistType::JSON;

    return PlaylistType::Unknown;
}


