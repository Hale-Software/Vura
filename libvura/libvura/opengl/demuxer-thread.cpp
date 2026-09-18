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

#include "demuxer-thread.h"

DemuxerThread::DemuxerThread(PacketQueue* videoQueue, PacketQueue* audioQueue, QObject* parent)
    : QThread(parent),
      m_videoQueue(videoQueue),
      m_audioQueue(audioQueue) {}

DemuxerThread::~DemuxerThread()
{
    requestInterruption();
    // Unblock any queue waits so run() can exit
    if (m_videoQueue) m_videoQueue->abort();
    if (m_audioQueue) m_audioQueue->abort();
    wait();

    if (m_formatContext)
        avformat_close_input(&m_formatContext);
}

bool DemuxerThread::openFile(const QString& filePath)
{
    if (m_formatContext)
        avformat_close_input(&m_formatContext);

    m_currentFile = filePath;
    m_formatContext = avformat_alloc_context();

    if (avformat_open_input(&m_formatContext, m_currentFile.toStdString().c_str(), nullptr, nullptr) != 0) {
        qWarning() << "DemuxerThread: could not open" << m_currentFile;
        return false;
    }

    if (avformat_find_stream_info(m_formatContext, nullptr) < 0) {
        qWarning() << "DemuxerThread: could not find stream info";
        return false;
    }

    // Find best video stream
    m_videoStreamIndex = av_find_best_stream(m_formatContext, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    if (m_videoStreamIndex < 0) {
        qWarning() << "DemuxerThread: no video stream found";
        return false;
    }

    // Find best audio stream (optional — files may be video-only)
    m_audioStreamIndex = av_find_best_stream(m_formatContext, AVMEDIA_TYPE_AUDIO, -1, m_videoStreamIndex, nullptr, 0);
    if (m_audioStreamIndex < 0)
        qDebug() << "DemuxerThread: no audio stream (video-only file)";
    else
        qDebug() << "DemuxerThread: audio stream index" << m_audioStreamIndex;

    // Emit video dimensions for the UI
    const AVCodecParameters * vpar = m_formatContext->streams[m_videoStreamIndex]->codecpar;
    emit videoDimentionsFound(vpar->width, vpar->height);

    return true;
}

void DemuxerThread::stop()
{
    requestInterruption();
    if (m_videoQueue) m_videoQueue->abort();
    if (m_audioQueue) m_audioQueue->abort();
    wait();
}

void DemuxerThread::run()
{
    while (!isInterruptionRequested()) {

        // --- Seek handling ---
        if (m_seekRequested.exchange(false)) {
            const int64_t target = m_seekTargetTs.load();

            avformat_seek_file(m_formatContext, -1, INT64_MIN, target, INT64_MAX, AVSEEK_FLAG_BACKWARD);

            // Flush both queues and send flush sentinels to both decoders
            if (m_videoQueue) {
                m_videoQueue->clear();
                m_videoQueue->push(PacketQueue::getFlushPacket());
            }
            if (m_audioQueue) {
                m_audioQueue->clear();
                m_audioQueue->push(PacketQueue::getFlushPacket());
            }

            emit flushDecoderRequested();
        }

        // --- Read next packet ---
        AVPacket* pkt = av_packet_alloc();
        const int ret = av_read_frame(m_formatContext, pkt);

        if (ret < 0) {
            av_packet_free(&pkt);
            if (ret == AVERROR_EOF)
                qDebug() << "DemuxerThread: end of file";
            else
                qWarning() << "DemuxerThread: read error" << ret;
            break;
        }

        // --- Route by stream index ---
        if (pkt->stream_index == m_videoStreamIndex) {
            if (!m_videoQueue || !m_videoQueue->push(pkt))
                av_packet_free(&pkt);

        } else if (pkt->stream_index == m_audioStreamIndex && m_audioQueue) {
            if (!m_audioQueue->push(pkt))
                av_packet_free(&pkt);

        } else {
            // Subtitle, data, or unknown stream — discard
            av_packet_free(&pkt);
        }
    }
}

int64_t DemuxerThread::getDurationSeconds() const
{
    if (m_formatContext && m_formatContext->duration != AV_NOPTS_VALUE)
        return m_formatContext->duration / AV_TIME_BASE;
    return 0;
}

AVRational DemuxerThread::getTimeBase() const
{
    if (m_formatContext && m_videoStreamIndex >= 0)
        return m_formatContext->streams[m_videoStreamIndex]->time_base;
    return {1, 1};
}

void DemuxerThread::seekTo(const int seconds)
{
    m_seekTargetTs  = static_cast<int64_t>(seconds) * AV_TIME_BASE;
    m_seekRequested = true;
}
