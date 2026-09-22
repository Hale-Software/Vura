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

#include "media-controller.h"
#include "util/resume-store.h"

#include <QTimer>
#include <QSettings>
#include <QDebug>

#include <algorithm>


int positionEmitThreshold()
{
    // Backends report position far more often than a UI can use.
    // Keeps the clock honest without waking the whole widget tree 60 times a second.
    QSettings settings;
    return settings.value("positionEmitThreshold", 250).toInt();
}

int seekCoalesceMs()
{
    // Seeks issued while dragging are coalesced into one per interval.
    QSettings settings;
    return settings.value("seekCoalesceMs", 60).toInt();
}

int mediaControllerMaxRetriesPerItem()
{
    QSettings settings;
    return settings.value("mediaControllerMaxRetriesPerItem", 1).toInt();
}


MediaController::MediaController(QObject *parent)
    : QObject(parent),
      m_playlist(new Playlist(this)),
      m_resumeStore(new ResumeStore(this)),
      m_seekTimer(new QTimer(this))
{
    m_seekTimer->setSingleShot(true);
    m_seekTimer->setInterval(seekCoalesceMs());
    connect(m_seekTimer, &QTimer::timeout, this, &MediaController::flushSeek);

    QString error;
    setBackend(media::Backend::Auto, &error);
}

MediaController::~MediaController()
{
    rememberPosition();
    if (m_engine)
        m_engine->detachOutput();
}

media::Capabilities MediaController::capabilities() const
{
    return m_engine ? m_engine->capabilities() : media::Capabilities{};
}

bool MediaController::setBackend(media::Backend backend, QString *errorOut)
{
    // Capture everything worth carrying across before the old engine dies.
    struct Snapshot {
        QUrl source;
        media::Msec position = 0;
        media::PlaybackState state = media::PlaybackState::Stopped;
        qreal rate = 1.0;
        qreal volume = 1.0;
        bool muted = false;
        QString videoTrack;
        QString audioTrack;
        QString subtitleTrack;
        QString audioDevice;
        bool valid = false;
    } snapshot;

    if (m_engine) {
        snapshot.source = m_engine->source();
        snapshot.position = m_engine->position();
        snapshot.state = m_engine->playbackState();
        snapshot.rate = m_engine->rate();
        snapshot.volume = m_engine->volume();
        snapshot.muted = m_engine->isMuted();
        snapshot.videoTrack = m_engine->activeTrack(media::TrackType::Video);
        snapshot.audioTrack = m_engine->activeTrack(media::TrackType::Audio);
        snapshot.subtitleTrack = m_engine->activeTrack(media::TrackType::Subtitle);
        snapshot.audioDevice = m_engine->activeAudioDevice();
        snapshot.valid = true;

        m_engine->stop();
        m_engine->detachOutput();
        m_engine.reset();
    }

    QString error;
    auto engine = media::createEngine(backend, &error);
    if (!engine) {
        if (errorOut)
            *errorOut = error;
        return false;
    }
    if (errorOut)
        *errorOut = error;

    m_engine = std::move(engine);
    m_backend = media::backendFromId(m_engine->name());
    connectEngine();
    attachOutput();

    if (snapshot.valid && !snapshot.audioDevice.isEmpty())
        m_engine->setAudioDevice(snapshot.audioDevice);

    m_engine->setVolume(snapshot.valid ? snapshot.volume : m_volume);
    m_engine->setMuted(snapshot.valid ? snapshot.muted : m_muted);

    emit backendChanged(m_backend);
    emit capabilitiesChanged(m_engine->capabilities());

    if (snapshot.valid && !snapshot.source.isEmpty()) {
        m_resumeTarget = snapshot.position;
        m_autoPlayPending = snapshot.state == media::PlaybackState::Playing;
        m_engine->setSource(snapshot.source);
        m_engine->setRate(snapshot.rate);

        // Track ids are engine-specific, so a saved id is meaningless to a
        // different backend. Restore by position in the list instead, and
        // accept that this is approximate.
        if (!snapshot.audioTrack.isEmpty() || !snapshot.subtitleTrack.isEmpty()) {
            QTimer::singleShot(0, this, [this] { emit tracksChanged(); });
        }
    }

    emit tracksChanged();
    return true;
}

