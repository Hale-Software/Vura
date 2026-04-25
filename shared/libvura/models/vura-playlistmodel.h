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

class VuraPlaylistModel : public QAbstractTableModel {
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
};

