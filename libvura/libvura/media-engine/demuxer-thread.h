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

#include <QThread>
#include <QString>
#include <QDebug>

#include "packet-queue.h"

#include <atomic>

extern "C" {
#include <libavformat/avformat.h>
}

class DemuxerThread : public QThread {
    Q_OBJECT

public:
    /**
     * Pass both queues at construction time.
     * audioQueue may be nullptr if the caller doesn't want audio
     * (e.g. video-only file, or audio not yet initialised).
     * The demuxer will simply discard audio packets in that case.
     */
    explicit DemuxerThread(PacketQueue* videoQueue,
                           PacketQueue* audioQueue = nullptr,
                           QObject*     parent     = nullptr);
    ~DemuxerThread() override;

    bool openFile(const QString& filePath);
    void stop();

    int              getVideoStreamIndex() const { return m_videoStreamIndex; }
    int              getAudioStreamIndex() const { return m_audioStreamIndex; }
    AVFormatContext* getFormatContext()    const { return m_formatContext; }
    int64_t          getDurationSeconds()  const;
    AVRational       getTimeBase()         const;
    void             seekTo(int seconds);

    signals:
        void flushDecoderRequested();
    void videoDimentionsFound(int width, int height);

protected:
    void run() override;

private:
    QString          m_currentFile;
    PacketQueue*     m_videoQueue        = nullptr;
    PacketQueue*     m_audioQueue        = nullptr;   // may be nullptr

    AVFormatContext* m_formatContext     = nullptr;
    int              m_videoStreamIndex  = -1;
    int              m_audioStreamIndex  = -1;

    std::atomic<bool>    m_seekRequested{false};
    std::atomic<int64_t> m_seekTargetTs{0};

};