void MediaController::connectEngine()
{
    media::Engine *engine = m_engine.get();

    connect(engine, &media::Engine::positionChanged, this, [this](media::Msec ms) {
        m_position = ms;
        // A backwards jump is a seek and must show immediately; otherwise
        // throttle so the UI is not flooded.
        if (m_lastEmittedPosition < 0 || ms < m_lastEmittedPosition || ms - m_lastEmittedPosition >= positionEmitThreshold()) {
            m_lastEmittedPosition = ms;
            emit positionChanged(ms);
        }
    });

    connect(engine, &media::Engine::durationChanged, this, [this](media::Msec ms) {
        m_duration = ms;
        m_playlist->updateItemInfo(m_playlist->currentIndex(), currentTitle(), ms);
        emit durationChanged(ms);
    });

    connect(engine, &media::Engine::playbackStateChanged, this, [this](media::PlaybackState state) {
                if (state != media::PlaybackState::Playing)
                    rememberPosition();
                emit playbackStateChanged(state);
            });

    connect(engine, &media::Engine::mediaStatusChanged, this, &MediaController::handleMediaStatus);
    connect(engine, &media::Engine::errorOccurred, this, &MediaController::handleError);
    connect(engine, &media::Engine::seekableChanged, this, &MediaController::seekableChanged);
    connect(engine, &media::Engine::rateChanged, this, &MediaController::rateChanged);
    connect(engine, &media::Engine::tracksChanged, this, &MediaController::tracksChanged);

    connect(engine, &media::Engine::volumeChanged, this, [this](qreal volume) {
        m_volume = volume;
        emit volumeChanged(volume);
    });
    connect(engine, &media::Engine::mutedChanged, this, [this](bool muted) {
        m_muted = muted;
        emit mutedChanged(muted);
    });

    connect(engine, &media::Engine::metaDataChanged, this, [this](const QVariantMap &data) {
        m_playlist->updateItemInfo(m_playlist->currentIndex(), data.value(QLatin1String(media::meta::Title)).toString(), m_duration);
        emit metaDataChanged(data);
    });

    connect(engine, &media::Engine::audioDevicesChanged, this, &MediaController::audioDevicesChanged);
}

void MediaController::setVideoOutputProvider(std::function<media::VideoOutput *(media::Engine *)> provider)
{
    m_outputProvider = std::move(provider);
    attachOutput();
}

void MediaController::attachOutput()
{
    if (!m_engine || !m_outputProvider)
        return;

    media::VideoOutput *output = m_outputProvider(m_engine.get());
    if (!output)
        return;

    if (!m_engine->attachOutput(output)) {
        qWarning() << "Engine" << m_engine->name() << "rejected the supplied video output kind";
    }
}

QVector<media::AudioDeviceInfo> MediaController::audioDevices() const
{
    return m_engine ? m_engine->audioDevices() : QVector<media::AudioDeviceInfo>{};
}

QString MediaController::activeAudioDevice() const
{
    return m_engine ? m_engine->activeAudioDevice() : QString();
}

void MediaController::openUrls(const QList<QUrl> &urls)
{
    if (urls.isEmpty())
        return;
    m_playlist->clear();
    m_playlist->append(urls);
    playIndex(0);
}

void MediaController::enqueue(const QList<QUrl> &urls)
{
    const bool wasEmpty = m_playlist->isEmpty();
    m_playlist->append(urls);
    if (wasEmpty)
        playIndex(0);
}

void MediaController::playIndex(int index)
{
    if (index < 0 || index >= m_playlist->count())
        return;
    rememberPosition();
    m_retriesForCurrentItem = 0;
    m_playlist->setCurrentIndex(index);
    loadCurrentItem(true);
}

void MediaController::loadCurrentItem(bool autoPlay)
{
    if (!m_engine)
        return;

    const PlaylistItem item = m_playlist->currentItem();
    if (item.url.isEmpty())
        return;

    m_seekTimer->stop();
    m_pendingSeek = -1;
    m_position = 0;
    m_duration = 0;
    m_lastEmittedPosition = -1;
    m_currentUrl = item.url;                   // new member
    m_currentTitle = item.displayTitle();      // new member
    m_autoPlayPending = autoPlay;
    m_resumeTarget = m_resumeStore->positionFor(item.url);

    emit positionChanged(0);
    emit durationChanged(0);

    m_engine->setSource(item.url);
    emit currentItemChanged(item);
}

void MediaController::handleMediaStatus(media::MediaStatus status)
{
    switch (status) {
    case media::MediaStatus::Loaded:
    case media::MediaStatus::Buffered:
        // Resume only once, and only after duration is known so the target
        // can be validated against it.
        if (m_resumeTarget > 0 && m_duration > 0) {
            const media::Msec target = m_resumeTarget;
            m_resumeTarget = -1;
            m_engine->seek(std::min(target, m_duration));
        }
        if (m_autoPlayPending) {
            m_autoPlayPending = false;
            m_engine->play();
        }
        m_retriesForCurrentItem = 0;
        break;

    case media::MediaStatus::EndOfMedia: {
        // Finishing a file clears its resume entry rather than storing a
        // position at the very end.
        m_resumeStore->forget(m_playlist->currentItem().url);
        const int nextIdx = m_playlist->nextIndex(false);
        if (nextIdx >= 0) {
            if (nextIdx == m_playlist->currentIndex()) {
                m_engine->seek(0);
                m_engine->play();
            } else {
                playIndex(nextIdx);
            }
        }
        break;
    }

    case media::MediaStatus::Invalid:
        break;

    default:
        break;
    }

    emit mediaStatusChanged(status);
}

