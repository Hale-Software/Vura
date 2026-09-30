#include "mpv-engine.h"

#include <QDebug>
#include <QMetaObject>
#include <QPointer>
#include <QStringList>
#include <QVariantList>
#include <QVariantMap>

#include <mpv/client.h>
#include <mpv/render_gl.h>

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace media {
namespace {

// reply_userdata values for observed properties.
enum PropertyId : quint64 {
    PropTimePos = 1,
    PropDuration,
    PropPause,
    PropCoreIdle,
    PropEofReached,
    PropPausedForCache,
    PropSeekable,
    PropTrackList,
    PropMediaTitle,
    PropVolume,
    PropMute,
    PropSpeed,
};

QVariant nodeToVariant(const mpv_node *node)
{
    switch (node->format) {
    case MPV_FORMAT_STRING:
        return QString::fromUtf8(node->u.string);
    case MPV_FORMAT_FLAG:
        return node->u.flag != 0;
    case MPV_FORMAT_INT64:
        return qlonglong(node->u.int64);
    case MPV_FORMAT_DOUBLE:
        return node->u.double_;
    case MPV_FORMAT_NODE_ARRAY: {
        QVariantList list;
        for (int i = 0; i < node->u.list->num; ++i)
            list.append(nodeToVariant(&node->u.list->values[i]));
        return list;
    }
    case MPV_FORMAT_NODE_MAP: {
        QVariantMap map;
        for (int i = 0; i < node->u.list->num; ++i) {
            map.insert(QString::fromUtf8(node->u.list->keys[i]),
                       nodeToVariant(&node->u.list->values[i]));
        }
        return map;
    }
    default:
        return {};
    }
}

ErrorKind errorFromMpv(int code)
{
    switch (code) {
    case MPV_ERROR_LOADING_FAILED:
        return ErrorKind::NotFound;
    case MPV_ERROR_UNKNOWN_FORMAT:
        return ErrorKind::UnsupportedFormat;
    case MPV_ERROR_UNSUPPORTED:
        return ErrorKind::MissingCodec;
    case MPV_ERROR_NOTHING_TO_PLAY:
        return ErrorKind::NotFound;
    case MPV_ERROR_AO_INIT_FAILED:
    case MPV_ERROR_VO_INIT_FAILED:
        return ErrorKind::Internal;
    default:
        return ErrorKind::DecodeFailure;
    }
}

TrackType typeFromMpv(const QString &type, bool *ok)
{
    *ok = true;
    if (type == QLatin1String("audio"))
        return TrackType::Audio;
    if (type == QLatin1String("sub"))
        return TrackType::Subtitle;
    if (type == QLatin1String("video"))
        return TrackType::Video;
    *ok = false;
    return TrackType::Audio;
}

const char *propertyForType(TrackType type)
{
    switch (type) {
    case TrackType::Video:
        return "vid";
    case TrackType::Audio:
        return "aid";
    case TrackType::Subtitle:
        return "sid";
    }
    return "aid";
}

} // namespace

MpvEngine::MpvEngine(QObject *parent)
    : Engine(parent)
{
    m_mpv = mpv_create();
    if (!m_mpv)
        throw std::runtime_error("mpv_create() failed");

    // Must be set before initialize: the render API requires the libmpv vo.
    mpv_set_option_string(m_mpv, "vo", "libmpv");
    mpv_set_option_string(m_mpv, "config", "no");
    mpv_set_option_string(m_mpv, "terminal", "no");
    mpv_set_option_string(m_mpv, "input-default-bindings", "no");
    mpv_set_option_string(m_mpv, "input-vo-keyboard", "no");
    mpv_set_option_string(m_mpv, "osc", "no");
    mpv_set_option_string(m_mpv, "hwdec", "auto-safe");
    mpv_set_option_string(m_mpv, "audio-pitch-correction", "yes");
    mpv_set_option_string(m_mpv, "keep-open", "yes"); // We decide what happens at EOF.
    mpv_set_option_string(m_mpv, "idle", "yes");

    if (mpv_initialize(m_mpv) < 0) {
        mpv_terminate_destroy(m_mpv);
        m_mpv = nullptr;
        throw std::runtime_error("mpv_initialize() failed");
    }

    mpv_observe_property(m_mpv, PropTimePos, "time-pos", MPV_FORMAT_DOUBLE);
    mpv_observe_property(m_mpv, PropDuration, "duration", MPV_FORMAT_DOUBLE);
    mpv_observe_property(m_mpv, PropPause, "pause", MPV_FORMAT_FLAG);
    mpv_observe_property(m_mpv, PropCoreIdle, "core-idle", MPV_FORMAT_FLAG);
    mpv_observe_property(m_mpv, PropEofReached, "eof-reached", MPV_FORMAT_FLAG);
    mpv_observe_property(m_mpv, PropPausedForCache, "paused-for-cache", MPV_FORMAT_FLAG);
    mpv_observe_property(m_mpv, PropSeekable, "seekable", MPV_FORMAT_FLAG);
    mpv_observe_property(m_mpv, PropTrackList, "track-list", MPV_FORMAT_NODE);
    mpv_observe_property(m_mpv, PropMediaTitle, "media-title", MPV_FORMAT_STRING);
    mpv_observe_property(m_mpv, PropVolume, "volume", MPV_FORMAT_DOUBLE);
    mpv_observe_property(m_mpv, PropMute, "mute", MPV_FORMAT_FLAG);
    mpv_observe_property(m_mpv, PropSpeed, "speed", MPV_FORMAT_DOUBLE);

    mpv_set_wakeup_callback(m_mpv, &MpvEngine::onWakeup, this);
}

MpvEngine::~MpvEngine()
{
    if (!m_mpv)
        return;

    // Stop the callback before anything is torn down, so no queued call can
    // reach a half-destroyed object.
    mpv_set_wakeup_callback(m_mpv, nullptr, nullptr);
    detachOutput();
    mpv_terminate_destroy(m_mpv);
    m_mpv = nullptr;
}

Capabilities MpvEngine::capabilities() const
{
    Capabilities caps;
    caps.variableRate = true;
    caps.pitchCorrection = true;
    caps.externalSubtitles = true;
    caps.gapless = true;
    caps.hardwareDecode = true;
    caps.audioTrackSelection = true;
    caps.subtitleTrackSelection = true;
    caps.video = true;
    caps.rateRange = {0.1, 8.0};
    return caps;
}

void MpvEngine::setSource(const QUrl &url)
{
    m_tracks.clear();
    m_activeTracks.clear();
    m_fileLoaded = false;
    m_eofReached = false;
    resetForNewSource(url);
    emit tracksChanged();

    if (url.isEmpty()) {
        command({QStringLiteral("stop")});
        return;
    }

    const QString target = url.isLocalFile() ? url.toLocalFile() : url.toString();
    // Load paused; the controller decides whether to start, which keeps
    // "open without autoplay" and resume-position restore straightforward.
    setProperty("pause", true);
    command({QStringLiteral("loadfile"), target});
}

void MpvEngine::play()
{
    if (m_eofReached)
        seek(0);
    setProperty("pause", false);
}

void MpvEngine::pause()
{
    setProperty("pause", true);
}

void MpvEngine::stop()
{
    command({QStringLiteral("stop")});
    updatePosition(0);
    updatePlaybackState(PlaybackState::Stopped);
}

void MpvEngine::seek(Msec ms)
{
    if (!isSeekable())
        return;
    const double seconds = double(std::clamp<Msec>(ms, 0, std::max<Msec>(0, duration()))) / 1000.0;
    // "absolute" + "keyframes" keeps scrubbing responsive; the exact frame
    // lands when the user releases the slider and we issue an exact seek.
    command({QStringLiteral("seek"), QString::number(seconds, 'f', 3),
             QStringLiteral("absolute")});
}

void MpvEngine::setRate(qreal rate)
{
    const auto range = capabilities().rateRange;
    setProperty("speed", std::clamp(rate, range.first, range.second));
}

void MpvEngine::setVolume(qreal linear)
{
    setProperty("volume", std::clamp<qreal>(linear, 0.0, 1.0) * 100.0);
}

void MpvEngine::setMuted(bool muted)
{
    setProperty("mute", muted);
}

QVector<TrackInfo> MpvEngine::tracks(TrackType type) const
{
    return m_tracks.value(type);
}

QString MpvEngine::activeTrack(TrackType type) const
{
    return m_activeTracks.value(type);
}

void MpvEngine::selectTrack(TrackType type, const QString &id)
{
    setProperty(propertyForType(type), id.isEmpty() ? QStringLiteral("no") : id);
}

bool MpvEngine::loadExternalSubtitle(const QUrl &url)
{
    const QString target = url.isLocalFile() ? url.toLocalFile() : url.toString();
    command({QStringLiteral("sub-add"), target, QStringLiteral("select")});
    return true;
}

bool MpvEngine::attachOutput(VideoOutput *output)
{
    if (!output) {
        detachOutput();
        return true;
    }
    if (output->kind() != OutputKind::OpenGL)
        return false;

    auto *gl = dynamic_cast<GLVideoOutput *>(output);
    if (!gl)
        return false;

    detachOutput();
    m_glOutput = gl;

    bool created = false;
    // The render context must be created on the render thread with the GL
    // context current, which is why this goes through the output rather
    // than being done inline here.
    gl->executeWithContext([this, gl, &created] {
        mpv_opengl_init_params glInit{};
        glInit.get_proc_address = &MpvEngine::onGetProcAddress;
        glInit.get_proc_address_ctx = gl;

        int advancedControl = 1;
        mpv_render_param params[] = {
            {MPV_RENDER_PARAM_API_TYPE, const_cast<char *>(MPV_RENDER_API_TYPE_OPENGL)},
            {MPV_RENDER_PARAM_OPENGL_INIT_PARAMS, &glInit},
            {MPV_RENDER_PARAM_ADVANCED_CONTROL, &advancedControl},
            {MPV_RENDER_PARAM_INVALID, nullptr},
        };
        created = mpv_render_context_create(&m_renderContext, m_mpv, params) >= 0;
    });

    if (!created) {
        m_renderContext = nullptr;
        m_glOutput = nullptr;
        reportError(ErrorKind::Internal,
                    tr("Could not create the mpv OpenGL render context."));
        return false;
    }

    mpv_render_context_set_update_callback(m_renderContext, &MpvEngine::onRenderUpdate, this);
    gl->setRenderCallback([this](const QSize &size, unsigned int fbo) { renderFrame(size, fbo); });

    setOutputPointer(output);
    return true;
}

void MpvEngine::detachOutput()
{
    if (!m_renderContext) {
        setOutputPointer(nullptr);
        return;
    }

    mpv_render_context_set_update_callback(m_renderContext, nullptr, nullptr);

    if (m_glOutput) {
        m_glOutput->setRenderCallback(nullptr);
        // Freeing also has to happen with the GL context current, or the
        // driver leaks every texture mpv allocated.
        m_glOutput->executeWithContext([this] {
            mpv_render_context_free(m_renderContext);
            m_renderContext = nullptr;
        });
        m_glOutput->clearSurface();
    } else {
        mpv_render_context_free(m_renderContext);
        m_renderContext = nullptr;
    }

    m_glOutput = nullptr;
    setOutputPointer(nullptr);
}

void MpvEngine::renderFrame(const QSize &sizeInPixels, unsigned int fbo)
{
    if (!m_renderContext)
        return;

    mpv_opengl_fbo mpvFbo{int(fbo), sizeInPixels.width(), sizeInPixels.height(), 0};
    int flipY = 1;
    mpv_render_param params[] = {
        {MPV_RENDER_PARAM_OPENGL_FBO, &mpvFbo},
        {MPV_RENDER_PARAM_FLIP_Y, &flipY},
        {MPV_RENDER_PARAM_INVALID, nullptr},
    };
    mpv_render_context_render(m_renderContext, params);
}

void MpvEngine::processEvents()
{
    if (!m_mpv)
        return;

    for (;;) {
        mpv_event *event = mpv_wait_event(m_mpv, 0);
        if (!event || event->event_id == MPV_EVENT_NONE)
            break;

        switch (event->event_id) {
        case MPV_EVENT_PROPERTY_CHANGE: {
            auto *prop = static_cast<mpv_event_property *>(event->data);
            handlePropertyChange(event->reply_userdata, prop->data, prop->format);
            break;
        }
        case MPV_EVENT_START_FILE:
            m_fileLoaded = false;
            m_eofReached = false;
            updateMediaStatus(MediaStatus::Loading);
            break;
        case MPV_EVENT_FILE_LOADED:
            m_fileLoaded = true;
            updateMediaStatus(MediaStatus::Loaded);
            refreshStatus();
            break;
        case MPV_EVENT_END_FILE: {
            auto *endFile = static_cast<mpv_event_end_file *>(event->data);
            m_fileLoaded = false;
            if (endFile->reason == MPV_END_FILE_REASON_ERROR) {
                const ErrorKind kind = errorFromMpv(endFile->error);
                updateMediaStatus(MediaStatus::Invalid);
                reportError(kind, QString::fromUtf8(mpv_error_string(endFile->error)));
            } else if (endFile->reason == MPV_END_FILE_REASON_EOF) {
                updateMediaStatus(MediaStatus::EndOfMedia);
                updatePlaybackState(PlaybackState::Stopped);
            }
            break;
        }
        case MPV_EVENT_SHUTDOWN:
            reportError(ErrorKind::Internal, tr("The playback engine shut down unexpectedly."));
            return;
        default:
            break;
        }
    }
}

void MpvEngine::handlePropertyChange(quint64 id, void *data, int format)
{
    // A null payload means the property is currently unavailable, which is
    // normal between files.
    const bool hasValue = data != nullptr;

    switch (id) {
    case PropTimePos:
        if (hasValue && format == MPV_FORMAT_DOUBLE)
            updatePosition(Msec(std::llround(*static_cast<double *>(data) * 1000.0)));
        break;
    case PropDuration:
        if (hasValue && format == MPV_FORMAT_DOUBLE)
            updateDuration(Msec(std::llround(*static_cast<double *>(data) * 1000.0)));
        break;
    case PropPause:
        if (hasValue && format == MPV_FORMAT_FLAG) {
            m_paused = *static_cast<int *>(data) != 0;
            refreshStatus();
        }
        break;
    case PropCoreIdle:
        if (hasValue && format == MPV_FORMAT_FLAG) {
            m_coreIdle = *static_cast<int *>(data) != 0;
            refreshStatus();
        }
        break;
    case PropEofReached:
        if (hasValue && format == MPV_FORMAT_FLAG) {
            m_eofReached = *static_cast<int *>(data) != 0;
            if (m_eofReached) {
                updateMediaStatus(MediaStatus::EndOfMedia);
                updatePlaybackState(PlaybackState::Stopped);
            }
        }
        break;
    case PropPausedForCache:
        if (hasValue && format == MPV_FORMAT_FLAG) {
            m_pausedForCache = *static_cast<int *>(data) != 0;
            refreshStatus();
        }
        break;
    case PropSeekable:
        if (hasValue && format == MPV_FORMAT_FLAG)
            updateSeekable(*static_cast<int *>(data) != 0);
        break;
    case PropTrackList:
        if (hasValue && format == MPV_FORMAT_NODE)
            applyTrackList(nodeToVariant(static_cast<mpv_node *>(data)));
        break;
    case PropMediaTitle:
        if (hasValue && format == MPV_FORMAT_STRING) {
            QVariantMap md = metaData();
            md[QLatin1String(meta::Title)] =
                    QString::fromUtf8(*static_cast<char **>(data));
            updateMetaData(md);
        }
        break;
    case PropVolume:
        if (hasValue && format == MPV_FORMAT_DOUBLE)
            updateVolume(*static_cast<double *>(data) / 100.0);
        break;
    case PropMute:
        if (hasValue && format == MPV_FORMAT_FLAG)
            updateMuted(*static_cast<int *>(data) != 0);
        break;
    case PropSpeed:
        if (hasValue && format == MPV_FORMAT_DOUBLE)
            updateRate(*static_cast<double *>(data));
        break;
    default:
        break;
    }
}

void MpvEngine::applyTrackList(const QVariant &list)
{
    QHash<TrackType, QVector<TrackInfo>> parsed;
    QHash<TrackType, QString> active;

    const QVariantList entries = list.toList();
    for (const QVariant &entry : entries) {
        const QVariantMap map = entry.toMap();

        bool known = false;
        const TrackType type = typeFromMpv(map.value(QStringLiteral("type")).toString(), &known);
        if (!known)
            continue;

        TrackInfo track;
        // mpv's ids are sparse: a file can have audio ids 1 and 3 with no 2.
        // Carrying them through as opaque strings is the whole reason
        // TrackInfo::id is not an index.
        track.id = QString::number(map.value(QStringLiteral("id")).toLongLong());
        track.title = map.value(QStringLiteral("title")).toString();
        track.language = map.value(QStringLiteral("lang")).toString();
        track.codec = map.value(QStringLiteral("codec")).toString();
        track.isDefault = map.value(QStringLiteral("default")).toBool();

        parsed[type].append(track);
        if (map.value(QStringLiteral("selected")).toBool())
            active[type] = track.id;
    }

    if (parsed == m_tracks && active == m_activeTracks)
        return;

    m_tracks = parsed;
    m_activeTracks = active;
    emit tracksChanged();
}

void MpvEngine::refreshStatus()
{
    if (!m_fileLoaded)
        return;

    if (m_pausedForCache) {
        updateMediaStatus(MediaStatus::Buffering);
    } else if (mediaStatus() != MediaStatus::EndOfMedia) {
        updateMediaStatus(MediaStatus::Buffered);
    }

    if (m_paused)
        updatePlaybackState(PlaybackState::Paused);
    else if (!m_coreIdle || !m_pausedForCache)
        updatePlaybackState(PlaybackState::Playing);
}

void MpvEngine::setProperty(const char *name, const QVariant &value)
{
    if (!m_mpv)
        return;

    switch (value.typeId()) {
    case QMetaType::Bool: {
        int flag = value.toBool() ? 1 : 0;
        mpv_set_property(m_mpv, name, MPV_FORMAT_FLAG, &flag);
        break;
    }
    case QMetaType::Double: {
        double number = value.toDouble();
        mpv_set_property(m_mpv, name, MPV_FORMAT_DOUBLE, &number);
        break;
    }
    default: {
        const QByteArray utf8 = value.toString().toUtf8();
        const char *raw = utf8.constData();
        mpv_set_property(m_mpv, name, MPV_FORMAT_STRING, &raw);
        break;
    }
    }
}

void MpvEngine::command(const QStringList &args)
{
    if (!m_mpv)
        return;

    QList<QByteArray> storage;
    storage.reserve(args.size());
    for (const QString &arg : args)
        storage.append(arg.toUtf8());

    QVarLengthArray<const char *, 8> argv;
    for (const QByteArray &arg : storage)
        argv.append(arg.constData());
    argv.append(nullptr);

    const int result = mpv_command(m_mpv, argv.data());
    if (result < 0) {
        qWarning() << "mpv command failed:" << args
                   << QString::fromUtf8(mpv_error_string(result));
    }
}

void MpvEngine::onWakeup(void *ctx)
{
    // Arbitrary thread. Do nothing here except hop to the GUI thread.
    auto *self = static_cast<MpvEngine *>(ctx);
    QMetaObject::invokeMethod(self, [self] { self->processEvents(); }, Qt::QueuedConnection);
}

void MpvEngine::onRenderUpdate(void *ctx)
{
    // Also an arbitrary thread. VideoOutput::requestRedraw() is documented
    // as thread-safe for exactly this call.
    auto *self = static_cast<MpvEngine *>(ctx);
    if (auto *out = self->m_glOutput)
        out->requestRedraw();
}

void *MpvEngine::onGetProcAddress(void *ctx, const char *name)
{
    return static_cast<GLVideoOutput *>(ctx)->glProcAddress(name);
}

} // namespace media
