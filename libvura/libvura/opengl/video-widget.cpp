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

#include "video-widget.h"
#include <QDebug>

static const float vertices[] = {
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
    -1.0f,  1.0f,  0.0f, 1.0f,
     1.0f,  1.0f,  1.0f, 1.0f
};

// ---------------------------------------------------------------------------
// Construction / destruction
// ---------------------------------------------------------------------------

VideoWidget::VideoWidget(FrameQueue* queue, QWidget* parent)
    : QOpenGLWidget(parent), frameQueue(queue)
{
    renderTimer = new QTimer(this);
    connect(renderTimer, &QTimer::timeout, this, [this]{ update(); });
}

VideoWidget::~VideoWidget()
{
    makeCurrent();
    delete shaderProgram;
    glDeleteTextures(3, textureIds);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    doneCurrent();

    if (currentFrame) av_frame_free(&currentFrame);
}

// ---------------------------------------------------------------------------
// OpenGL setup
// ---------------------------------------------------------------------------

void VideoWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    shaderProgram = new QOpenGLShaderProgram(this);
    shaderProgram->addShaderFromSourceFile(QOpenGLShader::Vertex,   "vertex.glsl");
    shaderProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, "fragment.glsl");
    shaderProgram->link();

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glGenTextures(3, textureIds);
    for (int i = 0; i < 3; ++i) {
        glBindTexture(GL_TEXTURE_2D, textureIds[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
}

void VideoWidget::resizeGL(int w, int h)
{
    windowWidth  = w;
    windowHeight = h;
}

// ---------------------------------------------------------------------------
// Clock helpers
// ---------------------------------------------------------------------------

int64_t VideoWidget::ptsToMs(int64_t pts) const
{
    return static_cast<int64_t>(pts * av_q2d(m_timeBase) * 1000.0);
}

int64_t VideoWidget::masterClockMs() const
{
    if (!m_clockAnchored) return 0;

    // --- Audio clock (preferred) ---
    if (m_audioOutput) {
        // IAudioClock reports how many ms of audio have been sent to the
        // hardware DAC. This is the most accurate sync source available.
        //return m_audioOutput->getPositionMs();
        return (m_audioOutput->getPositionMs() - m_clockAnchorMs) + m_ptsAnchorMs;
    }

    // --- Wall clock fallback (video-only) ---
    return (m_wallClock.elapsed() - m_clockAnchorMs) + m_ptsAnchorMs;
}

void VideoWidget::resetClock()
{
    m_clockAnchored = false;
}

void VideoWidget::setVideoTimeBase(AVRational timeBase) { m_timeBase = timeBase; }

// ---------------------------------------------------------------------------
// Core render loop
// ---------------------------------------------------------------------------

void VideoWidget::paintGL()
{
    if (!frameQueue) return;

    qreal dpr   = devicePixelRatio();
    int   physW = static_cast<int>(windowWidth  * dpr);
    int   physH = static_cast<int>(windowHeight * dpr);

    glViewport(0, 0, physW, physH);
    glClear(GL_COLOR_BUFFER_BIT);

    bool uploadNeeded = false;

    while (true) {
        int64_t nextPts = frameQueue->peekPts();
        if (nextPts == AV_NOPTS_VALUE) break;

        int64_t nextPtsMs = ptsToMs(nextPts);

        // Anchor the clock on the very first frame
        if (!m_clockAnchored) {
            if (!m_audioOutput) {
                // Wall-clock mode: record the wall time at this PTS
                m_clockAnchorMs = m_wallClock.elapsed();
                //m_ptsAnchorMs   = nextPtsMs;
            } else {
                m_clockAnchorMs = m_audioOutput->getPositionMs();
            }
            // Audio-clock mode: no anchor needed — IAudioClock starts at 0
            // and advances as audio is consumed, which naturally matches PTS.
            m_ptsAnchorMs = nextPtsMs;
            m_clockAnchored = true;
        }

        int64_t master = masterClockMs();
        int64_t diff   = nextPtsMs - master;

        if (diff > s_syncThresholdMs) break;       // too early

        AVFrame* frame = frameQueue->tryPop();
        if (!frame) break;

        if (diff < -s_dropThresholdMs) {
            qDebug() << "VideoWidget: drop frame diff=" << diff << "ms";
            av_frame_free(&frame);
            continue;                               // check next frame
        }

        // On time — accept
        if (currentFrame) av_frame_free(&currentFrame);
        currentFrame = frame;
        uploadNeeded = true;

        if (videoWidth == 0 && currentFrame->width > 0) {
            videoWidth  = currentFrame->width;
            videoHeight = currentFrame->height;
        }

        if (currentFrame->pts != AV_NOPTS_VALUE) {
            double secs = currentFrame->pts * av_q2d(m_timeBase);
            emit positionChanged(static_cast<int>(secs));
        }

        break; // one frame per paint tick
    }

    if (!currentFrame || videoWidth == 0) return;

    // Aspect-ratio viewport
    float videoAspect  = static_cast<float>(videoWidth)  / static_cast<float>(videoHeight);
    float windowAspect = static_cast<float>(physW) / static_cast<float>(physH);

    int vpX, vpY, vpW, vpH;
    if (windowAspect > videoAspect) {
        vpH = physH;
        vpW = static_cast<int>(physH * videoAspect);
        vpX = (physW - vpW) / 2;
        vpY = 0;
    } else {
        vpW = physW;
        vpH = static_cast<int>(physW / videoAspect);
        vpX = 0;
        vpY = (physH - vpH) / 2;
    }
    glViewport(vpX, vpY, vpW, vpH);

    shaderProgram->bind();
    shaderProgram->setUniformValue("texY", 0);
    shaderProgram->setUniformValue("texU", 1);
    shaderProgram->setUniformValue("texV", 2);

    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, textureIds[0]);
    glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, textureIds[1]);
    glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_2D, textureIds[2]);

    if (uploadNeeded)
        uploadFrame(currentFrame);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    shaderProgram->release();
}