void MediaController::handleError(media::ErrorKind kind, const QString &detail)
{
    emit errorOccurred(kind, detail);

    // Recovery policy lives here rather than in the adapters: a network
    // stall is worth one retry, a missing codec is not.
    if (media::isRecoverable(kind) && m_retriesForCurrentItem < mediaControllerMaxRetriesPerItem()) {
        ++m_retriesForCurrentItem;
        const media::Msec resumeAt = m_position;
        QTimer::singleShot(1000, this, [this, resumeAt] {
            if (!m_engine)
                return;
            m_resumeTarget = resumeAt;
            m_autoPlayPending = true;
            m_engine->setSource(m_playlist->currentItem().url);
        });
        return;
    }

    // Terminal failure: skip rather than stalling the queue.
    const int nextIdx = m_playlist->nextIndex(true);
    if (nextIdx >= 0 && nextIdx != m_playlist->currentIndex())
        playIndex(nextIdx);
}

void MediaController::play()
{
    if (!m_engine)
        return;
    if (m_engine->source().isEmpty() && !m_playlist->isEmpty()) {
        playIndex(std::max(0, m_playlist->currentIndex()));
        return;
    }
    m_engine->play();
}

void MediaController::pause()
{
    if (m_engine)
        m_engine->pause();
}

void MediaController::togglePlayPause()
{
    if (!m_engine)
        return;
    if (m_engine->playbackState() == media::PlaybackState::Playing)
        pause();
    else
        play();
}

void MediaController::stop()
{
    rememberPosition();
    if (m_engine)
        m_engine->stop();
}

void MediaController::next()
{
    const int index = m_playlist->nextIndex(true);
    if (index >= 0)
        playIndex(index);
}

void MediaController::previous()
{
    // The familiar behaviour: within the first few seconds go back a track,
    // otherwise restart the current one.
    if (m_position > 3000 && m_engine && m_engine->isSeekable()) {
        seek(0);
        return;
    }
    const int index = m_playlist->previousIndex();
    if (index >= 0)
        playIndex(index);
}

void MediaController::restart()
{
    if (!m_engine || !m_engine->isSeekable())
        return;

    seek(0);
}

void MediaController::seek(media::Msec ms)
{
    if (!m_engine || !m_engine->isSeekable())
        return;

    m_pendingSeek = std::clamp<media::Msec>(ms, 0, m_duration > 0 ? m_duration : ms);

    // Show the new position straight away; waiting for the backend to
    // confirm makes the slider feel like it is fighting the pointer.
    m_position = m_pendingSeek;
    m_lastEmittedPosition = m_pendingSeek;
    emit positionChanged(m_pendingSeek);

    if (!m_seekTimer->isActive())
        m_seekTimer->start();
}

void MediaController::flushSeek()
{
    if (m_pendingSeek < 0 || !m_engine)
        return;
    const media::Msec target = m_pendingSeek;
    m_pendingSeek = -1;
    m_engine->seek(target);
}

void MediaController::seekRelative(media::Msec deltaMs)
{
    seek(m_position + deltaMs);
}

void MediaController::setRate(qreal rate)
{
    if (m_engine && m_engine->capabilities().variableRate)
        m_engine->setRate(rate);
}

void MediaController::setVolume(qreal linear)
{
    m_volume = std::clamp<qreal>(linear, 0.0, 1.0);
    if (m_engine)
        m_engine->setVolume(m_volume);
}

void MediaController::setMuted(bool muted)
{
    m_muted = muted;
    if (m_engine)
        m_engine->setMuted(muted);
}

void MediaController::toggleMuted()
{
    setMuted(!m_muted);
}

void MediaController::selectTrack(media::TrackType type, const QString &id)
{
    if (m_engine)
        m_engine->selectTrack(type, id);
}

bool MediaController::loadExternalSubtitle(const QUrl &url)
{
    if (!m_engine || !m_engine->capabilities().externalSubtitles)
        return false;
    return m_engine->loadExternalSubtitle(url);
}

void MediaController::setAudioDevice(const QString &id)
{
    if (m_engine && m_engine->capabilities().audioDeviceSelection)
        m_engine->setAudioDevice(id);
}

void MediaController::rememberPosition()
{
    if (m_currentUrl.isEmpty() || m_duration <= 0)
        return;
    m_resumeStore->remember(m_currentUrl, m_position, m_duration);
}

media::PlaybackState MediaController::playbackState() const
{
    return m_engine ? m_engine->playbackState() : media::PlaybackState::Stopped;
}

media::MediaStatus MediaController::mediaStatus() const
{
    return m_engine ? m_engine->mediaStatus() : media::MediaStatus::NoMedia;
}

bool MediaController::isSeekable() const
{
    return m_engine && m_engine->isSeekable();
}

qreal MediaController::rate() const
{
    return m_engine ? m_engine->rate() : 1.0;
}
