#include "ThumbnailWorker.h"

// ThumbnailWorker.cpp

ThumbnailWorker::ThumbnailWorker(const QString &filePath, QObject *parent)
    : QThread(parent), m_filePath(filePath), m_hasNewRequest(false) {}

ThumbnailWorker::~ThumbnailWorker() {}


void ThumbnailWorker::requestThumbnail(int64_t timestamp) {
    QMutexLocker locker(&m_mutex);
    m_targetTimestamp = timestamp;
    m_hasNewRequest = true; // Flag that a new request replaced the old one
}

void ThumbnailWorker::run() {
    // 1. Initialize a localized FFmpeg Context specifically for thumbnails
    AVFormatContext* formatCtx = avformat_alloc_context();
    avformat_open_input(&formatCtx, m_filePath.toUtf8().constData(), nullptr, nullptr);
    avformat_find_stream_info(formatCtx, nullptr);

    // --- NEW: FIND THE VIDEO STREAM INDEX ---
    int videoStreamIndex = -1;
    AVCodecParameters* codecParams = nullptr;
    const AVCodec* codec = nullptr;

    for (unsigned int i = 0; i < formatCtx->nb_streams; i++) {
        if (formatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStreamIndex = i;
            codecParams = formatCtx->streams[i]->codecpar;
            break;
        }
    }

    if (videoStreamIndex == -1) {
        // No video stream found, safely exit thread
        avformat_close_input(&formatCtx);
        return;
    }

    // --- NEW: SETUP THE CODEC CONTEXT (codecCtx) ---
    codec = avcodec_find_decoder(codecParams->codec_id);
    AVCodecContext* codecCtx = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(codecCtx, codecParams);
    avcodec_open2(codecCtx, codec, nullptr);

    // --- NEW: SETUP THE SCALER CONTEXT (swsCtx) ---
    // This scales the original video resolution down to our 160x90 thumbnail
    // and converts the color from YUV to RGB24 so Qt can read it.
    int thumbWidth = 160;
    int thumbHeight = 90;

    SwsContext* swsCtx = sws_getContext(
        codecCtx->width, codecCtx->height, codecCtx->pix_fmt,  // Source: Original video size & format
        thumbWidth, thumbHeight, AV_PIX_FMT_RGB24,             // Target: 160x90 RGB
        SWS_BILINEAR, nullptr, nullptr, nullptr                // Scaling algorithm
    );

    while (!m_abort) {
        int64_t seekTarget = -1;

        // Safely check if we have a new timestamp to seek to
        {
            QMutexLocker locker(&m_mutex);
            if (m_hasNewRequest) {
                seekTarget = m_targetTimestamp;
                m_hasNewRequest = false;
            }
        }

        if (seekTarget != -1) {
            // 2. Seek to the requested timestamp
            av_seek_frame(formatCtx, videoStreamIndex, seekTarget, AVSEEK_FLAG_BACKWARD);
            avcodec_flush_buffers(codecCtx); // Crucial: clear old decoder state

            // 3. Read and decode the exact frame
            AVPacket* packet = av_packet_alloc();
            AVFrame* frame = av_frame_alloc();
            AVFrame* rgbFrame = av_frame_alloc(); // For sws_scale output

            bool frameDecoded = false;

            while (av_read_frame(formatCtx, packet) >= 0 && !frameDecoded && !m_hasNewRequest) {
                if (packet->stream_index == videoStreamIndex) {
                    avcodec_send_packet(codecCtx, packet);
                    if (avcodec_receive_frame(codecCtx, frame) >= 0) {

                        // 4. Convert YUV to RGB using sws_scale
                        sws_scale(swsCtx, frame->data, frame->linesize, 0, codecCtx->height,
                                  rgbFrame->data, rgbFrame->linesize);

                        // 5. Wrap the raw bytes in a QImage
                        // Assuming you scaled to exactly 160x90 for the thumbnail
                        QImage thumb(rgbFrame->data[0], 160, 90, rgbFrame->linesize[0], QImage::Format_RGB888);

                        // 6. Emit a deep copy of the image back to the UI thread
                        emit thumbnailReady(thumb.copy(), seekTarget);
                        frameDecoded = true;
                    }
                }
                av_packet_unref(packet);
            }
            av_frame_free(&frame);
            av_frame_free(&rgbFrame);
            av_packet_free(&packet);
        } else {
            // Sleep briefly to prevent 100% CPU usage while waiting for mouse movement
            msleep(10);
        }
    }

    // ... (Cleanup FFmpeg contexts) ...
}