void VideoWidget::uploadFrame(AVFrame* frame)
{
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureIds[0]);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, frame->linesize[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, frame->width, frame->height,
                 0, GL_RED, GL_UNSIGNED_BYTE, frame->data[0]);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureIds[1]);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, frame->linesize[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, frame->width / 2, frame->height / 2,
                 0, GL_RED, GL_UNSIGNED_BYTE, frame->data[1]);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, textureIds[2]);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, frame->linesize[2]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, frame->width / 2, frame->height / 2,
                 0, GL_RED, GL_UNSIGNED_BYTE, frame->data[2]);

    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
}

// ---------------------------------------------------------------------------
// Playback control
// ---------------------------------------------------------------------------

void VideoWidget::setFramerate(double fps)
{
    if (fps > 0.0) {
        renderInterval = qMax(static_cast<int>(500.0 / fps), 4);
        if (isPlaying) renderTimer->start(renderInterval);
        m_videoFps = fps;
        qDebug() << "VideoWidget: framerate" << fps << "fps, poll" << renderInterval << "ms";
    }
}

void VideoWidget::setPlaybackRate(double rate) {
    m_playbackRate = rate;

    if (isPlaying) {
        // Recalculate the timer interval based on the original video FPS
        int baseIntervalMs = 1000 / m_videoFps;
        int newIntervalMs = static_cast<int>(baseIntervalMs / m_playbackRate);

        renderTimer->setInterval(newIntervalMs);
    }
}

void VideoWidget::play()
{
    if (!isPlaying) {
        if (m_clockAnchored && !m_audioOutput) {
            // Wall-clock mode: re-anchor to continue from where we paused
            m_clockAnchorMs = m_wallClock.elapsed() - (m_pauseTimeMs - m_ptsAnchorMs);
        }
        m_wallClock.start();
        renderTimer->start(renderInterval);
        isPlaying = true;
    }
}

void VideoWidget::pause()
{
    if (isPlaying) {
        if (!m_audioOutput)
            m_pauseTimeMs = masterClockMs();
        renderTimer->stop();
        isPlaying = false;
    }
}

void VideoWidget::togglePlayback() { isPlaying ? pause() : play(); }

void VideoWidget::mousePressEvent(QMouseEvent* event)
{
    togglePlayback();
    QOpenGLWidget::mousePressEvent(event);
}
