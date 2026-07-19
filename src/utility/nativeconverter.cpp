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

#include "nativeconverter.h"
#include <QDebug>
#include <algorithm>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
}

NativeConverter::NativeConverter(QObject *parent) : QObject(parent) {}

bool NativeConverter::convert(const QString &inputPath, const QString &outputPath, AVCodecID chosenVideoCodecId) {
    AVFormatContext *inFmtCtx = nullptr;
    AVFormatContext *outFmtCtx = nullptr;
    AVCodecContext *videoDecCtx = nullptr;
    AVCodecContext *videoEncCtx = nullptr;

    int videoStreamIndex = -1;

    std::string inStr = inputPath.toStdString();
    std::string outStr = outputPath.toStdString();

    emit statusMessage("Analyzing media streams...");
    if (avformat_open_input(&inFmtCtx, inStr.c_str(), nullptr, nullptr) < 0) {
        emit statusMessage("Error: Cannot open source file.");
        return false;
    }

    if (avformat_find_stream_info(inFmtCtx, nullptr) < 0) {
        avformat_close_input(&inFmtCtx);
        return false;
    }

    // Allocate memory map structure for our output muxing system
    avformat_alloc_output_context2(&outFmtCtx, nullptr, nullptr, outStr.c_str());
    if (!outFmtCtx) {
        emit statusMessage("Error: Failed to construct output container wrapper.");
        avformat_close_input(&inFmtCtx);
        return false;
    }

    // Array track map to translate internal input indexing directly into the output index layout
    std::vector<int> streamMap(inFmtCtx->nb_streams, -1);
    int currentOutputStreamIndex = 0;

    // Loop 1: Configure stream tracks and initialize the chosen codec pipelines
    for (unsigned int i = 0; i < inFmtCtx->nb_streams; i++) {
        AVStream *inStream = inFmtCtx->streams[i];
        AVCodecParameters *inCodecPar = inStream->codecpar;

        // Channel Type A: Handle the primary video stream tracking
        if (inCodecPar->codec_type == AVMEDIA_TYPE_VIDEO && videoStreamIndex == -1) {
            videoStreamIndex = i;

            // Setup the matching Decoder interface
            const AVCodec *decoder = avcodec_find_decoder(inCodecPar->codec_id);
            videoDecCtx = avcodec_alloc_context3(decoder);
            avcodec_parameters_to_context(videoDecCtx, inCodecPar);
            if (avcodec_open2(videoDecCtx, decoder, nullptr) < 0) {
                emit statusMessage("Error: Failed to initialize decoder context mapping.");
                return false;
            }

            // Dynamically query and allocate the custom video encoder chosen by the user
            const AVCodec *userEncoder = avcodec_find_encoder(chosenVideoCodecId);
            if (!userEncoder) {
                emit statusMessage("Error: Target selected Video Encoder not found or supported on this system.");
                return false;
            }

            videoEncCtx = avcodec_alloc_context3(userEncoder);
            videoEncCtx->height = videoDecCtx->height;
            videoEncCtx->width = videoDecCtx->width;
            videoEncCtx->sample_aspect_ratio = videoDecCtx->sample_aspect_ratio;
            videoEncCtx->pix_fmt = AV_PIX_FMT_YUV420P; // Highly compatible pixel structure representation
            videoEncCtx->time_base = inStream->time_base;

            if (outFmtCtx->oformat->flags & AVFMT_GLOBALHEADER) {
                videoEncCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
            }

            if (avcodec_open2(videoEncCtx, userEncoder, nullptr) < 0) {
                emit statusMessage("Error: User configuration rejected by the selected Video Encoder.");
                return false;
            }

            AVStream *outStream = avformat_new_stream(outFmtCtx, nullptr);
            avcodec_parameters_from_context(outStream->codecpar, videoEncCtx);

            streamMap[i] = currentOutputStreamIndex++;
        }
        // Channel Type B: Direct high-fidelity stream copy for audio lines
        else if (inCodecPar->codec_type == AVMEDIA_TYPE_AUDIO) {
            AVStream *outStream = avformat_new_stream(outFmtCtx, nullptr);
            if (!outStream) return false;

            // Pure structural byte-copy prevents synchronization loss or sample format corruption
            //avbitstream_alloc;
            avcodec_parameters_copy(outStream->codecpar, inCodecPar);
            outStream->codecpar->codec_tag = 0; // Let context recalculate container tag maps

            streamMap[i] = currentOutputStreamIndex++;
        }
    }

    // Open target file writing handle descriptors
    if (!(outFmtCtx->oformat->flags & AVFMT_NOFILE)) {
        if (avio_open(&outFmtCtx->pb, outStr.c_str(), AVIO_FLAG_WRITE) < 0) {
            emit statusMessage("Error: Target output write paths blocked.");
            return false;
        }
    }

    if (avformat_write_header(outFmtCtx, nullptr) < 0) {
        emit statusMessage("Error: Container header syntax allocation failed.");
        return false;
    }

    AVPacket *packet = av_packet_alloc();
    AVFrame *frame = av_frame_alloc();
    int64_t totalFrames = inFmtCtx->streams[videoStreamIndex]->nb_frames;
    int64_t processedFrames = 0;

    emit statusMessage("Executing dual-track muxing engine loop...");

    // Loop 2: Core conversion and interleaving operations
    while (av_read_frame(inFmtCtx, packet) >= 0) {
        int outIndex = streamMap[packet->stream_index];
        if (outIndex == -1) {
            av_packet_unref(packet);
            continue;
        }

        // Process Route A: Execute full transcode logic on the target video stream
        if (packet->stream_index == videoStreamIndex) {
            if (avcodec_send_packet(videoDecCtx, packet) >= 0) {
                while (avcodec_receive_frame(videoDecCtx, frame) >= 0) {
                    frame->pts = frame->best_effort_timestamp;

                    if (avcodec_send_frame(videoEncCtx, frame) >= 0) {
                        AVPacket *outPacket = av_packet_alloc();
                        while (avcodec_receive_packet(videoEncCtx, outPacket) >= 0) {
                            av_packet_rescale_ts(outPacket, videoEncCtx->time_base, outFmtCtx->streams[outIndex]->time_base);
                            outPacket->stream_index = outIndex;
                            av_interleaved_write_frame(outFmtCtx, outPacket);
                            av_packet_unref(outPacket);
                        }
                        av_packet_free(&outPacket);
                    }
                    processedFrames++;
                    if (totalFrames > 0) {
                        int progress = static_cast<int>((processedFrames * 100) / totalFrames);
                        emit progressUpdated(std::min(progress, 99));
                    }
                }
            }
        }
        // Process Route B: Direct write audio stream copy route
        else {
            av_packet_rescale_ts(packet, inFmtCtx->streams[packet->stream_index]->time_base, outFmtCtx->streams[outIndex]->time_base);
            packet->stream_index = outIndex;
            av_interleaved_write_frame(outFmtCtx, packet);
        }
        av_packet_unref(packet);
    }

    // Flush lingering data sequences inside the active video codec hardware buffers
    avcodec_send_frame(videoEncCtx, nullptr);
    AVPacket *flushPacket = av_packet_alloc();
    while (avcodec_receive_packet(videoEncCtx, flushPacket) >= 0) {
        int outIndex = streamMap[videoStreamIndex];
        av_packet_rescale_ts(flushPacket, videoEncCtx->time_base, outFmtCtx->streams[outIndex]->time_base);
        flushPacket->stream_index = outIndex;
        av_interleaved_write_frame(outFmtCtx, flushPacket);
        av_packet_unref(flushPacket);
    }
    av_packet_free(&flushPacket);

    av_write_trailer(outFmtCtx);
    emit progressUpdated(100);
    emit statusMessage("Processing completely finished.");

    // Resource Cleanup Free Calls
    av_packet_free(&packet);
    av_frame_free(&frame);
    avcodec_free_context(&videoDecCtx);
    avcodec_free_context(&videoEncCtx);
    avformat_close_input(&inFmtCtx);
    if (outFmtCtx && !(outFmtCtx->oformat->flags & AVFMT_NOFILE)) {
        avio_closep(&outFmtCtx->pb);
    }
    avformat_free_context(outFmtCtx);

    return true;
}
