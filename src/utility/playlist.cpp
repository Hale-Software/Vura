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

#include "playlist.h"
#include "playlist_p.h"
#include "playlistfileparser.h"


QT_BEGIN_NAMESPACE

class QM3uPlaylistWriter
{
public:
    QM3uPlaylistWriter(QIODevice *device) : m_device(device), m_textStream(new QTextStream(m_device)) {}

    ~QM3uPlaylistWriter() { delete m_textStream; }

    bool writeItem(const QUrl &item)
    {
        *m_textStream << item.toString() << Qt::endl;
        return true;
    }

private:
    QIODevice *m_device;
    QTextStream *m_textStream;

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int PlaylistPrivate::nextPosition(int steps) const
{
    if (playlist.count() == 0)
        return -1;

    if (m_isShuffle) {

    } else {
        int next = currentPos + steps;

        switch (playbackMode) {
            case Playlist::CurrentItemOnce:
                return steps != 0 ? -1 : currentPos;
            case Playlist::CurrentItemInLoop:
                return currentPos;
            case Playlist::Sequential:
                if (next >= playlist.size())
                    next = 0;
                break;
            case Playlist::Loop:
                next %= playlist.count();
                break;
        }

        return next;
    }

    return -1;
}

int PlaylistPrivate::prevPosition(int steps) const
{
    if (playlist.count() == 0)
        return -1;

    if (m_isShuffle) {

    } else {
        int next = currentPos;
        if (next < 0)
            next = playlist.size();
        next -= steps;

        switch (playbackMode) {
            case Playlist::CurrentItemOnce:
                return steps != 0 ? -1 : currentPos;
            case Playlist::CurrentItemInLoop:
                return currentPos;
            case Playlist::Sequential:
                if (next < 0)
                    next = playlist.size() - 1;
                break;
            case Playlist::Loop:
                next %= playlist.size();
                if (next < 0)
                    next += playlist.size();
                break;
        }

        return next;
    }

    return -1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*!
    \class Playlist
    \inmodule QtMultimedia
    \ingroup multimedia
    \ingroup multimedia_playback


    \brief The Playlist class provides a list of media content to play.

    Playlist is intended to be used with other media objects,
    like QMediaPlayer.

    Playlist allows to access the service intrinsic playlist functionality
    if available, otherwise it provides the local memory playlist implementation.

    \snippet multimedia-snippets/media.cpp Movie playlist

    Depending on playlist source implementation, most of the playlist mutating
    operations can be asynchronous.

    QMediaPlayList currently supports M3U playlists (file extension .m3u and .m3u8).

    \sa QUrl
*/

/*!
    \enum Playlist::PlaybackMode

    The Playlist::PlaybackMode describes the order items in playlist are played.

    \value CurrentItemOnce    The current item is played only once.

    \value CurrentItemInLoop  The current item is played repeatedly in a loop.

    \value Sequential         Playback starts from the current and moves through each successive
   item until the last is reached and then stops. The next item is a null item when the last one is
   currently playing.

    \value Loop               Playback restarts at the first item after the last has finished
   playing.

    \value Random             Play items in random order.
*/

/*!
  Create a new playlist object with the given \a parent.
*/

Playlist::Playlist(QObject *parent) : QObject(parent), d_ptr(new PlaylistPrivate)
{
    Q_D(Playlist);

    d->q_ptr = this;
}

Playlist::~Playlist()
{
    delete d_ptr;
}

Playlist::PlaybackMode Playlist::playbackMode() const
{
    return d_func()->playbackMode;
}

void Playlist::setPlaybackMode(Playlist::PlaybackMode mode)
{
    Q_D(Playlist);

    if (mode == d->playbackMode)
        return;

    d->playbackMode = mode;

    emit playbackModeChanged(mode);
}

int Playlist::currentIndex() const
{
    return d_func()->currentPos;
}

QUrl Playlist::currentMedia() const
{
    Q_D(const Playlist);
    if (d->currentPos < 0 || d->currentPos >= d->playlist.size())
        return QUrl();
    return d_func()->playlist.at(d_func()->currentPos);
}

int Playlist::nextIndex(int steps) const
{
    return d_func()->nextPosition(steps);
}

int Playlist::previousIndex(int steps) const
{
    return d_func()->prevPosition(steps);
}

int Playlist::mediaCount() const
{
    return d_func()->playlist.count();
}

bool Playlist::isEmpty() const
{
    return mediaCount() == 0;
}

QUrl Playlist::media(int index) const
{
    Q_D(const Playlist);
    if (index < 0 || index >= d->playlist.size())
        return QUrl();
    return d->playlist.at(index);
}

void Playlist::addMedia(const QUrl &content)
{
    Q_D(Playlist);
    int pos = d->playlist.size();
    emit mediaAboutToBeInserted(pos, pos);
    d->playlist.append(content);
    emit mediaInserted(pos, pos);
}

void Playlist::addMedia(const QList<QUrl> &items)
{
    if (!items.size())
        return;

    Q_D(Playlist);
    int first = d->playlist.size();
    int last = first + items.size() - 1;
    emit mediaAboutToBeInserted(first, last);
    d_func()->playlist.append(items);
    emit mediaInserted(first, last);
}

bool Playlist::insertMedia(int pos, const QUrl &content)
{
    Q_D(Playlist);
    pos = qBound(0, pos, d->playlist.size());
    emit mediaAboutToBeInserted(pos, pos);
    d->playlist.insert(pos, content);
    emit mediaInserted(pos, pos);
    return true;
}

bool Playlist::insertMedia(int pos, const QList<QUrl> &items)
{
    if (!items.size())
        return true;

    Q_D(Playlist);
    pos = qBound(0, pos, d->playlist.size());
    int last = pos + items.size() - 1;
    emit mediaAboutToBeInserted(pos, last);
    auto newList = d->playlist.mid(0, pos);
    newList += items;
    newList += d->playlist.mid(pos);
    d->playlist = newList;
    emit mediaInserted(pos, last);
    return true;
}

bool Playlist::moveMedia(int from, int to)
{
    Q_D(Playlist);
    if (from < 0 || from > d->playlist.count() || to < 0 || to > d->playlist.count())
        return false;

    d->playlist.move(from, to);
    emit mediaChanged(from, to);
    return true;
}

bool Playlist::removeMedia(int pos)
{
    return removeMedia(pos, pos);
}

bool Playlist::removeMedia(int start, int end)
{
    Q_D(Playlist);
    if (end < start || end < 0 || start >= d->playlist.count())
        return false;
    start = qBound(0, start, d->playlist.size() - 1);
    end = qBound(0, end, d->playlist.size() - 1);

    emit mediaAboutToBeRemoved(start, end);
    d->playlist.remove(start, end - start + 1);
    emit mediaRemoved(start, end);
    return true;
}

void Playlist::clear()
{
    Q_D(Playlist);
    int size = d->playlist.size();
    emit mediaAboutToBeRemoved(0, size - 1);
    d->playlist.clear();
    emit mediaRemoved(0, size - 1);
}

void Playlist::load(const QUrl &location, const char *format)
{
    Q_D(Playlist);

    d->error = NoError;
    d->errorString.clear();

    d->ensureParser();
    d->parser->start(location, QString::fromUtf8(format));
}

void Playlist::load(QIODevice *device, const char *format)
{
    Q_D(Playlist);

    d->error = NoError;
    d->errorString.clear();

    d->ensureParser();
    d->parser->start(device, QString::fromUtf8(format));
}

bool Playlist::save(const QUrl &location, const char *format) const
{
    Q_D(const Playlist);

    d->error = NoError;
    d->errorString.clear();

    if (!d->checkFormat(format))
        return false;

    QFile file(location.toLocalFile());

    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        d->error = AccessDeniedError;
        d->errorString = tr("The file could not be accessed.");
        return false;
    }

    return save(&file, format);
}

bool Playlist::save(QIODevice *device, const char *format) const
{
    Q_D(const Playlist);

    d->error = NoError;
    d->errorString.clear();

    if (!d->checkFormat(format))
        return false;

    QM3uPlaylistWriter writer(device);
    for (const auto &entry : d->playlist)
        writer.writeItem(entry);
    return true;
}

bool Playlist::savePlaylist(QString location)
{
    Q_D(const Playlist);

    QJsonArray j_playlist;
    for (QUrl item : d->playlist)
    {
        QString itemString = item.toString();
        j_playlist.append(itemString);
    }

    QJsonObject rootObj;
    rootObj["playlist"] = j_playlist;

    QJsonDocument root(rootObj);
    QFile saveFile(location);
    if (saveFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        saveFile.write(root.toJson(QJsonDocument::Indented));
        saveFile.close();
    }
    return true;
}

void Playlist::loadPlaylist(QString location)
{
    QList<QUrl> playlist;

    QFile loadFile(location);
    if (!loadFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Failed to open playlist file: " << loadFile.errorString();
        return;
    }

    QByteArray jsonData = loadFile.readAll();
    loadFile.close();

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &error);

