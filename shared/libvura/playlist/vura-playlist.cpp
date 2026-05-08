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

#include "vura-playlist.h"


class M3uPlaylistWriter
{
public:
    // ReSharper disable once CppNonExplicitConvertingConstructor
    M3uPlaylistWriter(QIODevice *device) : m_device(device), m_textStream(new QTextStream(m_device)) {}
    ~M3uPlaylistWriter() { delete m_textStream; }

    bool writeItem(const QUrl &item) const
    {
        *m_textStream << item.toString() << Qt::endl;
        return true;
    }

private:
    QIODevice *m_device;
    QTextStream *m_textStream;

};

int PlaylistPrivate::nextPosition(const int steps) const
{
    if (playlist.count() == 0)
        return -1;

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
    default:
            break;
    }

    return next;
}

int PlaylistPrivate::prevPosition(const int steps) const
{
    if (playlist.count() == 0)
        return -1;

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
    default:
            break;
    }

    return next;
}

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

/*!
  Destroys the playlist.
  */

Playlist::~Playlist()
{
    delete d_ptr;
}

/*!
  \property Playlist::playbackMode

  This property defines the order that items in the playlist are played.

  \sa Playlist::PlaybackMode
*/

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

/*!
  Returns position of the current media content in the playlist.
*/
int Playlist::currentIndex() const
{
    return d_func()->currentPos;
}

/*!
  Returns the current media content.
*/

QUrl Playlist::currentMedia() const
{
    Q_D(const Playlist);
    if (d->currentPos < 0 || d->currentPos >= d->playlist.size())
        return QUrl();
    return d_func()->playlist.at(d_func()->currentPos);
}

/*!
  Returns the index of the item, which would be current after calling next()
  \a steps times.

  Returned value depends on the size of playlist, current position
  and playback mode.

  \sa Playlist::playbackMode(), previousIndex()
*/
int Playlist::nextIndex(const int steps) const
{
    return d_func()->nextPosition(steps);
}

/*!
  Returns the index of the item, which would be current after calling previous()
  \a steps times.

  \sa Playlist::playbackMode(), nextIndex()
*/

int Playlist::previousIndex(const int steps) const
{
    return d_func()->prevPosition(steps);
}

/*!
  Returns the number of items in the playlist.

  \sa isEmpty()
  */
int Playlist::mediaCount() const
{
    return d_func()->playlist.count();
}

/*!
  Returns true if the playlist contains no items, otherwise returns false.

  \sa mediaCount()
  */
bool Playlist::isEmpty() const
{
    return mediaCount() == 0;
}

/*!
  Returns the media content at \a index in the playlist.
*/

QUrl Playlist::media(const int index) const
{
    Q_D(const Playlist);
    if (index < 0 || index >= d->playlist.size())
        return QUrl();
    return d->playlist.at(index);
}

/*!
  Append the media \a content to the playlist.

  Returns true if the operation is successful, otherwise returns false.
  */
void Playlist::addMedia(const QUrl &content)
{
    Q_D(Playlist);
    const int pos = d->playlist.size();
    emit mediaAboutToBeInserted(pos, pos);
    d->playlist.append(content);
    emit mediaInserted(pos, pos);
}

/*!
  Append multiple media content \a items to the playlist.

  Returns true if the operation is successful, otherwise returns false.
  */
void Playlist::addMedia(const QList<QUrl> &items)
{
    if (!items.size())
        return;

    Q_D(Playlist);
    const int first = d->playlist.size();
    const int last = first + items.size() - 1;
    emit mediaAboutToBeInserted(first, last);
    d_func()->playlist.append(items);
    emit mediaInserted(first, last);
}

/*!
  Insert the media \a content to the playlist at position \a index.

  Returns true if the operation is successful, otherwise returns false.
*/
bool Playlist::insertMedia(int index, const QUrl &content)
{
    Q_D(Playlist);
    index = qBound(0, index, d->playlist.size());
    emit mediaAboutToBeInserted(index, index);
    d->playlist.insert(index, content);
    emit mediaInserted(index, index);
    return true;
}

/*!
  Insert multiple media content \a items to the playlist at position \a index.

  Returns true if the operation is successful, otherwise returns false.
*/

bool Playlist::insertMedia(int index, const QList<QUrl> &items)
{
    if (!items.size())
        return true;

    Q_D(Playlist);
    index = qBound(0, index, d->playlist.size());
    const int last = index + items.size() - 1;
    emit mediaAboutToBeInserted(index, last);
    auto newList = d->playlist.mid(0, index);
    newList += items;
    newList += d->playlist.mid(index);
    d->playlist = newList;
    emit mediaInserted(index, last);
    return true;
}

/*!
  Move the item from position \a from to position \a to.

  Returns true if the operation is successful, otherwise false.

  \since 5.7
*/
bool Playlist::moveMedia(const int from, const int to)
{
    Q_D(Playlist);
    if (from < 0 || from > d->playlist.count() || to < 0 || to > d->playlist.count())
        return false;

    d->playlist.move(from, to);
    emit mediaChanged(from, to);
    return true;
}

/*!
  Remove the item from the playlist at position \a pos.

  Returns true if the operation is successful, otherwise return false.
  */
bool Playlist::removeMedia(const int pos)
{
    return removeMedia(pos, pos);
}

/*!
  Remove items in the playlist from \a start to \a end inclusive.

  Returns true if the operation is successful, otherwise return false.
  */
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

/*!
  Remove all the items from the playlist.

  Returns true if the operation is successful, otherwise return false.
  */
