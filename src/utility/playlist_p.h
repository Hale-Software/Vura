/*******************************************************************************
     Copyright (c) 2026.  by Andrew Hale <halea2196@gmail.com>

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

#include "playlist.h"
#include "playlistfileparser.h"

#include <QUrl>
#include <QMap>
#include <QMapIterator>
#include <QList>
#include <QVector>
#include <QRandomGenerator>
#include <algorithm>
#include <QDebug>

#ifdef Q_MOC_RUN
#    pragma Q_MOC_EXPAND_MACROS
#endif

QT_BEGIN_NAMESPACE

class PlaylistControl;

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

    void setShuffle(bool shuffle, QMap<int, QUrl> urlMap);

    int nextPosition(int steps) const;
    int prevPosition(int steps) const;

    QList<QUrl> playlist;
    QMap<int, QUrl> shuffledPlaylist;
    QMapIterator<int, QUrl> *i;

    int currentPos = -1;
    Playlist::PlaybackMode playbackMode = Playlist::Sequential;

    PlaylistFileParser *parser = nullptr;
    mutable Playlist::Error error;
    mutable QString errorString;

    Playlist *q_ptr = nullptr;

    bool m_isShuffle = false;
};

QT_END_NAMESPACE
