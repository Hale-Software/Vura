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

#include <QObject>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QDataStream>
#include <QString>
#include <QCoreApplication>
#include <QList>
#include <QRandomGenerator>
#include <QUrl>

QT_BEGIN_NAMESPACE

class PlaylistPrivate;
class Playlist : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Playlist::PlaybackMode playbackMode READ playbackMode WRITE setPlaybackMode NOTIFY playbackModeChanged)
    Q_PROPERTY(QUrl currentMedia READ currentMedia NOTIFY currentMediaChanged)
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)

public:
    enum PlaybackMode { CurrentItemOnce, CurrentItemInLoop, Sequential, Loop };
    Q_ENUM(PlaybackMode)
    enum Error { NoError, FormatError, FormatNotSupportedError, NetworkError, AccessDeniedError };
    Q_ENUM(Error)

    explicit Playlist(QObject *parent = nullptr);
    virtual ~Playlist();

    PlaybackMode playbackMode() const;
    void setPlaybackMode(PlaybackMode mode);

    int currentIndex() const;
    QUrl currentMedia() const;

    int nextIndex(int steps = 1) const;
    int previousIndex(int steps = 1) const;

    QUrl media(int index) const;

    int mediaCount() const;
    bool isEmpty() const;

    void addMedia(const QUrl &content);
    void addMedia(const QList<QUrl> &items);
    bool insertMedia(int index, const QUrl &content);
    bool insertMedia(int index, const QList<QUrl> &items);
    bool moveMedia(int from, int to);
    bool removeMedia(int pos);
    bool removeMedia(int start, int end);
    void clear();

    void load(const QUrl &location, const char *format = nullptr);
    void load(QIODevice *device, const char *format = nullptr);

    bool save(const QUrl &location, const char *format = nullptr) const;
    bool save(QIODevice *device, const char *format) const;

    bool savePlaylist(QString location);
    void loadPlaylist(QString location);

    Error error() const;
    QString errorString() const;

public slots:
    void shuffle();

    void next();
    void previous();

    void setCurrentIndex(int index);

signals:
    void currentIndexChanged(int index);
    void playbackModeChanged(Playlist::PlaybackMode mode);
    void currentMediaChanged(const QUrl &);

    void mediaAboutToBeInserted(int start, int end);
    void mediaInserted(int start, int end);
    void mediaAboutToBeRemoved(int start, int end);
    void mediaRemoved(int start, int end);
    void mediaChanged(int start, int end);

    void loaded();
    void loadFailed();

private:
    PlaylistPrivate *d_ptr = nullptr;
    Q_DECLARE_PRIVATE(Playlist)

};

QT_END_NAMESPACE
