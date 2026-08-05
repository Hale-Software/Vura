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

#include "OpenGLWidget.h"
#include <libvura/media-engine/video-widget.h>
#include <libvura/media-engine/demuxer-thread.h>
#include <libvura/media-engine/decoder-thread.h>
#include <libvura/media-engine/audio-decoder.h>
#include <libvura/media-engine/audio-output.h>


OpenGLWidget::OpenGLWidget(QWidget *parent) : QWidget(parent), m_state(Stopped)
{
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0); // No borders

    m_videoWidget = new VideoWidget(nullptr, this);
    m_layout->addWidget(m_videoWidget);

    connect(m_videoWidget, &VideoWidget::positionChanged, this, &OpenGLWidget::onPositionChanged);

    initializePipeline();
}

OpenGLWidget::~OpenGLWidget()
{
    stop();
    cleanupPipeline();
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

void OpenGLWidget::loadMedia(const QString& filePath)
{
    stop(); // Ensure previous media is cleared
    cleanupPipeline();
    initializePipeline();

    m_videoWidget->setFrameQueue(m_videoFrameQueue);
    m_videoWidget->setAudioClock(nullptr); // cleared until audio init succeeds

    // --- Demuxer ---
    m_demuxer = new DemuxerThread(m_videoPacketQueue, m_audioPacketQueue, this);

    connect(m_demuxer, &DemuxerThread::videoDimentionsFound, this, &OpenGLWidget::onVideoDimensionsFound, Qt::QueuedConnection);

    if (!m_demuxer->openFile(filePath)) {
        qWarning() << "OpenGLWidget: failed to open" << filePath;
        return;
    }

    m_duration = static_cast<int>(m_demuxer->getDurationSeconds());
    emit durationChanged(m_duration);

    const AVFormatContext * fmt = m_demuxer->getFormatContext();

    // --- Video decoder ---
    const AVStream * vStream = fmt->streams[m_demuxer->getVideoStreamIndex()];
    m_videoWidget->setVideoTimeBase(vStream->time_base);

    const double fps = av_q2d(vStream->avg_frame_rate);
    if (fps > 0.0) m_videoWidget->setFramerate(fps);

    m_videoDecoder = new DecoderThread(m_videoPacketQueue, m_videoFrameQueue, vStream->codecpar, this);
    if (!m_videoDecoder->initDecoder()) {
        qWarning() << "OpenGLWidget: video decoder init failed";
        return;
    }

    // --- Audio decoder + output (optional) ---
    const int aIdx = m_demuxer->getAudioStreamIndex();
    if (aIdx >= 0) {
        const AVStream * aStream = fmt->streams[aIdx];

        m_audioDecoder = new AudioDecoder(m_audioPacketQueue, m_audioFrameQueue, aStream->codecpar, this);

        m_audioOutput = new AudioOutput(m_audioFrameQueue, this);

        if (m_audioDecoder->initDecoder() && m_audioOutput->init()) {
            // Give VideoWidget the audio clock as sync master
            m_videoWidget->setAudioClock(m_audioOutput);
            qDebug() << "OpenGLWidget: audio pipeline ready";
        } else {
            qWarning() << "OpenGLWidget: audio init failed, falling back to wall clock";
            // Clean up failed audio objects; video-only playback continues
            delete m_audioDecoder; m_audioDecoder = nullptr;
            delete m_audioOutput;  m_audioOutput  = nullptr;
        }
    }

    // --- Start all threads ---
    m_demuxer->start();
    m_videoDecoder->start();
    if (m_audioDecoder) m_audioDecoder->start();
    if (m_audioOutput)  m_audioOutput->start();

    play();
}

void OpenGLWidget::loadMedia(const QUrl& url)
{
    stop(); // Ensure previous media is cleared
    cleanupPipeline();
    initializePipeline();

    m_videoWidget->setFrameQueue(m_videoFrameQueue);
    m_videoWidget->setAudioClock(nullptr); // cleared until audio init succeeds

    // --- Demuxer ---
    m_demuxer = new DemuxerThread(m_videoPacketQueue, m_audioPacketQueue, this);

    connect(m_demuxer, &DemuxerThread::videoDimentionsFound, this, &OpenGLWidget::onVideoDimensionsFound, Qt::QueuedConnection);

    if (!m_demuxer->openFile(url.toLocalFile())) {
        qWarning() << "OpenGLWidget: failed to open" << url.toLocalFile();
        return;
    }

    m_duration = static_cast<int>(m_demuxer->getDurationSeconds());
    emit durationChanged(m_duration);

    const AVFormatContext * fmt = m_demuxer->getFormatContext();

    // --- Video decoder ---
    const AVStream * vStream = fmt->streams[m_demuxer->getVideoStreamIndex()];
    m_videoWidget->setVideoTimeBase(vStream->time_base);

    const double fps = av_q2d(vStream->avg_frame_rate);
    if (fps > 0.0) m_videoWidget->setFramerate(fps);

    m_videoDecoder = new DecoderThread(m_videoPacketQueue, m_videoFrameQueue, vStream->codecpar, this);
    if (!m_videoDecoder->initDecoder()) {
        qWarning() << "OpenGLWidget: video decoder init failed";
        return;
    }

    // --- Audio decoder + output (optional) ---
    const int aIdx = m_demuxer->getAudioStreamIndex();
    if (aIdx >= 0) {
        const AVStream * aStream = fmt->streams[aIdx];

        m_audioDecoder = new AudioDecoder(m_audioPacketQueue, m_audioFrameQueue, aStream->codecpar, this);

        m_audioOutput = new AudioOutput(m_audioFrameQueue, this);

        if (m_audioDecoder->initDecoder() && m_audioOutput->init()) {
            // Give VideoWidget the audio clock as sync master
            m_videoWidget->setAudioClock(m_audioOutput);
            qDebug() << "OpenGLWidget: audio pipeline ready";
        } else {
            qWarning() << "OpenGLWidget: audio init failed, falling back to wall clock";
            // Clean up failed audio objects; video-only playback continues
            delete m_audioDecoder; m_audioDecoder = nullptr;
            delete m_audioOutput;  m_audioOutput  = nullptr;
        }
    }

    // --- Start all threads ---
    m_demuxer->start();
    m_videoDecoder->start();
    if (m_audioDecoder) m_audioDecoder->start();
    if (m_audioOutput)  m_audioOutput->start();

    play();
}

void OpenGLWidget::play()
{
    if (!m_isPlaying) {
        if (m_audioOutput) m_audioOutput->setPaused(false);
        m_videoWidget->play();
        m_isPlaying = true;
        m_state = Playing;
        emit playbackStateChanged(m_state);
    }
}

void OpenGLWidget::pause()
{
    if (m_state == Playing) {
        m_videoWidget->pause();
        if (m_audioOutput) m_audioOutput->setPaused(true);
        m_state = Paused;
        m_isPlaying = false;
        emit playbackStateChanged(m_state);
    }
}

void OpenGLWidget::stop()
{
    if (m_state != Stopped) {
        m_state = Stopped;
        pause();
        cleanupPipeline();
        m_position = 0;
        m_duration = 0;

        emit playbackStateChanged(m_state);
    }
}

void OpenGLWidget::seek(const int seconds) const
{
    if (!m_demuxer) return;

    // Reset video sync clock — re-anchors on next incoming frame
    m_videoWidget->resetClock();

    // Flush WASAPI buffer and reset IAudioClock to 0
    if (m_audioOutput) m_audioOutput->requestFlush();

    m_demuxer->seekTo(seconds);
}

void OpenGLWidget::setPlaybackRate(const double rate)
{
    if (rate <= 0.0) return;
    m_playbackRate = rate;

    //if (m_videoWidget) m_videoWidget->setPlaybackRate(m_playbackRate);
    //if (m_audioOutput) m_audioOutput->setPlaybackRate(m_playbackRate);
}

bool OpenGLWidget::isPlaying() const { return m_isPlaying; }
int  OpenGLWidget::duration()  const { return m_duration; }
int  OpenGLWidget::position()  const { return m_position; }

// ---------------------------------------------------------------------------
// Private slots
// ---------------------------------------------------------------------------

void OpenGLWidget::onPositionChanged(const int seconds)
{
    m_position = seconds;
    emit positionChanged(seconds);
}

void OpenGLWidget::onVideoDimensionsFound(const int width, const int height)
{
    emit videoSizeChanged(width, height);
}

// ---------------------------------------------------------------------------
// Private helpers
// ---------------------------------------------------------------------------

void OpenGLWidget::initializePipeline()
{
    m_videoPacketQueue = new PacketQueue(200);
    m_videoFrameQueue  = new FrameQueue(15);
    m_audioPacketQueue = new PacketQueue(200);
    m_audioFrameQueue  = new FrameQueue(60);   // larger: audio frames are small
}

void OpenGLWidget::cleanupPipeline()
{
    // 1. Stop render timer and audio output before touching queues
    m_videoWidget->pause();
    if (m_audioOutput) m_audioOutput->setPaused(true);

    // 2. Abort queues to unblock any blocking push/pop
    if (m_videoPacketQueue) m_videoPacketQueue->abort();
    if (m_videoFrameQueue)  m_videoFrameQueue->abort();
    if (m_audioPacketQueue) m_audioPacketQueue->abort();
    if (m_audioFrameQueue)  m_audioFrameQueue->abort();

    // 3. Stop threads (order: demuxer first, then decoders, then output)
    auto stopThread = [](QThread* t) {
        if (!t) return;
        t->requestInterruption();
        t->quit();
        t->wait();
    };

    stopThread(m_demuxer);      delete m_demuxer;       m_demuxer       = nullptr;
    stopThread(m_videoDecoder); delete m_videoDecoder;  m_videoDecoder  = nullptr;
    stopThread(m_audioDecoder); delete m_audioDecoder;  m_audioDecoder  = nullptr;
    stopThread(m_audioOutput);  delete m_audioOutput;   m_audioOutput   = nullptr;

    // 4. Delete queues — fresh ones created in next initQueues()
    delete m_videoPacketQueue; m_videoPacketQueue = nullptr;
    delete m_videoFrameQueue;  m_videoFrameQueue  = nullptr;
    delete m_audioPacketQueue; m_audioPacketQueue = nullptr;
    delete m_audioFrameQueue;  m_audioFrameQueue  = nullptr;

    m_isPlaying = false;
}
