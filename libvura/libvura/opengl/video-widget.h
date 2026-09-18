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

#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <QTimer>
#include <QElapsedTimer>
#include <QMouseEvent>
#include "frame-queue.h"
#include "audio-output.h"

extern "C" {
#include <libavutil/rational.h>
}

class VideoWidget : public QOpenGLWidget, protected QOpenGLExtraFunctions {
    Q_OBJECT

public:
    explicit VideoWidget(FrameQueue* queue, QWidget* parent = nullptr);
    ~VideoWidget() override;

    void setFrameQueue(FrameQueue* queue) { frameQueue = queue; }
    void setFramerate(double fps);
    void setVideoTimeBase(AVRational timeBase);
    void setPlaybackRate(double rate);

    /**
     * Provide an AudioOutput so paintGL() uses the audio hardware clock
     * as the sync master instead of QElapsedTimer.
     *
     * Call this after AudioOutput::init() succeeds.
     * Pass nullptr to fall back to wall-clock mode (video-only files).
     */
    void setAudioClock(AudioOutput* audio) { m_audioOutput = audio; }

    /**
     * Called by MediaPlayer after a seek.
     * Clears the clock anchor so it re-establishes on the next frame.
     */
    void resetClock();

signals:
    void positionChanged(int seconds);

public slots:
    void play();
    void pause();
    void togglePlayback();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    void uploadFrame(AVFrame* frame);

    FrameQueue*            frameQueue    = nullptr;
    QTimer*                renderTimer   = nullptr;
    QOpenGLShaderProgram*  shaderProgram = nullptr;
    AudioOutput*           m_audioOutput = nullptr;  // borrowed, not owned

    GLuint textureIds[3] = {};
    GLuint vbo = 0, vao = 0;

    AVFrame* currentFrame = nullptr;

    int videoWidth   = 0;
    int videoHeight  = 0;
    int windowWidth  = 0;
    int windowHeight = 0;

    bool isPlaying      = false;
    int  renderInterval = 16;

    AVRational m_timeBase = {1, 1};

    // -----------------------------------------------------------------------
    // Sync clock
    //
    // Master clock priority:
    //   1. AudioOutput::getPositionMs()  — when audio is available
    //      The audio hardware clock is the most accurate source and
    //      automatically accounts for driver buffering / resampler latency.
    //
    //   2. QElapsedTimer (wall clock)    — video-only fallback
    //      Same anchor model as before: first frame sets the anchor,
    //      subsequent frames are compared against elapsed wall time.
    //
    // resetClock() sets m_clockAnchored = false.
    // The next frame to arrive re-anchors automatically in both modes.
    // -----------------------------------------------------------------------
    QElapsedTimer m_wallClock;
    bool    m_clockAnchored = false;
    int64_t m_clockAnchorMs = 0;
    int64_t m_ptsAnchorMs   = 0;
    int64_t m_pauseTimeMs   = 0;

    static constexpr int64_t s_syncThresholdMs = 15;
    static constexpr int64_t s_dropThresholdMs = 40;

    int64_t masterClockMs() const;
    int64_t ptsToMs(int64_t pts) const;

    double m_playbackRate = 1.0;
    double m_videoFps = 0.0;

};
