#include "null-engine.h"

#include <QTimer>
#include <QUrlQuery>

#include <algorithm>

namespace media {
namespace {

ErrorKind errorFromKeyword(const QString &keyword)
{
    if (keyword == QLatin1String("notfound"))
        return ErrorKind::NotFound;
    if (keyword == QLatin1String("denied"))
        return ErrorKind::AccessDenied;
    if (keyword == QLatin1String("codec"))
        return ErrorKind::MissingCodec;
    if (keyword == QLatin1String("network"))
        return ErrorKind::NetworkFailure;
    if (keyword == QLatin1String("format"))
        return ErrorKind::UnsupportedFormat;
    return ErrorKind::Internal;
}

} // namespace

NullEngine::NullEngine(QObject *parent)
    : Engine(parent)
    , m_timer(new QTimer(this))
{
    m_timer->setTimerType(Qt::PreciseTimer);
    connect(m_timer, &QTimer::timeout, this, &NullEngine::onTick);
}

NullEngine::~NullEngine() = default;

Capabilities NullEngine::capabilities() const
{
    Capabilities caps;
    caps.variableRate = true;
    caps.pitchCorrection = true;
    caps.externalSubtitles = true;
    caps.gapless = true;
    caps.hardwareDecode = false;
    caps.audioTrackSelection = true;
    caps.subtitleTrackSelection = true;
    caps.video = false; // Renders nothing, so it never claims an output kind.
    caps.rateRange = {0.25, 4.0};
    return caps;
}

void NullEngine::setSource(const QUrl &url)
{
    m_timer->stop();
    resetForNewSource(url);
    updatePlaybackState(PlaybackState::Stopped);

    if (url.isEmpty()) {
        rebuildTracks(0, 0, false);
        emit tracksChanged();
        return;
    }

    const QUrlQuery query(url);
    const QString failure = query.queryItemValue(QStringLiteral("fail"));
    if (!failure.isEmpty()) {
        updateMediaStatus(MediaStatus::Invalid);
        reportError(errorFromKeyword(failure), describe(errorFromKeyword(failure)));
        return;
    }

    const Msec duration = query.hasQueryItem(QStringLiteral("duration"))
            ? query.queryItemValue(QStringLiteral("duration")).toLongLong()
            : 180'000;
    m_tickMs = query.hasQueryItem(QStringLiteral("tick"))
            ? std::max(1, query.queryItemValue(QStringLiteral("tick")).toInt())
            : 100;

    const int audioCount = query.hasQueryItem(QStringLiteral("audio"))
            ? query.queryItemValue(QStringLiteral("audio")).toInt()
            : 1;
    const int subCount = query.hasQueryItem(QStringLiteral("subs"))
            ? query.queryItemValue(QStringLiteral("subs")).toInt()
            : 0;
    const bool hasVideo = query.queryItemValue(QStringLiteral("video")) != QLatin1String("0");

    rebuildTracks(audioCount, subCount, hasVideo);
    emit tracksChanged();

    updateDuration(duration);
    updateSeekable(duration > 0);

    QVariantMap md;
    md[QLatin1String(meta::Title)] = url.fileName().isEmpty() ? QStringLiteral("Simulated media")
                                                             : url.fileName();
    md[QLatin1String(meta::Artist)] = QStringLiteral("NullEngine");
    updateMetaData(md);

    updateMediaStatus(MediaStatus::Loaded);
    updateMediaStatus(MediaStatus::Buffered);
}

void NullEngine::play()
{
    if (mediaStatus() == MediaStatus::NoMedia || mediaStatus() == MediaStatus::Invalid)
        return;
    if (mediaStatus() == MediaStatus::EndOfMedia) {
        updatePosition(0);
        updateMediaStatus(MediaStatus::Buffered);
    }
    updatePlaybackState(PlaybackState::Playing);
    m_timer->start(m_tickMs);
}

void NullEngine::pause()
{
    if (playbackState() != PlaybackState::Playing)
        return;
    m_timer->stop();
    updatePlaybackState(PlaybackState::Paused);
}

void NullEngine::stop()
{
    m_timer->stop();
    updatePosition(0);
    updatePlaybackState(PlaybackState::Stopped);
    if (mediaStatus() != MediaStatus::NoMedia && mediaStatus() != MediaStatus::Invalid)
        updateMediaStatus(MediaStatus::Loaded);
}

void NullEngine::seek(Msec ms)
{
    if (!isSeekable())
        return;
    const Msec target = std::clamp<Msec>(ms, 0, duration());
    updatePosition(target);
    if (target < duration() && mediaStatus() == MediaStatus::EndOfMedia)
        updateMediaStatus(MediaStatus::Buffered);
}

void NullEngine::setRate(qreal rate)
{
    const auto range = capabilities().rateRange;
    updateRate(std::clamp(rate, range.first, range.second));
}

void NullEngine::setVolume(qreal linear)
{
    updateVolume(linear);
}

void NullEngine::setMuted(bool muted)
{
    updateMuted(muted);
}

QVector<TrackInfo> NullEngine::tracks(TrackType type) const
{
    switch (type) {
    case TrackType::Video:
        return m_video;
    case TrackType::Audio:
        return m_audio;
    case TrackType::Subtitle:
        return m_subtitles;
    }
    return {};
}

QString NullEngine::activeTrack(TrackType type) const
{
    switch (type) {
    case TrackType::Video:
        return m_activeVideo;
    case TrackType::Audio:
        return m_activeAudio;
    case TrackType::Subtitle:
        return m_activeSubtitle;
    }
    return {};
}

void NullEngine::selectTrack(TrackType type, const QString &id)
{
    const auto available = tracks(type);
    const bool known = id.isEmpty()
            || std::any_of(available.cbegin(), available.cend(),
                           [&id](const TrackInfo &t) { return t.id == id; });
    if (!known)
        return;

    switch (type) {
    case TrackType::Video:
        m_activeVideo = id;
        break;
    case TrackType::Audio:
        m_activeAudio = id;
        break;
    case TrackType::Subtitle:
        m_activeSubtitle = id;
        break;
    }
    emit tracksChanged();
}

bool NullEngine::loadExternalSubtitle(const QUrl &url)
{
    TrackInfo track;
    track.id = QStringLiteral("ext%1").arg(++m_externalSubCount);
    track.title = url.fileName();
    track.language = QStringLiteral("external");
    m_subtitles.append(track);
    m_activeSubtitle = track.id;
    emit tracksChanged();
    return true;
}

bool NullEngine::attachOutput(VideoOutput *output)
{
    // Accepts only the null output kind; anything else is a programming
    // error worth surfacing rather than silently ignoring.
    if (output && output->kind() != OutputKind::None)
        return false;
    setOutputPointer(output);
    return true;
}

void NullEngine::detachOutput()
{
    if (auto *out = this->output())
        out->clearSurface();
    setOutputPointer(nullptr);
}

void NullEngine::advance(Msec ms)
{
    if (duration() <= 0)
        return;

    const Msec target = position() + ms;
    if (target >= duration()) {
        m_timer->stop();
        updatePosition(duration());
        updatePlaybackState(PlaybackState::Stopped);
        updateMediaStatus(MediaStatus::EndOfMedia);
        return;
    }
    updatePosition(target);
}

void NullEngine::onTick()
{
    advance(Msec(qreal(m_tickMs) * rate()));
}

void NullEngine::rebuildTracks(int audioCount, int subtitleCount, bool hasVideo)
{
    m_video.clear();
    m_audio.clear();
    m_subtitles.clear();
    m_activeVideo.clear();
    m_activeAudio.clear();
    m_activeSubtitle.clear();
    m_externalSubCount = 0;

    if (hasVideo) {
        TrackInfo v;
        v.id = QStringLiteral("v1");
        v.title = QStringLiteral("Video");
        v.codec = QStringLiteral("simulated");
        v.isDefault = true;
        m_video.append(v);
        m_activeVideo = v.id;
    }

    // Ids are deliberately sparse and non-sequential. Anything that assumes
    // "id == index" breaks here rather than in production against mpv.
    for (int i = 0; i < audioCount; ++i) {
        TrackInfo a;
        a.id = QStringLiteral("a%1").arg(i * 2 + 1);
        a.title = QStringLiteral("Audio %1").arg(i + 1);
        a.language = i == 0 ? QStringLiteral("eng") : QStringLiteral("und");
        a.codec = QStringLiteral("pcm");
        a.isDefault = (i == 0);
        m_audio.append(a);
    }
    if (!m_audio.isEmpty())
        m_activeAudio = m_audio.first().id;

    for (int i = 0; i < subtitleCount; ++i) {
        TrackInfo s;
        s.id = QStringLiteral("s%1").arg(i * 3 + 2);
        s.title = QStringLiteral("Subtitle %1").arg(i + 1);
        s.language = QStringLiteral("eng");
        m_subtitles.append(s);
    }
}

} // namespace media
