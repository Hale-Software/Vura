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

#include "opengl-engine.h"

#include "gl-yuv-renderer.h"

#include <QDebug>
#include <QMetaObject>
#include <QTimer>

#include <algorithm>
#include <cmath>

namespace media {
namespace {

// Position, end-of-media detection and a redraw kick. The controller
// throttles what reaches the UI, so this only needs to be regular.
constexpr int kTickMs = 40;

}

OpenGLEngine::OpenGLEngine(QObject *parent)
    : Engine(parent), m_tick(new QTimer(this)), m_frameTimer(new QTimer(this))
{
    m_tick->setInterval(kTickMs);
    connect(m_tick, &QTimer::timeout, this, &OpenGLEngine::tick);

    m_frameTimer->setSingleShot(true);
    m_frameTimer->setTimerType(Qt::PreciseTimer);
    connect(m_frameTimer, &QTimer::timeout, this, [this] {
        if (m_gl)
            m_gl->requestRedraw();
    });
}

OpenGLEngine::~OpenGLEngine()
{
    stopPipeline();
    detachOutput();
}

Capabilities OpenGLEngine::capabilities() const
{
    Capabilities caps;
    caps.variableRate = false;         // needs an atempo filter for audio
    caps.externalSubtitles = false;
    caps.audioDeviceSelection = false; // default device only for now
    return caps;
}

// ----------------------------------------------------------- pipeline ----

void OpenGLEngine::startPipeline(Msec startAt)
{
    const quint64 generation = ++m_generation;
    m_opened = false;
    m_eofReported = false;
    m_displayedSerial = -1;

    // Callbacks fire on the demux thread. Hop to ours before touching
    // anything; the generation check discards results from a pipeline that
    // has since been replaced, and using `this` as the context drops them
    // entirely if the engine is gone.
    ffmpeg::Pipeline::Callbacks callbacks;
    callbacks.opened = [this, generation](const ffmpeg::OpenResult &result) {
        QMetaObject::invokeMethod(this, [this, generation, result] { onOpened(generation, result); },
                                  Qt::QueuedConnection);
    };
    callbacks.failed = [this, generation](ErrorKind kind, const QString &message) {
        QMetaObject::invokeMethod(this, [this, generation, kind, message] { onFailed(generation, kind, message); },
                                  Qt::QueuedConnection);
    };

    m_pipeline = std::make_unique<ffmpeg::Pipeline>(source(), startAt, m_wantVideo, m_wantAudio, std::move(callbacks));
    applyVolume();
    m_pipeline->start();

    updateMediaStatus(MediaStatus::Loading);
    m_tick->start();
}

void OpenGLEngine::stopPipeline()
{
    m_tick->stop();
    m_frameTimer->stop();
    ++m_generation;
    m_pipeline.reset(); // joins every worker thread
    m_opened = false;
    m_playWhenReady = false;
    m_eofReported = false;
    m_displayedSerial = -1;

    if (m_renderer)
        m_renderer->forgetFrame();
    if (m_gl)
        m_gl->requestRedraw();
}

void OpenGLEngine::onOpened(quint64 generation, const ffmpeg::OpenResult &result)
{
    if (generation != m_generation)
        return;

    m_opened = true;
    m_streams = result.streams;
    m_videoStream = result.videoStream;
    m_audioStream = result.audioStream;

    updateDuration(result.duration);
    updateSeekable(result.seekable);
    updateMetaData(result.metadata);
    emit tracksChanged();

    const bool playNow = m_playWhenReady;
    m_playWhenReady = false;

    // The controller reacts to Loaded synchronously (resume seek, autoplay),
    // so everything above must be in place before this line.
    updateMediaStatus(MediaStatus::Loaded);

    if (m_gl)
        m_gl->requestRedraw(); // first frame, even while paused
    if (playNow && generation == m_generation)
        play();
}

void OpenGLEngine::onFailed(quint64 generation, ErrorKind kind, const QString &message)
{
    if (generation != m_generation)
        return;

    stopPipeline();
    updatePlaybackState(PlaybackState::Stopped);
    updateMediaStatus(MediaStatus::Invalid);
    reportError(kind, message);
}

// ----------------------------------------------------------- transport ---

void OpenGLEngine::setSource(const QUrl &url)
{
    stopPipeline();
    m_streams.clear();
    m_videoStream = m_audioStream = -1;
    m_wantVideo = m_wantAudio = -1;

    updatePlaybackState(PlaybackState::Stopped);
    resetForNewSource(url);
    emit tracksChanged();

    if (!url.isEmpty())
        startPipeline(0);
}

void OpenGLEngine::play()
{
    if (source().isEmpty())
        return;

    if (!m_pipeline) { // after stop()
        startPipeline(0);
        m_playWhenReady = true;
        return;
    }
    if (!m_opened) {
        m_playWhenReady = true;
        return;
    }
    if (m_eofReported) {
        m_pipeline->seek(0);
        updatePosition(0);
        leaveEndOfMedia();
    }

    m_pipeline->setPaused(false);
    updatePlaybackState(PlaybackState::Playing);
    scheduleNextFrame();
}

void OpenGLEngine::pause()
{
    if (!m_pipeline || !m_opened) {
        m_playWhenReady = false;
        return;
    }
    m_pipeline->setPaused(true);
    m_frameTimer->stop();
    const double clock = m_pipeline->clock();
    if (std::isfinite(clock))
        updatePosition(Msec(std::llround(clock * 1000.0)));
    updatePlaybackState(PlaybackState::Paused);
}

void OpenGLEngine::stop()
{
    // Tearing the pipeline down releases the file and the audio device;
    // play() rebuilds it from the start.
    stopPipeline();
    updatePlaybackState(PlaybackState::Stopped);
    updatePosition(0);
}

void OpenGLEngine::seek(Msec ms)
{
    if (!m_pipeline) {
        if (!source().isEmpty()) {
            startPipeline(ms);
            updatePosition(ms);
        }
        return;
    }
    if (!m_opened || !isSeekable())
        return;

    ms = std::clamp<Msec>(ms, 0, duration() > 0 ? duration() : ms);
    m_pipeline->seek(ms);
    updatePosition(ms);

    if (m_eofReported) {
        m_eofReported = false;
        leaveEndOfMedia();
    }
    if (m_gl)
        m_gl->requestRedraw();
}

void OpenGLEngine::leaveEndOfMedia()
{
    m_eofReported = false;
    // Deferred: the controller calls seek(0)/play() from inside its own
    // EndOfMedia handler, and changing status re-entrantly there would leave
    // it emitting EndOfMedia after Loaded.
    const quint64 generation = m_generation;
    QMetaObject::invokeMethod(this, [this, generation] {
        if (generation == m_generation && !m_eofReported && mediaStatus() == MediaStatus::EndOfMedia)
            updateMediaStatus(MediaStatus::Loaded);
    }, Qt::QueuedConnection);
}

void OpenGLEngine::setRate(qreal rate)
{
    Q_UNUSED(rate) // capabilities().variableRate is false
}

void OpenGLEngine::setVolume(qreal linear)
{
    updateVolume(linear);
    applyVolume();
}

void OpenGLEngine::setMuted(bool muted)
{
    updateMuted(muted);
    applyVolume();
}

void OpenGLEngine::applyVolume()
{
    if (m_pipeline)
        m_pipeline->setVolume(isMuted() ? 0.0 : volume());
}

// -------------------------------------------------------------- tracks ---

QVector<TrackInfo> OpenGLEngine::tracks(TrackType type) const
{
    QVector<TrackInfo> out;
    for (const ffmpeg::StreamInfo &s : m_streams) {
        if (s.type != type)
            continue;
        // ADJUST if your TrackInfo uses different field names.
        TrackInfo info;
        info.id = QString::number(s.index);
        info.title = s.title.isEmpty() ? s.codec : s.title;
        info.language = s.language;
        out.append(info);
    }
    return out;
}

QString OpenGLEngine::activeTrack(TrackType type) const
{
    switch (type) {
    case TrackType::Video:
        return m_videoStream >= 0 ? QString::number(m_videoStream) : QString();
    case TrackType::Audio:
        return m_audioStream >= 0 ? QString::number(m_audioStream) : QString();
    default:
        return {};
    }
}

void OpenGLEngine::selectTrack(TrackType type, const QString &id)
{
    if (type != TrackType::Video && type != TrackType::Audio)
        return; // subtitles are not rendered yet

    bool ok = false;
    const int index = id.toInt(&ok);
    const bool known = std::any_of(m_streams.cbegin(), m_streams.cend(),
                                   [&](const ffmpeg::StreamInfo &s) { return s.index == index && s.type == type; });
    if (!ok || !known)
        return;
    if ((type == TrackType::Video ? m_videoStream : m_audioStream) == index)
        return;

    // Decoders are opened per pipeline, so switching a stream means
    // reopening at the current position. Cheap for local files, and it
    // keeps the pipeline free of mid-stream reconfiguration.
    m_wantVideo = type == TrackType::Video ? index : m_videoStream;
    m_wantAudio = type == TrackType::Audio ? index : m_audioStream;

    const Msec resumeAt = position();
    const bool wasPlaying = playbackState() == PlaybackState::Playing;
    stopPipeline();
    startPipeline(resumeAt);
    m_playWhenReady = wasPlaying;
}

// -------------------------------------------------------------- output ---

bool OpenGLEngine::attachOutput(VideoOutput *output)
{
    if (!output || output->kind() != OutputKind::OpenGL)
        return false;
    if (output == m_gl)
        return true;

    detachOutput();
    m_gl = static_cast<GLVideoOutput *>(output);
    setOutputPointer(output);

    // GL objects are created lazily on the first render, where the context
    // is guaranteed current. Doing it here through executeWithContext would
    // queue a lambda holding `this` that could outlive us if the widget is
    // never shown.
    m_gl->setRenderCallback([this](const QSize &size, unsigned int fbo) { render(size, fbo); });
    m_gl->requestRedraw();
    return true;
}

void OpenGLEngine::detachOutput()
{
    if (!m_gl)
        return;

    m_gl->setRenderCallback(nullptr);
    if (m_renderer) {
        // A renderer only exists if paintGL() has run, so the widget is
        // initialised and this executes immediately rather than queueing.
        GLYuvRenderer *renderer = m_renderer.get();
        m_gl->executeWithContext([renderer] { renderer->destroy(); });
        m_renderer.reset();
    }
    m_gl->clearSurface();
    m_gl = nullptr;
    setOutputPointer(nullptr);
    m_rendererFailed = false;
}

void OpenGLEngine::render(const QSize &size, unsigned int fbo)
{
    if (!m_renderer) {
        if (m_rendererFailed)
            return;
        auto renderer = std::make_unique<GLYuvRenderer>();
        QString error;
        if (!renderer->initialize(&error)) {
            m_rendererFailed = true;
            qWarning() << "OpenGLEngine:" << error;
            // Queued: this is inside paintGL, and the controller's error
            // handling may swap sources or engines.
            QMetaObject::invokeMethod(this, [this, error] { reportError(ffmpeg::errors::Renderer, error); },
                                      Qt::QueuedConnection);
            return;
        }
        m_renderer = std::move(renderer);
    }

    if (m_pipeline && m_opened) {
        const int serial = m_pipeline->serial();
        const bool haveCurrent = m_renderer->hasFrame() && m_displayedSerial == serial;
        if (ffmpeg::VideoFramePtr frame = m_pipeline->takeFrame(m_pipeline->clock(), haveCurrent)) {
            m_renderer->upload(*frame);
            m_displayedSerial = frame->serial;
        }
    }

    m_renderer->render(size, fbo);
    scheduleNextFrame();
}

void OpenGLEngine::scheduleNextFrame()
{
    // Repaint when the next frame is due rather than on every vsync: 24 fps
    // content costs 24 paints a second instead of 60-144, and a driver that
    // ignores the swap interval can't turn this into a busy loop.
    if (!m_gl || !m_pipeline || !m_opened || playbackState() != PlaybackState::Playing)
        return;

    const double clock = m_pipeline->clock();
    const double due = m_pipeline->nextFrameDue();
    int ms = 10; // nothing decoded yet, or audio not audible yet: poll
    if (std::isfinite(clock) && std::isfinite(due))
        ms = std::clamp(int((due - clock) * 1000.0), 0, 100);

    if (!m_frameTimer->isActive() || m_frameTimer->remainingTime() > ms)
        m_frameTimer->start(ms);
}

void OpenGLEngine::tick()
{
    if (!m_pipeline || !m_opened)
        return;

    const bool playing = playbackState() == PlaybackState::Playing;
    const double clock = m_pipeline->clock();

    if (playing && std::isfinite(clock)) {
        Msec ms = std::max<Msec>(0, Msec(std::llround(clock * 1000.0)));
        if (duration() > 0)
            ms = std::min(ms, duration());
        updatePosition(ms);
        // Keeps decoding moving if nothing is painting (minimised window).
        m_pipeline->dropLateFrames(clock);
    }

    if (playing)
        scheduleNextFrame(); // safety net if a paint was ever dropped
    else if (m_gl && m_displayedSerial != m_pipeline->serial())
        m_gl->requestRedraw(); // paused seek: show the new frame once

    if (playing && !m_eofReported && m_pipeline->finished()) {
        m_eofReported = true;
        m_pipeline->setPaused(true);
        if (duration() > 0)
            updatePosition(duration());
        updatePlaybackState(PlaybackState::Stopped);
        // Last statement on purpose: the controller may respond by calling
        // setSource(), which destroys m_pipeline.
        updateMediaStatus(MediaStatus::EndOfMedia);
    }
}

}
