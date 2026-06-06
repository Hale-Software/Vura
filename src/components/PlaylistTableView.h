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

#include <QTableView>
#include <QDragEnterEvent>
#include <QMimeData>

#include <models/vura-playlistmodel.h>

class PlaylistTableView : public QTableView {
public:
    explicit PlaylistTableView(QWidget *parent = nullptr) : QTableView(parent) {
        setAcceptDrops(true); // Mandatory for receiving drops
        setDragDropMode(QAbstractItemView::DropOnly); // Or DragDrop to move rows
    }

protected:
    void dragEnterEvent(QDragEnterEvent *event) override {
        if (event->mimeData()->hasUrls()) { // Check if data contains file paths
            event->acceptProposedAction();
        }
    }

    void dropEvent(QDropEvent *event) override {
        const QMimeData *mimeData = event->mimeData();
        if (mimeData->hasUrls()) {
            auto *myModel = static_cast<VuraPlaylistModel*>(model());
            for (const QUrl &url : mimeData->urls()) {
                myModel->addMedia(url); // Custom method from previous step
            }
            event->acceptProposedAction();
        }
    }
};