void Playlist::clear()
{
    Q_D(Playlist);
    const int size = d->playlist.size();
    emit mediaAboutToBeRemoved(0, size - 1);
    d->playlist.clear();
    emit mediaRemoved(0, size - 1);
}

/*!
  Load playlist from \a location. If \a format is specified, it is used,
  otherwise format is guessed from location name and data.

  New items are appended to playlist.

  Playlist::loaded() signal is emitted if playlist was loaded successfully,
  otherwise the playlist emits loadFailed().
*/

void Playlist::load(const QUrl &location, const char *format)
{
    Q_D(Playlist);

    d->error = NoError;
    d->errorString.clear();

    d->ensureParser();
    d->parser->start(location, QString::fromUtf8(format));
}

/*!
  Load playlist from QIODevice \a device. If \a format is specified, it is used,
  otherwise format is guessed from device data.

  New items are appended to playlist.

  Playlist::loaded() signal is emitted if playlist was loaded successfully,
  otherwise the playlist emits loadFailed().
*/
void Playlist::load(QIODevice *device, const char *format)
{
    Q_D(Playlist);

    d->error = NoError;
    d->errorString.clear();

    d->ensureParser();
    d->parser->start(device, QString::fromUtf8(format));
}

/*!
  Save playlist to \a location. If \a format is specified, it is used,
  otherwise format is guessed from location name.

  Returns true if playlist was saved successfully, otherwise returns false.
  */
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

/*!
  Save playlist to QIODevice \a device using format \a format.

  Returns true if playlist was saved successfully, otherwise returns false.
*/
bool Playlist::save(QIODevice *device, const char *format) const
{
    Q_D(const Playlist);

    d->error = NoError;
    d->errorString.clear();

    if (!d->checkFormat(format))
        return false;

    const M3uPlaylistWriter writer(device);
    for (const auto &entry : d->playlist)
        // ReSharper disable once CppExpressionWithoutSideEffects
        writer.writeItem(entry);

    return true;
}

/*!
    Returns the last error condition.
*/
Playlist::Error Playlist::error() const
{
    return d_func()->error;
}

/*!
    Returns the string describing the last error condition.
*/
QString Playlist::errorString() const
{
    return d_func()->errorString;
}

/*!
  Shuffle items in the playlist.
*/
void Playlist::shuffle()
{
    Q_D(Playlist);
    QList<QUrl> playlist;

    // keep the current item when shuffling
    QUrl current;
    if (d->currentPos != -1)
        current = d->playlist.takeAt(d->currentPos);

    while (!d->playlist.isEmpty())
        playlist.append(
                d->playlist.takeAt(QRandomGenerator::global()->bounded(int(d->playlist.size()))));

    if (d->currentPos != -1)
        playlist.insert(d->currentPos, current);
    d->playlist = playlist;
    emit mediaChanged(0, d->playlist.count());
}

/*!
    Advance to the next media content in playlist.
*/
void Playlist::next()
{
    Q_D(Playlist);
    d->currentPos = d->nextPosition(1);

    emit currentIndexChanged(d->currentPos);
    emit currentMediaChanged(currentMedia());
}

/*!
    Return to the previous media content in playlist.
*/
void Playlist::previous()
{
    Q_D(Playlist);
    d->currentPos = d->prevPosition(1);

    emit currentIndexChanged(d->currentPos);
    emit currentMediaChanged(currentMedia());
}

/*!
    Activate media content from playlist at position \a index.
*/

void Playlist::setCurrentIndex(int index)
{
    Q_D(Playlist);
    if (index < 0 || index >= d->playlist.size())
        index = -1;
    d->currentPos = index;

    emit currentIndexChanged(d->currentPos);
    emit currentMediaChanged(currentMedia());
}

/*!
    \fn void Playlist::mediaInserted(int start, int end)

    This signal is emitted after media has been inserted into the playlist.
    The new items are those between \a start and \a end inclusive.
 */

/*!
    \fn void Playlist::mediaRemoved(int start, int end)

    This signal is emitted after media has been removed from the playlist.
    The removed items are those between \a start and \a end inclusive.
 */

/*!
    \fn void Playlist::mediaChanged(int start, int end)

    This signal is emitted after media has been changed in the playlist
    between \a start and \a end positions inclusive.
 */

/*!
    \fn void Playlist::currentIndexChanged(int position)

    Signal emitted when playlist position changed to \a position.
*/

/*!
    \fn void Playlist::playbackModeChanged(Playlist::PlaybackMode mode)

    Signal emitted when playback mode changed to \a mode.
*/

/*!
    \fn void Playlist::mediaAboutToBeInserted(int start, int end)

    Signal emitted when items are to be inserted at \a start and ending at \a end.
*/

/*!
    \fn void Playlist::mediaAboutToBeRemoved(int start, int end)

    Signal emitted when item are to be deleted at \a start and ending at \a end.
*/

/*!
    \fn void Playlist::currentMediaChanged(const QUrl &content)

    Signal emitted when current media changes to \a content.
*/

/*!
    \property Playlist::currentIndex
    \brief Current position.
*/

/*!
    \property Playlist::currentMedia
    \brief Current media content.
*/

/*!
    \fn Playlist::loaded()

    Signal emitted when playlist finished loading.
*/

/*!
    \fn Playlist::loadFailed()

    Signal emitted if failed to load playlist.
*/

/*!
    \enum Playlist::Error

    This enum describes the Playlist error codes.

    \value NoError                 No errors.
    \value FormatError             Format error.
    \value FormatNotSupportedError Format not supported.
    \value NetworkError            Network error.
    \value AccessDeniedError       Access denied error.
*/
