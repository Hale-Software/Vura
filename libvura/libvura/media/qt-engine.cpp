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

#include "qt-engine.h"

#include <QAudioOutput>
#include <QImage>
#include <QLocale>
#include <QMediaMetaData>
#include <QMediaPlayer>
#include <QVideoSink>
#include <QAudioDevice>
#include <QMediaDevices>

#include <algorithm>

namespace media {
namespace {

PlaybackState toPlaybackState(QMediaPlayer::PlaybackState state)
{
    switch (state) {
    case QMediaPlayer::PlayingState:
        return PlaybackState::Playing;
    case QMediaPlayer::PausedState:
        return PlaybackState::Paused;
    case QMediaPlayer::StoppedState:
        break;
    }
    return PlaybackState::Stopped;
}

MediaStatus toMediaStatus(QMediaPlayer::MediaStatus status)
{
    switch (status) {
    case QMediaPlayer::NoMedia:
        return MediaStatus::NoMedia;
    case QMediaPlayer::LoadingMedia:
        return MediaStatus::Loading;
    case QMediaPlayer::LoadedMedia:
        return MediaStatus::Loaded;
    case QMediaPlayer::StalledMedia:
    case QMediaPlayer::BufferingMedia:
        return MediaStatus::Buffering;
    case QMediaPlayer::BufferedMedia:
        return MediaStatus::Buffered;
    case QMediaPlayer::EndOfMedia:
        return MediaStatus::EndOfMedia;
    case QMediaPlayer::InvalidMedia:
        return MediaStatus::Invalid;
    }
    return MediaStatus::NoMedia;
}

ErrorKind toErrorKind(QMediaPlayer::Error error)
{
    switch (error) {
    case QMediaPlayer::NoError:
        return ErrorKind::None;
    case QMediaPlayer::ResourceError:
        return ErrorKind::NotFound;
    case QMediaPlayer::FormatError:
        return ErrorKind::UnsupportedFormat;
    case QMediaPlayer::NetworkError:
        return ErrorKind::NetworkFailure;
    case QMediaPlayer::AccessDeniedError:
        return ErrorKind::AccessDenied;
    }
    return ErrorKind::Internal;
}

QLatin1Char idPrefix(TrackType type)
{
    switch (type) {
    case TrackType::Video:
        return QLatin1Char('v');
    case TrackType::Audio:
        return QLatin1Char('a');
    case TrackType::Subtitle:
        return QLatin1Char('s');
    }
    return QLatin1Char('x');
}

QString makeId(TrackType type, int index)
{
    return QStringLiteral("%1%2").arg(idPrefix(type)).arg(index);
}

QVector<TrackInfo> convertTracks(const QList<QMediaMetaData> &source, TrackType type)
{
    QVector<TrackInfo> result;
    result.reserve(source.size());
    for (int i = 0; i < source.size(); ++i) {
        const QMediaMetaData &md = source.at(i);
        TrackInfo track;
        track.id = makeId(type, i);
        track.title = md.stringValue(QMediaMetaData::Title);

        const QVariant language = md.value(QMediaMetaData::Language);
        if (language.isValid()) {
            const auto code = QLocale::languageToCode(language.value<QLocale::Language>());
            if (!code.isEmpty())
                track.language = code;
        }

        const QVariant codec = md.value(QMediaMetaData::AudioCodec);
        if (codec.isValid())
            track.codec = md.stringValue(QMediaMetaData::AudioCodec);

        result.append(track);
    }
    return result;
}

} // namespace


QtEngine::QtEngine(QObject *parent)
    : Engine(parent),
      m_player(new QMediaPlayer(this)),
      m_audioOutput(new QAudioOutput(this))
{
    m_player->setAudioOutput(m_audioOutput);
    m_videoSink = m_player->videoSink();

    connect(m_videoSink, &QVideoSink::videoFrameChanged, this, &QtEngine::videoFrameChanged);

    connect(m_player, &QMediaPlayer::positionChanged, this, [this](qint64 ms) { updatePosition(ms); });
    connect(m_player, &QMediaPlayer::durationChanged, this, [this](qint64 ms) { updateDuration(ms); });
    connect(m_player, &QMediaPlayer::playbackStateChanged, this, [this](QMediaPlayer::PlaybackState s) { updatePlaybackState(toPlaybackState(s)); });
    connect(m_player, &QMediaPlayer::seekableChanged, this, [this](bool seekable) { updateSeekable(seekable); });
    connect(m_player, &QMediaPlayer::playbackRateChanged, this, [this](qreal rate) { updateRate(rate); });

    connect(m_player, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status) {
        // Track lists only become valid once the media is loaded.
        if (status == QMediaPlayer::LoadedMedia || status == QMediaPlayer::BufferedMedia)
            rebuildTrackCache();
        updateMediaStatus(toMediaStatus(status));
    });

