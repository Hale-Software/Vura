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

#include "decoder-thread.h"
#include <QDebug>

DecoderThread::DecoderThread(PacketQueue* packetQueue, FrameQueue* frameQueue, AVCodecParameters* codecParams, QObject *parent)
    : QThread(parent),
      packetQueue(packetQueue),
      frameQueue(frameQueue),
      codecParams(codecParams),
      codecContext(nullptr),
      abortRequested(false) {}

DecoderThread::~DecoderThread()
{
    stop();
    if (codecContext) {
        avcodec_free_context(&codecContext);
    }
}

bool DecoderThread::initDecoder()
{
    // Find the specific decoder for the video stream
    const AVCodec* codec = avcodec_find_decoder(codecParams->codec_id);
    if (!codec) {
        qWarning() << "Unsupported codec!";
        return false;
    }

    // Allocate the codec context
    codecContext = avcodec_alloc_context3(codec);
    if (!codecContext) {
        qWarning() << "Failed to allocate codec context.";
        return false;
    }

    // Copy parameters from the demuxer's stream to the codec context
    if (avcodec_parameters_to_context(codecContext, codecParams) < 0) {
        qWarning() << "Failed to copy codec parameters.";
        return false;
    }

    // Open the decoder
    if (avcodec_open2(codecContext, codec, nullptr) < 0) {
        qWarning() << "Failed to open codec.";
        return false;
    }

    return true;
}

void DecoderThread::stop()
{
    requestInterruption();
    frameQueue->abort();
    packetQueue->abort();
    wait();
}

void DecoderThread::run()
{
    while (!isInterruptionRequested()) {
        AVPacket* packet = packetQueue->pop();
        if (!packet) continue;

        if (packet == PacketQueue::getFlushPacket()) {
            avcodec_flush_buffers(codecContext);
            frameQueue->clear();
            continue;
        }

        if (!packet->data || packet->size <= 0) {
            av_packet_free(&packet);
            continue;
        }

        if (avcodec_send_packet(codecContext, packet) >= 0) {
            AVFrame* frame = av_frame_alloc();
            while (avcodec_receive_frame(codecContext, frame) == 0) {
                if (isInterruptionRequested()) {
                    av_frame_free(&frame);
                    break;
                }
                frameQueue->push(frame); // Ownership transferred to queue
                frame = av_frame_alloc(); // Prepare for next
            }
            av_frame_free(&frame); // Free the last unused allocation
        }
        av_packet_free(&packet);
    }
}
