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

#pragma once

#include <QObject>
#include <QAbstractListModel>
#include <QList>
#include <QVariant>
#include <QDebug>

#include <random>

#include "playlist-item.h"


class PlaylistModel : public QAbstractListModel {
    Q_OBJECT
    QList<PlaylistItem> m_items;
    int m_currentIndex = 0;
    bool m_isShuffle = false;
    bool m_isLoop = false;
    QList<int> m_playOrder;

public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    void addItem(const PlaylistItem &item);
    void removeItem(int row);
    void setShuffle(bool enable);
    void resetPlayOrder();
    int getNextIndex() const;
    int getPreviousIndex() const;
    void clear();

    PlaylistItem getItemAt(int row) const;
    QString currentURL(int row) const;

    Qt::ItemFlags flags(const QModelIndex &index) const override;
    Qt::DropActions supportedDropActions() const override;
    bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild) override;

signals:
    void requestStopPlayback();

};
