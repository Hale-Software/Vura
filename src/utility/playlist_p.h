#ifndef PLAYLIST_P_H
#define PLAYLIST_P_H

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

#endif // PLAYLIST_P_H
