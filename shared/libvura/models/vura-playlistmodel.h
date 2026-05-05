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

#pragma once

#include <QAbstractTableModel>
#include <QUrl>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

class VuraPlaylistModel : public QAbstractTableModel {
    enum class PlaylistType { Unknown, JSON, Binary, M3U };

    static PlaylistType identifyPlaylist(const QString &fileName) {
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

public:
    struct Track { QString name; QUrl url; QString duration; };
    QList<Track> m_media;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override { return m_media.size(); }
    int columnCount(const QModelIndex &parent = QModelIndex()) const override { return 2; } // Track Name

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void addMedia(const QUrl &url);
    bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent,
                  int destinationChild) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    void updateDuration(int row, const QString &duration);
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    void saveJson(const QString &fileName);
    void loadJson(const QString &fileName);

    void loadFile(const QString &path)
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
};

