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

#include "playlist_p.h"

QT_BEGIN_NAMESPACE


PlaylistPrivate::PlaylistPrivate() : error(Playlist::NoError) { }

PlaylistPrivate::~PlaylistPrivate()
{
    delete parser;
}

void PlaylistPrivate::loadFailed(Playlist::Error error, const QString &errorString)
{
    this->error = error;
    this->errorString = errorString;

    emit q_ptr->loadFailed();
    qCritical() << errorString;
}

void PlaylistPrivate::loadFinished()
{
    q_ptr->addMedia(parser->playlist);

    emit q_ptr->loaded();
}

bool PlaylistPrivate::checkFormat(const char *format) const
{
    QLatin1String f(format);
    PlaylistFileParser::FileType type = format ? PlaylistFileParser::UNKNOWN : PlaylistFileParser::M3U8;
    if (format) {
        if (f == QLatin1String("m3u") || f == QLatin1String("text/uri-list") || f == QLatin1String("audio/x-mpegurl") || f == QLatin1String("audio/mpegurl"))
            type = PlaylistFileParser::M3U;
        else if (f == QLatin1String("m3u8") || f == QLatin1String("application/x-mpegURL") || f == QLatin1String("application/vnd.apple.mpegurl"))
            type = PlaylistFileParser::M3U8;
    }

    if (type == PlaylistFileParser::UNKNOWN || type == PlaylistFileParser::PLS) {
        error = Playlist::FormatNotSupportedError;
        errorString = Playlist::tr("This file format is not supported.");
        qCritical() << errorString;
        return false;
    }
    return true;
}

void PlaylistPrivate::ensureParser()
{
    if (parser)
        return;

    parser = new PlaylistFileParser(q_ptr);
    QObject::connect(parser, &PlaylistFileParser::finished, q_ptr, [this]() { loadFinished(); });
    QObject::connect(parser, &PlaylistFileParser::error, q_ptr, [this](Playlist::Error err, const QString &errorMsg) {
        loadFailed(err, errorMsg);
    });
}

void PlaylistPrivate::setShuffle(bool shuffle, QMap<int, QUrl> urlMap)
{
    if (shuffle) {
        m_isShuffle = true;
    } else {
        //playlist = b_playlist;
        m_isShuffle = false;
    }
}

QT_END_NAMESPACE
