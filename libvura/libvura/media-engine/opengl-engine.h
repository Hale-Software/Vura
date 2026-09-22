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

#include "engine.h"
#include "ffmpeg-pipeline.h"

#include <memory>

QT_BEGIN_NAMESPACE
class QTimer;
QT_END_NAMESPACE

namespace media {

class GLYuvRenderer;

/// FFmpeg decodes, we draw. Renders through GLVideoOutput exactly like the
/// mpv engine, so VideoStage needs no knowledge of which one is running.
///
/// Scope of this first version: play/pause/seek, volume and mute, audio and
/// video track selection, audio-clocked A/V sync. No subtitles, no rate
/// change, no hardware decoding; capabilities() says so, and the controller
/// already gates on it.
class OpenGLEngine : public Engine
{
    Q_OBJECT

public:
    explicit OpenGLEngine(QObject *parent = nullptr);
    ~OpenGLEngine() override;

    /// Must match the id in availableBackends(): the controller maps the
    /// running engine back to a Backend through this string.
    QString name() const override { return QStringLiteral("opengl"); }
    Capabilities capabilities() const override;
    OutputKind requiredOutput() const override { return OutputKind::OpenGL; }

    void setSource(const QUrl &url) override;
    void play() override;
    void pause() override;
    void stop() override;
    void seek(Msec ms) override;

    void setRate(qreal rate) override;
    void setVolume(qreal linear) override;
    void setMuted(bool muted) override;

    QVector<TrackInfo> tracks(TrackType type) const override;
    QString activeTrack(TrackType type) const override;
    void selectTrack(TrackType type, const QString &id) override;

    bool attachOutput(VideoOutput *output) override;
    void detachOutput() override;

private:
    void startPipeline(Msec startAt);
    void stopPipeline();
    void onOpened(quint64 generation, const ffmpeg::OpenResult &result);
    void onFailed(quint64 generation, ErrorKind kind, const QString &message);
    void leaveEndOfMedia();
    void applyVolume();
    void tick();
    void scheduleNextFrame();
    void render(const QSize &size, unsigned int fbo);

    std::unique_ptr<ffmpeg::Pipeline> m_pipeline;
    std::unique_ptr<GLYuvRenderer> m_renderer;
    GLVideoOutput *m_gl = nullptr;
    QTimer *m_tick = nullptr;
    QTimer *m_frameTimer = nullptr;

    /// Bumped whenever the pipeline is replaced, so callbacks queued by an
    /// old one are recognised and ignored.
    quint64 m_generation = 0;
    bool m_opened = false;
    bool m_playWhenReady = false;
    bool m_eofReported = false;
    bool m_rendererFailed = false;
    int m_displayedSerial = -1;

    QVector<ffmpeg::StreamInfo> m_streams;
    int m_videoStream = -1;
    int m_audioStream = -1;
    int m_wantVideo = -1;
    int m_wantAudio = -1;
};

}
