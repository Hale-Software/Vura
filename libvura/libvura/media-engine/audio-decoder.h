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
#include <QDebug>
#include "packet-queue.h"
#include "frame-queue.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <libavutil/opt.h>
#include <libavutil/channel_layout.h>
}

/**
 * AudioDecoder
 *
 * Mirrors DecoderThread but handles audio streams.
 * In addition to decoding compressed audio (AAC, AC3, etc.) it resamples
 * every decoded frame to a fixed output format that AudioOutput expects:
 *
 *   AV_SAMPLE_FMT_S16  |  48000 Hz  |  stereo (AV_CH_LAYOUT_STEREO)
 *
 * The resampled frame is pushed to m_frameQueue for AudioOutput to consume.
 *
 * libswresample handles:
 *   - Format conversion (float planar → interleaved S16, etc.)
 *   - Sample rate conversion (44100 → 48000, etc.)
 *   - Channel layout conversion (5.1 → stereo downmix, etc.)
 */
class AudioDecoder : public QThread
{
    Q_OBJECT

public:
    AudioDecoder(PacketQueue* packetQueue,
                 FrameQueue*  frameQueue,
                 AVCodecParameters* codecParams,
                 QObject* parent = nullptr);
    ~AudioDecoder() override;

    bool initDecoder();

    // Target output format constants — must match AudioOutput
    static constexpr int    k_outSampleRate = 48000;
    static constexpr int    k_outChannels   = 2;
    static constexpr AVSampleFormat k_outFmt = AV_SAMPLE_FMT_S16;

protected:
    void run() override;

private:
    bool initResampler();
    AVFrame* resampleFrame(AVFrame* input);

    PacketQueue*       m_packetQueue  = nullptr;
    FrameQueue*        m_frameQueue   = nullptr;
    AVCodecParameters* m_codecParams  = nullptr;
    AVCodecContext*    m_codecContext = nullptr;
    SwrContext*        m_swrContext   = nullptr;

};
