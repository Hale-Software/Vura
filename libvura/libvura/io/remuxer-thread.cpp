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

#include "remuxer-thread.h"


RemuxerThread::RemuxerThread(const QString &inputFile, const QString &outputFile, QObject *parent)
    : QThread(parent), m_inputFile(inputFile), m_outputFile(outputFile) {}

RemuxerThread::~RemuxerThread()
{
    cancel();
    wait();
}

void RemuxerThread::cancel()
{
    m_cancelRequested = true;
}

void RemuxerThread::run()
{
    AVFormatContext *inputCtx = nullptr;
    AVFormatContext *outputCtx = nullptr;
    AVPacket *packet = av_packet_alloc();

    // Open input file
    if (avformat_open_input(&inputCtx, m_inputFile.toStdString().c_str(), nullptr, nullptr) < 0) {
        emit conversionFinished(false, "Failed to open input file.");
        return;
    }
    avformat_find_stream_info(inputCtx, nullptr);

    // Allocate output context
    avformat_alloc_output_context2(&outputCtx, nullptr, nullptr, m_outputFile.toStdString().c_str());
    if (!outputCtx) {
        emit conversionFinished(false, "Failed to allocate output context.");
        avformat_close_input(&inputCtx);
        return;
    }

    // Map streams (copy codec parameters from input to output)
    std::vector<int> streamMapping(inputCtx->nb_streams, -1);
    int streamIndex = 0;

    for (unsigned int i = 0; i < inputCtx->nb_streams; i++) {
        const AVStream *inStream = inputCtx->streams[i];
        const AVCodecParameters *inCodecPar = inStream->codecpar;

        // Only copy video, audio, and subtitles
        if (inCodecPar->codec_type != AVMEDIA_TYPE_VIDEO &&
            inCodecPar->codec_type != AVMEDIA_TYPE_AUDIO &&
            inCodecPar->codec_type != AVMEDIA_TYPE_SUBTITLE) {
            continue;
        }

        streamMapping[i] = streamIndex++;
        const AVStream *outStream = avformat_new_stream(outputCtx, nullptr);
        avcodec_parameters_copy(outStream->codecpar, inCodecPar);
        outStream->codecpar->codec_tag = 0; // Let FFmpeg choose the correct tag for the new format
    }

    // Open output file
    if (!(outputCtx->oformat->flags & AVFMT_NOFILE)) {
        avio_open(&outputCtx->pb, m_outputFile.toStdString().c_str(), AVIO_FLAG_WRITE);
    }

    // Write file header
    const int ret = avformat_write_header(outputCtx, nullptr);
    if (ret < 0) {
        char errbuf[AV_ERROR_MAX_STRING_SIZE] = {};
        av_strerror(ret, errbuf, sizeof(errbuf));

        emit conversionFinished(false, QString("Failed to write output header: %1").arg(errbuf));

        if (!(outputCtx->oformat->flags & AVFMT_NOFILE)) {
            avio_closep(&outputCtx->pb);
        }

        avformat_free_context(outputCtx);
        avformat_close_input(&inputCtx);
        av_packet_free(&packet);
        return;
    }

    const int64_t duration = inputCtx->duration;

    // Read packets, rescale timestamps, and write to output
    while (av_read_frame(inputCtx, packet) >= 0 && !m_cancelRequested) {
        const AVStream *inStream = inputCtx->streams[packet->stream_index];
        const int outStreamIndex = streamMapping[packet->stream_index];

        if (outStreamIndex >= 0) {
            const AVStream *outStream = outputCtx->streams[outStreamIndex];
            packet->stream_index = outStreamIndex;

            // Rescale timestamps to match the new containers time base
            av_packet_rescale_ts(packet, inStream->time_base, outStream->time_base);
            packet->pos = -1;

            av_interleaved_write_frame(outputCtx, packet);

            // Calculate progress percentage
            if (duration > 0 && packet->pts != AV_NOPTS_VALUE) {
                const int64_t currentPts = av_rescale_q(packet->pts, outStream->time_base, {1, AV_TIME_BASE});
                const int progress = (currentPts * 100) / duration;
                emit progressUpdated(progress);
            }
        }
        av_packet_unref(packet);
    }

    // Finalize and cleanup
    av_write_trailer(outputCtx);
    avio_closep(&outputCtx->pb);
    avformat_free_context(outputCtx);
    avformat_close_input(&inputCtx);
    av_packet_free(&packet);

    if (m_cancelRequested) {
        emit conversionFinished(false, "Conversion cancelled by user.");
    } else {
        emit progressUpdated(100);
        emit conversionFinished(true, "Conversion completed successfully!");
    }
}
