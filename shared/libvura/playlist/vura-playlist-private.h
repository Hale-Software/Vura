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

#include <QUrl>
#include <QDebug>

#include "vura-playlist.h"
#include "vura-playlist-file-parser.h"


class PlaylistPrivate
{
    Q_DECLARE_PUBLIC(Playlist)
public:
    PlaylistPrivate();

    virtual ~PlaylistPrivate();

    void loadFailed(Playlist::Error error, const QString &errorString);

    void loadFinished();

    bool checkFormat(const char *format) const;

    void ensureParser();

    int nextPosition(int steps) const;
    int prevPosition(int steps) const;

    QList<QUrl> playlist;

    int currentPos = -1;
    Playlist::PlaybackMode playbackMode = Playlist::Sequential;

    PlaylistFileParser *parser = nullptr;
    mutable Playlist::Error error;
    mutable QString errorString;

    Playlist *q_ptr = nullptr;

};

