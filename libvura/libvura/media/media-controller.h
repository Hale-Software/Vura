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

#include "playlist.h"
#include "engine-factory.h"
#include "engine.h"

#include <QObject>
#include <QUrl>

#include <memory>

class ResumeStore;

QT_BEGIN_NAMESPACE
class QTimer;
QT_END_NAMESPACE

/// Everything the backend does not need to know about.
///
/// The playlist, resume positions, signal throttling, end-of-media
/// advancement and the engine swap itself all live here. Adapters stay
/// dumb, which is what keeps them interchangeable.
///
/// The UI talks only to this class and never to an Engine directly.
class MediaController : public QObject
{
    Q_OBJECT

public:
    explicit MediaController(QObject *parent = nullptr);
    ~MediaController() override;

    // ---- engine management -------------------------------------------------
    media::Engine *engine() const { return m_engine.get(); }
    media::Backend backend() const { return m_backend; }
    media::Capabilities capabilities() const;

    /// Swaps the backend live, carrying source, position, playback state,
    /// volume and track selection across.
    ///
    /// Worth writing even if it is never exposed in the UI, because it is
    /// the only way to find out whether the abstraction actually holds.
    /// Anything that leaks shows up here immediately.
    bool setBackend(media::Backend backend, QString *errorOut = nullptr);

    /// The engine renders into this. Ownership stays with the caller.
    void setVideoOutputProvider(std::function<media::VideoOutput *(media::Engine *)> provider);

    // ---- queue -------------------------------------------------------------
    Playlist *playlist() const { return m_playlist; }
    ResumeStore *resumeStore() const { return m_resumeStore; }

    /// Replaces the queue and starts at the first item.
    void openUrls(const QList<QUrl> &urls);
    /// Adds to the queue without disturbing what is playing.
    void enqueue(const QList<QUrl> &urls);
    void playIndex(int index);

    // ---- cached state, safe to read at any time ----------------------------
    media::Msec position() const { return m_position; }
    media::Msec duration() const { return m_duration; }
    media::PlaybackState playbackState() const;
    media::MediaStatus mediaStatus() const;
    bool isSeekable() const;
    qreal rate() const;
    qreal volume() const { return m_volume; }
    bool isMuted() const { return m_muted; }
    QString currentTitle() const;
    QVector<media::AudioDeviceInfo> audioDevices() const;
    QString activeAudioDevice() const;

public slots:
    void play();
    void pause();
    void togglePlayPause();
    void stop();
    void next();
    void previous();
    void restart();

    /// Coalesced: dragging a slider issues one seek per interval instead of
    /// one per pixel, which is the difference between smooth scrubbing and
    /// a stutter.
    void seek(media::Msec ms);
    void seekRelative(media::Msec deltaMs);

    void setRate(qreal rate);
    void setVolume(qreal linear);
    void setMuted(bool muted);
    void toggleMuted();

    void selectTrack(media::TrackType type, const QString &id);
    bool loadExternalSubtitle(const QUrl &url);

    void setAudioDevice(const QString &id);

signals:
    void positionChanged(media::Msec ms);
    void durationChanged(media::Msec ms);
    void playbackStateChanged(media::PlaybackState state);
    void mediaStatusChanged(media::MediaStatus status);
    void seekableChanged(bool seekable);
    void rateChanged(qreal rate);
    void volumeChanged(qreal linear);
    void mutedChanged(bool muted);
    void tracksChanged();
    void audioDevicesChanged();
    void metaDataChanged(const QVariantMap &data);
    void errorOccurred(media::ErrorKind kind, const QString &detail);

    void backendChanged(media::Backend backend);
    void capabilitiesChanged(const media::Capabilities &capabilities);
    void currentItemChanged(const PlaylistItem &item);

private:
    void connectEngine();
    void attachOutput();
    void loadCurrentItem(bool autoPlay);
    void handleMediaStatus(media::MediaStatus status);
    void handleError(media::ErrorKind kind, const QString &detail);
    void rememberPosition();
    void flushSeek();

    std::unique_ptr<media::Engine> m_engine;
    media::Backend m_backend = media::Backend::Auto;
    Playlist *m_playlist = nullptr;
    ResumeStore *m_resumeStore = nullptr;

    std::function<media::VideoOutput *(media::Engine *)> m_outputProvider;

    // Cached so the UI never round-trips into a backend mid-seek.
    media::Msec m_position = 0;
    media::Msec m_duration = 0;
    media::Msec m_lastEmittedPosition = -1;
    qreal m_volume = 1.0;
    bool m_muted = false;

    QTimer *m_seekTimer = nullptr;
    media::Msec m_pendingSeek = -1;

    media::Msec m_resumeTarget = -1;
    bool m_autoPlayPending = false;
    int m_retriesForCurrentItem = 0;
};
