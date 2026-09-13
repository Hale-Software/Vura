#include "engine.h"

#include <QtGlobal>

#include <algorithm>
#include <cmath>


namespace media {

Engine::Engine(QObject *parent)
    : QObject(parent)
{
}

Engine::~Engine() = default;

void Engine::seekRelative(Msec deltaMs)
{
    Msec target = m_position + deltaMs;
    target = std::max<Msec>(0, target);
    if (m_duration > 0)
        target = std::min(target, m_duration);
    seek(target);
}

bool Engine::loadExternalSubtitle(const QUrl &)
{
    return false;
}

void Engine::updateSource(const QUrl &url)
{
    if (m_source == url)
        return;
    m_source = url;
    emit sourceChanged(m_source);
}

void Engine::updatePosition(Msec ms)
{
    if (m_position == ms)
        return;
    m_position = ms;
    emit positionChanged(m_position);
}

void Engine::updateDuration(Msec ms)
{
    if (m_duration == ms)
        return;
    m_duration = ms;
    emit durationChanged(m_duration);
}

void Engine::updatePlaybackState(PlaybackState state)
{
    if (m_state == state)
        return;
    m_state = state;
    emit playbackStateChanged(m_state);
}

void Engine::updateMediaStatus(MediaStatus status)
{
    if (m_status == status)
        return;
    m_status = status;
    emit mediaStatusChanged(m_status);
}

void Engine::updateSeekable(bool seekable)
{
    if (m_seekable == seekable)
        return;
    m_seekable = seekable;
    emit seekableChanged(m_seekable);
}

void Engine::updateRate(qreal rate)
{
    if (qFuzzyCompare(m_rate, rate))
        return;
    m_rate = rate;
    emit rateChanged(m_rate);
}

void Engine::updateVolume(qreal linear)
{
    linear = std::clamp<qreal>(linear, 0.0, 1.0);
    if (qFuzzyCompare(m_volume + 1.0, linear + 1.0))
        return;
    m_volume = linear;
    emit volumeChanged(m_volume);
}

void Engine::updateMuted(bool muted)
{
    if (m_muted == muted)
        return;
    m_muted = muted;
    emit mutedChanged(m_muted);
}

void Engine::updateMetaData(const QVariantMap &data)
{
    if (m_metadata == data)
        return;
    m_metadata = data;
    emit metaDataChanged(m_metadata);
}

void Engine::reportError(ErrorKind kind, const QString &detail)
{
    emit errorOccurred(kind, detail);
}

void Engine::resetForNewSource(const QUrl &url)
{
    updatePosition(0);
    updateDuration(0);
    updateSeekable(false);
    updateMetaData({});
    updateSource(url);
    updateMediaStatus(url.isEmpty() ? MediaStatus::NoMedia : MediaStatus::Loading);
}

}
