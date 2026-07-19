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

#include "libvura/media-engine/audio-decoder.h"


#include <QWidget>
#include <QString>
#include <QVBoxLayout>

extern "C" {
#include <libavformat/avformat.h>
}

class VideoWidget;
class DemuxerThread;
class DecoderThread;
class AudioOutput;
class PacketQueue;
class FrameQueue;

enum PlaybackState
{
    Stopped,
    Playing,
    Paused
};

class VuraMediaEngine : public QWidget {
    Q_OBJECT

public:
    explicit VuraMediaEngine(QWidget *parent = nullptr);
    ~VuraMediaEngine() override;

    PlaybackState currentState() const { return m_state; }

    bool isPlaying() const;
    int duration() const;
    int position() const;

public slots:
    void loadMedia(const QString& filePath);
    void loadMedia(const QUrl& url);
    void play();
    void pause();
    void stop();
    void seek(int seconds) const;
    void setPlaybackRate(double rate);

signals:
    void videoSizeChanged(int width, int height);
    void mediaFinished();
    void positionChanged(qint64 positionMs);
    void durationChanged(qint64 durationMs);
    void playbackStateChanged(PlaybackState state);
    void errorOccurred(const QString& errorMessage);

private slots:
    void onPositionChanged(int seconds);
    void onVideoDimensionsFound(int width, int height);

private:
    void initializePipeline();
    void cleanupPipeline();

    // Video Pipeline
    PacketQueue* m_videoPacketQueue = nullptr;
    FrameQueue* m_videoFrameQueue = nullptr;
    DecoderThread* m_videoDecoder = nullptr;

    // Audio Pipeline
    PacketQueue* m_audioPacketQueue = nullptr;
    FrameQueue* m_audioFrameQueue = nullptr;
    AudioDecoder* m_audioDecoder = nullptr;
    AudioOutput* m_audioOutput = nullptr;

    // Shared
    DemuxerThread* m_demuxer = nullptr;
    VideoWidget* m_videoWidget = nullptr;

    QVBoxLayout* m_layout;
    PlaybackState m_state;

    bool m_isPlaying = false;
    int m_duration = 0;
    int m_position = 0;
    double m_playbackRate = 1.0;

};
