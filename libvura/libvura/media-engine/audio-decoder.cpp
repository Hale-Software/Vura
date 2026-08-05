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

#include "audio-decoder.h"


AudioDecoder::AudioDecoder(PacketQueue* packetQueue, FrameQueue* frameQueue, AVCodecParameters* codecParams, QObject* parent)
    : QThread(parent), m_packetQueue(packetQueue), m_frameQueue(frameQueue), m_codecParams(codecParams) {}

AudioDecoder::~AudioDecoder()
{
    requestInterruption();
    m_packetQueue->abort();
    m_frameQueue->abort();
    wait();

    if (m_swrContext) { swr_free(&m_swrContext); }
    if (m_codecContext) { avcodec_free_context(&m_codecContext); }
}

bool AudioDecoder::initDecoder()
{
    const AVCodec* codec = avcodec_find_decoder(m_codecParams->codec_id);
    if (!codec) {
        qWarning() << "AudioDecoder: unsupported codec";
        return false;
    }

    m_codecContext = avcodec_alloc_context3(codec);
    if (!m_codecContext) {
        qWarning() << "AudioDecoder: failed to alloc codec context";
        return false;
    }

    if (avcodec_parameters_to_context(m_codecContext, m_codecParams) < 0) {
        qWarning() << "AudioDecoder: failed to copy codec params";
        return false;
    }

    if (avcodec_open2(m_codecContext, codec, nullptr) < 0) {
        qWarning() << "AudioDecoder: failed to open codec";
        return false;
    }

    return initResampler();
}

bool AudioDecoder::initResampler()
{
    // Input layout: take it from the codec context.
    // FFmpeg 5.1+ uses AVChannelLayout; we use the legacy ch_layout field
    // which is available in both old and new API.
    constexpr AVChannelLayout outLayout = AV_CHANNEL_LAYOUT_STEREO;

    const int ret = swr_alloc_set_opts2(
        &m_swrContext,
        &outLayout,                    // output channel layout
        k_outFmt,                      // output sample format (S16)
        k_outSampleRate,               // output sample rate (48000)
        &m_codecContext->ch_layout,    // input channel layout
        m_codecContext->sample_fmt,    // input sample format
        m_codecContext->sample_rate,   // input sample rate
        0, nullptr
    );

    if (ret < 0 || !m_swrContext) {
        qWarning() << "AudioDecoder: swr_alloc_set_opts2 failed";
        return false;
    }

    if (swr_init(m_swrContext) < 0) {
        qWarning() << "AudioDecoder: swr_init failed";
        return false;
    }

    qDebug() << "AudioDecoder: resampler"
             << m_codecContext->sample_rate << "Hz"
             << av_get_sample_fmt_name(m_codecContext->sample_fmt)
             << "->" << k_outSampleRate << "Hz S16 stereo";

    return true;
}

AVFrame* AudioDecoder::resampleFrame(const AVFrame * input) const
{
    AVFrame* out = av_frame_alloc();
    out->format         = k_outFmt;
    out->sample_rate    = k_outSampleRate;
    out->ch_layout      = AV_CHANNEL_LAYOUT_STEREO;

    // Calculate how many output samples we'll produce
    const int outSamples = static_cast<int>(
        av_rescale_rnd(swr_get_delay(m_swrContext, input->sample_rate) + input->nb_samples,
                       k_outSampleRate, input->sample_rate, AV_ROUND_UP)
    );
    out->nb_samples = outSamples;

    // Allocate the output buffer
    if (av_frame_get_buffer(out, 0) < 0) {
        qWarning() << "AudioDecoder: av_frame_get_buffer failed";
        av_frame_free(&out);
        return nullptr;
    }

    // Convert
    const int converted = swr_convert(
        m_swrContext,
        out->data, outSamples,
        const_cast<const uint8_t**>(input->data), input->nb_samples
    );

    if (converted < 0) {
        qWarning() << "AudioDecoder: swr_convert failed";
        av_frame_free(&out);
        return nullptr;
    }

    out->nb_samples = converted;

    // Carry the PTS through (rescaled to the output sample rate's timebase)
    // so AudioOutput's IAudioClock can be cross-checked if needed.
    out->pts = av_rescale_q(input->pts, m_codecContext->time_base, AVRational{1, k_outSampleRate});

    return out;
}

void AudioDecoder::run()
{
    while (!isInterruptionRequested()) {
        AVPacket* packet = m_packetQueue->pop();
        if (!packet) continue;

        // Flush signal from demuxer after a seek
        if (packet == PacketQueue::getFlushPacket()) {
            avcodec_flush_buffers(m_codecContext);
            swr_init(m_swrContext); // Reset resampler internal state
            m_frameQueue->clear();
            continue;
        }

        if (!packet->data || packet->size <= 0) {
            av_packet_free(&packet);
            continue;
        }

        const int ret = avcodec_send_packet(m_codecContext, packet);
        av_packet_free(&packet);

        if (ret < 0 && ret != AVERROR(EAGAIN)) {
            qWarning() << "AudioDecoder: avcodec_send_packet error" << ret;
            continue;
        }

        AVFrame* decoded = av_frame_alloc();
        while (avcodec_receive_frame(m_codecContext, decoded) == 0) {
            if (isInterruptionRequested()) break;

            AVFrame* resampled = resampleFrame(decoded);
            if (resampled) {
                if (!m_frameQueue->push(resampled)) {
                    av_frame_free(&resampled);
                    break;
                }
            }
        }
        av_frame_free(&decoded);
    }
}
