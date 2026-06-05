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

#include <QObject>
#include <QMediaPlayer>
#include <QTime>

#include "../models/vura-playlistmodel.h"


class PlaylistManager : public QObject {
    Q_OBJECT

public:
    explicit PlaylistManager(QMediaPlayer *p, VuraPlaylistModel *m);

    void scanMediaDuration(int row);
    void playTrack(int index);
    void playNext();
    void playPrevious();

private:
    QMediaPlayer *player;
    VuraPlaylistModel *model;
    int currentIndex = -1;
    QMediaPlayer *durationScanner = nullptr;

};