    if (doc.isNull())
    {
        qDebug() << "Failed to create JSON document from data: " << error.errorString();
        return;
    }

    if (doc.isObject())
    {
        QJsonObject obj = doc.object();

        QJsonArray playlistArray = obj.value("playlist").toArray();
        for (QJsonValue item : playlistArray)
        {
            QUrl url(item.toString());
            playlist.append(url);
        }
        addMedia(playlist);
    }
}

Playlist::Error Playlist::error() const
{
    return d_func()->error;
}

QString Playlist::errorString() const
{
    return d_func()->errorString;
}

void Playlist::shuffle()
{
    Q_D(Playlist);
    QMap<int, QUrl> urlMap;

    if (d->m_isShuffle) {
        d->setShuffle(false, urlMap);
    } else {
        int pos = 0;
        for (QUrl item : d->playlist) {
            urlMap.insert(pos, item);
            pos++;
        }
        d->setShuffle(true, urlMap);
    }
}

void Playlist::next()
{
    Q_D(Playlist);
    d->currentPos = d->nextPosition(1);

    emit currentIndexChanged(d->currentPos);
    emit currentMediaChanged(currentMedia());
}

void Playlist::previous()
{
    Q_D(Playlist);
    d->currentPos = d->prevPosition(1);

    emit currentIndexChanged(d->currentPos);
    emit currentMediaChanged(currentMedia());
}

void Playlist::setCurrentIndex(int playlistPosition)
{
    Q_D(Playlist);
    if (playlistPosition < 0 || playlistPosition >= d->playlist.size())
        playlistPosition = -1;
    d->currentPos = playlistPosition;

    emit currentIndexChanged(d->currentPos);
    emit currentMediaChanged(currentMedia());
}

QT_END_NAMESPACE