    connect(m_player, &QMediaPlayer::tracksChanged, this, [this] {
        rebuildTrackCache();
        emit tracksChanged();
    });

    connect(m_player, &QMediaPlayer::metaDataChanged, this, &QtEngine::publishMetaData);

    connect(m_player, &QMediaPlayer::errorOccurred, this, [this](QMediaPlayer::Error error, const QString &message) {
        if (error == QMediaPlayer::NoError)
            return;
        reportError(toErrorKind(error), message.isEmpty() ? describe(toErrorKind(error)) : message);
    });

    connect(m_audioOutput, &QAudioOutput::volumeChanged, this, [this](float volume) { updateVolume(volume); });
    connect(m_audioOutput, &QAudioOutput::mutedChanged, this, [this](bool muted) { updateMuted(muted); });

    m_mediaDevices = new QMediaDevices(this);
    connect(m_mediaDevices, &QMediaDevices::audioOutputsChanged, this, [this] {
        // Following the default means following it through hotplug, not
        // pinning whatever was default at startup.
        if (m_followSystemDefaultAudioDevice) {
            m_audioOutput->setDevice(QMediaDevices::defaultAudioOutput());
        } else if (m_audioOutput->device().isNull()) {
            // The chosen device went away. Fall back rather than going silent.
            m_followSystemDefaultAudioDevice = true;
            m_audioOutput->setDevice(QMediaDevices::defaultAudioOutput());
        }
        emit audioDevicesChanged();
    });

    updateVolume(m_audioOutput->volume());
    updateMuted(m_audioOutput->isMuted());
}

QtEngine::~QtEngine()
{
    // Tear the pipeline down before the sink owned by the widget can
    // disappear underneath it.
    m_player->stop();
    m_player->setVideoSink(nullptr);
}

Capabilities QtEngine::capabilities() const
{
    Capabilities caps;
    caps.variableRate = true;
    // QMediaPlayer resamples without pitch correction, so 2x speech is
    // chipmunked. The UI greys out the rate control's higher steps rather
    // than letting the engine silently disappoint.
    caps.pitchCorrection = false;
    caps.externalSubtitles = true;
    caps.gapless = false;
    caps.hardwareDecode = true;
    caps.audioTrackSelection = true;
    caps.subtitleTrackSelection = true;
    caps.audioDeviceSelection = true;
    caps.video = true;
    caps.rateRange = {0.05, 10.0};
    return caps;
}

void QtEngine::setSource(const QUrl &url)
{
    m_tracks.clear();
    resetForNewSource(url);
    m_player->setSource(url);
    emit tracksChanged();
}

void QtEngine::play()
{
    m_player->play();
}

void QtEngine::pause()
{
    m_player->pause();
}

void QtEngine::stop()
{
    m_player->stop();
}

void QtEngine::seek(Msec ms)
{
    if (!m_player->isSeekable())
        return;
    m_player->setPosition(std::clamp<Msec>(ms, 0, std::max<Msec>(0, m_player->duration())));
}

void QtEngine::setRate(qreal rate)
{
    const auto range = capabilities().rateRange;
    m_player->setPlaybackRate(std::clamp(rate, range.first, range.second));
}

void QtEngine::setVolume(qreal linear)
{
    m_audioOutput->setVolume(float(std::clamp<qreal>(linear, 0.0, 1.0)));
}

void QtEngine::setMuted(bool muted)
{
    m_audioOutput->setMuted(muted);
}

QVector<TrackInfo> QtEngine::tracks(TrackType type) const
{
    return m_tracks.value(type);
}

QString QtEngine::activeTrack(TrackType type) const
{
    int index = -1;
    switch (type) {
    case TrackType::Video:
        index = m_player->activeVideoTrack();
        break;
    case TrackType::Audio:
        index = m_player->activeAudioTrack();
        break;
    case TrackType::Subtitle:
        index = m_player->activeSubtitleTrack();
        break;
    }
    return index < 0 ? QString() : makeId(type, index);
}

void QtEngine::selectTrack(TrackType type, const QString &id)
{
    const int index = id.isEmpty() ? -1 : indexForId(type, id);
    if (!id.isEmpty() && index < 0)
        return; // Unknown id: ignore rather than disabling the track.

    switch (type) {
    case TrackType::Video:
        m_player->setActiveVideoTrack(index);
        break;
    case TrackType::Audio:
        m_player->setActiveAudioTrack(index);
        break;
    case TrackType::Subtitle:
        m_player->setActiveSubtitleTrack(index);
        break;
    }
    emit tracksChanged();
}

bool QtEngine::loadExternalSubtitle(const QUrl &url)
{
    m_subtitleTrack = new SubtitleTrack(this);
    m_subtitleTrack->loadSrt(url.toLocalFile());
    return true;
}

bool QtEngine::attachOutput(VideoOutput *output)
{
    if (!output) {
        detachOutput();
        return true;
    }
    if (output->kind() != OutputKind::VideoSink)
        return false;

    auto *sinkOutput = dynamic_cast<VideoSinkOutput *>(output);
    if (!sinkOutput || !sinkOutput->videoSink())
        return false;

    m_player->setVideoSink(sinkOutput->videoSink());
    setOutputPointer(output);
    return true;
}

void QtEngine::detachOutput()
{
    m_player->setVideoSink(nullptr);
    if (auto *out = output())
        out->clearSurface();
    setOutputPointer(nullptr);
}

QVector<AudioDeviceInfo> QtEngine::audioDevices() const
{
    const QAudioDevice defaultDevice = QMediaDevices::defaultAudioOutput();
    const QList<QAudioDevice> outputs = QMediaDevices::audioOutputs();

    QVector<AudioDeviceInfo> result;
    result.reserve(outputs.size());
    for (const QAudioDevice &device : outputs) {
        AudioDeviceInfo info;
        info.id = QString::fromUtf8(device.id());
        info.description = device.description();
        info.isDefault = device.id() == defaultDevice.id();
        result.append(info);
    }
    return result;
}

QString QtEngine::activeAudioDevice() const
{
    if (m_followSystemDefaultAudioDevice)
        return {};
    return QString::fromUtf8(m_audioOutput->device().id());
}

void QtEngine::setAudioDevice(const QString &id)
{
    if (id.isEmpty()) {
        m_followSystemDefaultAudioDevice = true;
        m_audioOutput->setDevice(QMediaDevices::defaultAudioOutput());
        emit audioDevicesChanged();
        return;
    }

    const QByteArray wanted = id.toUtf8();
    for (const QAudioDevice &device : QMediaDevices::audioOutputs()) {
        if (device.id() != wanted)
            continue;
        m_followSystemDefaultAudioDevice = false;
        m_audioOutput->setDevice(device);
        emit audioDevicesChanged();
        return;
    }
    // Unknown id: ignore, matching selectTrack()'s behaviour.
}

void QtEngine::videoFrameChanged(const QVideoFrame &frame)
{
    if (!m_subtitleTrack)
        return;

    //if (!m_subtitlesEnabled)
    //    return;

    const qint64 ms = frame.startTime() / 1000;
    m_currentCue = m_subtitleTrack->cueAt(ms + m_subtitleOffsetMs);
    m_videoSink->setSubtitleText(m_currentCue ? m_currentCue->lines.join('\n') : QString());
}

void QtEngine::rebuildTrackCache()
{
    m_tracks[TrackType::Video] = convertTracks(m_player->videoTracks(), TrackType::Video);
    m_tracks[TrackType::Audio] = convertTracks(m_player->audioTracks(), TrackType::Audio);
    m_tracks[TrackType::Subtitle] = convertTracks(m_player->subtitleTracks(), TrackType::Subtitle);
}

void QtEngine::publishMetaData()
{
    const QMediaMetaData md = m_player->metaData();
    QVariantMap out;

    const QString title = md.stringValue(QMediaMetaData::Title);
    out[QLatin1String(meta::Title)] =
            title.isEmpty() ? m_player->source().fileName() : title;

    const QString artist = md.stringValue(QMediaMetaData::ContributingArtist);
    out[QLatin1String(meta::Artist)] =
            artist.isEmpty() ? md.stringValue(QMediaMetaData::AlbumArtist) : artist;
    out[QLatin1String(meta::Album)] = md.stringValue(QMediaMetaData::AlbumTitle);

    QVariant cover = md.value(QMediaMetaData::CoverArtImage);
    if (!cover.isValid())
        cover = md.value(QMediaMetaData::ThumbnailImage);
    if (cover.isValid())
        out[QLatin1String(meta::CoverArt)] = cover;

    updateMetaData(out);
}

int QtEngine::indexForId(TrackType type, const QString &id) const
{
    const auto &list = m_tracks[type];
    for (int i = 0; i < list.size(); ++i) {
        if (list.at(i).id == id)
            return i;
    }
    return -1;
}

} // namespace media
