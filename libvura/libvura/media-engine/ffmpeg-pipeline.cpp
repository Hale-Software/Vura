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

#include "ffmpeg-pipeline.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/channel_layout.h>
#include <libavutil/pixdesc.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
}

#ifdef VURA_HAVE_QTMULTIMEDIA
#include <QAudioDevice>
#include <QAudioFormat>
#include <QAudioSink>
#include <QIODevice>
#include <QMediaDevices>
#endif

#include <QCoreApplication>
#include <QDebug>

#include <chrono>
#include <cmath>
#include <condition_variable>
#include <cstring>
#include <deque>
#include <limits>
#include <mutex>
#include <thread>

// The channel-layout API used below arrived in FFmpeg 5.1.
static_assert(LIBAVUTIL_VERSION_INT >= AV_VERSION_INT(57, 28, 100), "FFmpeg 5.1 or newer is required");

namespace media::ffmpeg {

VideoFrame::~VideoFrame()
{
    av_frame_free(&frame);
}

namespace {

constexpr size_t kMaxPacketBytes = 16 * 1024 * 1024; // demux read-ahead ceiling
constexpr int kEnoughPackets = 50;                    // per stream, before pausing reads
constexpr size_t kMaxVideoFrames = 6;                 // decoded pictures held ahead
constexpr double kNaN = std::numeric_limits<double>::quiet_NaN();
constexpr double kNoSkip = -std::numeric_limits<double>::infinity();

QString tr(const char *text)
{
    return QCoreApplication::translate("media", text);
}

QString avErrorString(int err)
{
    char buf[AV_ERROR_MAX_STRING_SIZE] = {};
    av_strerror(err, buf, sizeof buf);
    return QString::fromUtf8(buf);
}

double toSeconds(int64_t ts, AVRational tb)
{
    return ts == AV_NOPTS_VALUE ? kNaN : double(ts) * av_q2d(tb);
}

QString tag(AVDictionary *dict, const char *key)
{
    const AVDictionaryEntry *e = av_dict_get(dict, key, nullptr, 0);
    return e ? QString::fromUtf8(e->value) : QString();
}

// ------------------------------------------------------------- clock ----

/// A timestamp that advances with wall time while running. Audio re-anchors
/// it every time the device pulls data, so it tracks what is actually
/// audible; without audio it simply free-runs from the last seek.
class Clock
{
public:
    double get() const
    {
        std::lock_guard lock(m_mutex);
        if (std::isnan(m_base))
            return kNaN;
        if (m_paused)
            return m_base;
        return m_base + secondsSince(m_anchor);
    }

    void set(double pts)
    {
        std::lock_guard lock(m_mutex);
        m_base = pts;
        m_anchor = Steady::now();
    }

    void setPaused(bool paused)
    {
        std::lock_guard lock(m_mutex);
        if (!std::isnan(m_base) && !m_paused)
            m_base += secondsSince(m_anchor);
        m_anchor = Steady::now();
        m_paused = paused;
    }

    void invalidate()
    {
        std::lock_guard lock(m_mutex);
        m_base = kNaN;
    }

private:
    using Steady = std::chrono::steady_clock;
    static double secondsSince(Steady::time_point t)
    {
        return std::chrono::duration<double>(Steady::now() - t).count();
    }

    mutable std::mutex m_mutex;
    double m_base = kNaN;
    Steady::time_point m_anchor = Steady::now();
    bool m_paused = true;
};

// ------------------------------------------------------ packet queue ----

struct Packet
{
    enum Kind { Data, Flush, Eof };
    AVPacket *pkt = nullptr;
    int serial = 0;
    Kind kind = Data;
};

class PacketQueue
{
public:
    ~PacketQueue() { clear(); }

    void push(Packet p)
    {
        std::lock_guard lock(m_mutex);
        if (p.pkt)
            m_bytes += size_t(p.pkt->size);
        m_queue.push_back(p);
        m_cv.notify_one();
    }

    /// Blocks until a packet arrives; false once aborted.
    bool pop(Packet &out)
    {
        std::unique_lock lock(m_mutex);
        m_cv.wait(lock, [this] { return m_abort || !m_queue.empty(); });
        if (m_abort)
            return false;
        out = m_queue.front();
        m_queue.pop_front();
        if (out.pkt)
            m_bytes -= size_t(out.pkt->size);
        return true;
    }

    void clear()
    {
        std::lock_guard lock(m_mutex);
        for (Packet &p : m_queue)
            av_packet_free(&p.pkt);
        m_queue.clear();
        m_bytes = 0;
    }

    void abort()
    {
        std::lock_guard lock(m_mutex);
        m_abort = true;
        m_cv.notify_all();
    }

    size_t bytes() const
    {
        std::lock_guard lock(m_mutex);
        return m_bytes;
    }

    int count() const
    {
        std::lock_guard lock(m_mutex);
        return int(m_queue.size());
    }

private:
    mutable std::mutex m_mutex;
    std::condition_variable m_cv;
    std::deque<Packet> m_queue;
    size_t m_bytes = 0;
    bool m_abort = false;
};

// ------------------------------------------------------- frame queue ----

class FrameQueue
{
public:
    /// Blocks while full. Gives up (dropping the frame) on abort or when a
    /// seek has made the frame stale.
    bool push(VideoFramePtr frame, const std::atomic<int> &currentSerial)
    {
        std::unique_lock lock(m_mutex);
        m_cv.wait(lock, [&] {
            return m_abort || m_queue.size() < kMaxVideoFrames || frame->serial != currentSerial.load();
        });
        if (m_abort || frame->serial != currentSerial.load())
            return false;
        m_queue.push_back(std::move(frame));
        return true;
    }

    VideoFramePtr take(double clock, bool haveCurrent, int serial)
    {
        std::lock_guard lock(m_mutex);
        dropStale(serial);
        if (m_queue.empty())
            return {};

        VideoFramePtr out;
        if (!haveCurrent) {
            out = std::move(m_queue.front());
            m_queue.pop_front();
        } else if (!std::isnan(clock)) {
            // Newest frame that is due; anything older is late and skipped.
            while (!m_queue.empty() && m_queue.front()->pts <= clock) {
                out = std::move(m_queue.front());
                m_queue.pop_front();
            }
        }
        if (out)
            m_cv.notify_all();
        return out;
    }

    double nextPts(int serial)
    {
        std::lock_guard lock(m_mutex);
        dropStale(serial);
        return m_queue.empty() ? kNaN : m_queue.front()->pts;
    }

    void dropLate(double clock, int serial)
    {
        std::lock_guard lock(m_mutex);
        dropStale(serial);
        if (std::isnan(clock))
            return;
        bool dropped = false;
        while (m_queue.size() >= 2 && m_queue[1]->pts <= clock) {
            m_queue.pop_front();
            dropped = true;
        }
        if (dropped)
            m_cv.notify_all();
    }

    void clear()
    {
        std::lock_guard lock(m_mutex);
        m_queue.clear();
        m_cv.notify_all();
    }

    bool empty() const
    {
        std::lock_guard lock(m_mutex);
        return m_queue.empty();
    }

    void abort()
    {
        std::lock_guard lock(m_mutex);
        m_abort = true;
        m_cv.notify_all();
    }

private:
    void dropStale(int serial)
    {
        bool dropped = false;
        while (!m_queue.empty() && m_queue.front()->serial != serial) {
            m_queue.pop_front();
            dropped = true;
        }
        if (dropped)
            m_cv.notify_all();
    }

    mutable std::mutex m_mutex;
    std::condition_variable m_cv;
    std::deque<VideoFramePtr> m_queue;
    bool m_abort = false;
};

// ------------------------------------------------------ audio buffer ----

/// Interleaved S16 PCM waiting for the audio device, tagged with the
/// timestamp of its first sample so the device side can drive the clock.
class AudioBuffer
{
public:
    struct Chunk
    {
        QByteArray data;
        qsizetype offset = 0;
        double pts = 0.0;
        int serial = 0;
    };

    void setBytesPerSecond(int bps)
    {
        std::lock_guard lock(m_mutex);
        m_bytesPerSecond = bps;
        m_maxBytes = size_t(bps / 2); // half a second ahead is plenty
    }

    bool push(Chunk chunk, const std::atomic<int> &currentSerial)
    {
        std::unique_lock lock(m_mutex);
        m_cv.wait(lock, [&] {
            return m_abort || m_bytes < m_maxBytes || chunk.serial != currentSerial.load();
        });
        if (m_abort || chunk.serial != currentSerial.load())
            return false;
        m_bytes += size_t(chunk.data.size());
        m_queue.push_back(std::move(chunk));
        return true;
    }

    /// Copies up to @p max bytes. @p ptsEnd receives the timestamp just past
    /// the last byte copied, or NaN when nothing was.
    qint64 read(char *out, qint64 max, int serial, double &ptsEnd)
    {
        std::lock_guard lock(m_mutex);
        ptsEnd = kNaN;
        qint64 copied = 0;
        while (copied < max && !m_queue.empty()) {
            Chunk &c = m_queue.front();
            if (c.serial != serial) {
                m_bytes -= size_t(c.data.size() - c.offset);
                m_queue.pop_front();
                continue;
            }
            const qint64 n = std::min<qint64>(max - copied, c.data.size() - c.offset);
            std::memcpy(out + copied, c.data.constData() + c.offset, size_t(n));
            c.offset += n;
            copied += n;
            m_bytes -= size_t(n);
            ptsEnd = c.pts + double(c.offset) / m_bytesPerSecond;
            if (c.offset >= c.data.size())
                m_queue.pop_front();
        }
        if (copied)
            m_cv.notify_all();
        return copied;
    }

    void clear()
    {
        std::lock_guard lock(m_mutex);
        m_queue.clear();
        m_bytes = 0;
        m_cv.notify_all();
    }

    bool empty() const
    {
        std::lock_guard lock(m_mutex);
        return m_queue.empty();
    }

    void abort()
    {
        std::lock_guard lock(m_mutex);
        m_abort = true;
        m_cv.notify_all();
    }

private:
    mutable std::mutex m_mutex;
    std::condition_variable m_cv;
    std::deque<Chunk> m_queue;
    size_t m_bytes = 0;
    size_t m_maxBytes = 96000;
    int m_bytesPerSecond = 192000;
    bool m_abort = false;
};

#ifdef VURA_HAVE_QTMULTIMEDIA
/// Pull-mode source for QAudioSink.
///
/// Records which byte ranges of its output carried which timestamps. The
/// clock later maps the sink's processedUSecs() back through this table to
/// find out what is audible right now. That is more honest than guessing
/// from bufferSize(): PulseAudio, for one, happily takes over a second of
/// audio up front while reporting a much smaller buffer.
class AudioPullDevice : public QIODevice
{
public:
    AudioPullDevice(AudioBuffer *buffer, const std::atomic<int> *serial, int bytesPerSecond)
        : m_buffer(buffer), m_serial(serial), m_bytesPerSecond(bytesPerSecond)
    {
    }

    bool isSequential() const override { return true; }
    qint64 bytesAvailable() const override { return 4096 + QIODevice::bytesAvailable(); }

    /// Call whenever the sink is (re)started: processedUSecs() restarts at 0.
    void resetTimeline()
    {
        std::lock_guard lock(m_mutex);
        m_segments.clear();
        m_written = 0;
    }

    /// Timestamp at byte @p played of the output, or NaN if that byte was
    /// padding or belongs to an earlier seek.
    double ptsAt(qint64 played, int serial)
    {
        std::lock_guard lock(m_mutex);
        while (m_segments.size() > 1 && m_segments.front().end <= played)
            m_segments.pop_front();
        if (m_segments.empty())
            return kNaN;
        const Segment &s = m_segments.front();
        if (played < s.start || played >= s.end || s.serial != serial)
            return kNaN;
        return s.pts + double(played - s.start) / m_bytesPerSecond;
    }

    /// True once playback has passed the last real sample handed over.
    bool drained(qint64 played)
    {
        std::lock_guard lock(m_mutex);
        return m_segments.empty() || played >= m_segments.back().end;
    }

protected:
    qint64 readData(char *data, qint64 maxlen) override
    {
        const int serial = m_serial->load();
        double ptsEnd = kNaN;
        const qint64 n = m_buffer->read(data, maxlen, serial, ptsEnd);
        // Underrun: pad with silence rather than returning short, which
        // would drop the sink into IdleState and stop it pulling.
        if (n < maxlen)
            std::memset(data + n, 0, size_t(maxlen - n));

        std::lock_guard lock(m_mutex);
        if (n > 0 && !std::isnan(ptsEnd))
            m_segments.push_back({m_written, m_written + n, ptsEnd - double(n) / m_bytesPerSecond, serial});
        m_written += maxlen;
        return maxlen;
    }

    qint64 writeData(const char *, qint64) override { return -1; }

private:
    struct Segment
    {
        qint64 start;
        qint64 end;
        double pts;
        int serial;
    };

    AudioBuffer *m_buffer;
    const std::atomic<int> *m_serial;
    const int m_bytesPerSecond;
    std::mutex m_mutex;
    std::deque<Segment> m_segments;
    qint64 m_written = 0;
};
#endif

} // namespace

// ============================================================== Impl ====

struct Pipeline::Impl
{
    QUrl url;
    Msec startAt = 0;
    int wantVideo = -1;
    int wantAudio = -1;
    Callbacks callbacks;

    // Owned by the demux thread until opened() fires, read-only afterwards.
    AVFormatContext *fmt = nullptr;
    AVCodecContext *vctx = nullptr;
    AVCodecContext *actx = nullptr;
    int vidx = -1;
    int aidx = -1;
    AVRational vtb{0, 1};
    AVRational atb{0, 1};
    double startTime = 0.0;
    double frameDuration = 1.0 / 25.0;
    bool audioActive = false;

    // Decoder-thread private.
    SwsContext *sws = nullptr;
    SwrContext *swr = nullptr;
    int swrInFormat = -1;
    int swrInRate = 0;
    AVChannelLayout swrInLayout{};

    std::thread demuxThread;
    std::thread videoThread;
    std::thread audioThread;

    std::atomic<bool> abort{false};
    std::atomic<int> requestedSerial{0};
    std::atomic<int> eofSerial{-1};
    std::atomic<int> videoDrainedSerial{-1};
    std::atomic<int> audioDrainedSerial{-1};
    std::atomic<double> skipUntil{kNoSkip};
    std::atomic<bool> audioMaster{false};

    std::mutex ctrlMutex;
    std::condition_variable ctrlCv;
    bool seekPending = false;
    double seekTarget = 0.0;

    PacketQueue videoQ;
    PacketQueue audioQ;
    FrameQueue frames;
    AudioBuffer audio;
    Clock clock;

    int outRate = 48000;
    static constexpr int outChannels = 2;
    bool audioOutputOk = false;
    qreal volume = 1.0;
#ifdef VURA_HAVE_QTMULTIMEDIA
    // Declared before the sink so it outlives it.
    std::unique_ptr<AudioPullDevice> device;
    std::unique_ptr<QAudioSink> sink;
#endif

    static int interruptCallback(void *opaque)
    {
        return static_cast<Impl *>(opaque)->abort.load() ? 1 : 0;
    }

    bool open(OpenResult &result, ErrorKind &kind, QString &error);
    AVCodecContext *openDecoder(int index, QString &error);
    void demuxLoop();
    void doSeek(double target, int serial);
    bool queuesFull() const;
    void videoLoop();
    void audioLoop();
    VideoFramePtr convert(AVFrame *frame);
    bool ensureResampler(const AVFrame *frame);
    void shutdown();
#ifdef VURA_HAVE_QTMULTIMEDIA
    bool sinkRunning() const;
    void startSink();
    qint64 playedBytes() const;
#endif
};

#ifdef VURA_HAVE_QTMULTIMEDIA
bool Pipeline::Impl::sinkRunning() const
{
    return sink && (sink->state() == QAudio::ActiveState || sink->state() == QAudio::IdleState);
}

void Pipeline::Impl::startSink()
{
    device->resetTimeline();
    sink->start(device.get());
    if (sink->error() != QAudio::NoError) {
        // Keep going on the wall clock rather than freezing the picture.
        qWarning() << "ffmpeg: audio output failed to start; using the system clock";
        audioMaster = false;
    }
}

qint64 Pipeline::Impl::playedBytes() const
{
    return sink->format().bytesForDuration(sink->processedUSecs());
}
#endif

bool Pipeline::Impl::open(OpenResult &result, ErrorKind &kind, QString &error)
{
    const bool local = url.isLocalFile();
    const QByteArray location = local ? url.toLocalFile().toUtf8() : url.toString(QUrl::FullyEncoded).toUtf8();

    fmt = avformat_alloc_context();
    fmt->interrupt_callback.callback = &Impl::interruptCallback;
    fmt->interrupt_callback.opaque = this;

    int r = avformat_open_input(&fmt, location.constData(), nullptr, nullptr);
    if (r < 0) {
        fmt = nullptr; // freed by avformat_open_input on failure
        kind = local ? errors::OpenFailed : errors::Network;
        error = tr("Could not open %1: %2").arg(url.toDisplayString(), avErrorString(r));
        return false;
    }

    r = avformat_find_stream_info(fmt, nullptr);
    if (r < 0) {
        kind = errors::Unsupported;
        error = tr("Could not read stream information: %1").arg(avErrorString(r));
        return false;
    }

    startTime = fmt->start_time != AV_NOPTS_VALUE ? double(fmt->start_time) / AV_TIME_BASE : 0.0;

    for (unsigned i = 0; i < fmt->nb_streams; ++i) {
        AVStream *st = fmt->streams[i];
        StreamInfo info;
        info.index = int(i);
        switch (st->codecpar->codec_type) {
        case AVMEDIA_TYPE_VIDEO:
            if (st->disposition & AV_DISPOSITION_ATTACHED_PIC)
                continue; // cover art, not a playable track
            info.type = TrackType::Video;
            break;
        case AVMEDIA_TYPE_AUDIO:
            info.type = TrackType::Audio;
            break;
        case AVMEDIA_TYPE_SUBTITLE:
            info.type = TrackType::Subtitle;
            break;
        default:
            continue;
        }
        info.title = tag(st->metadata, "title");
        info.language = tag(st->metadata, "language");
        info.codec = QString::fromUtf8(avcodec_get_name(st->codecpar->codec_id));
        result.streams.append(info);
    }

    auto isType = [this](int index, AVMediaType type) {
        return index >= 0 && index < int(fmt->nb_streams) && fmt->streams[index]->codecpar->codec_type == type
               && !(fmt->streams[index]->disposition & AV_DISPOSITION_ATTACHED_PIC);
    };

    vidx = isType(wantVideo, AVMEDIA_TYPE_VIDEO) ? wantVideo : av_find_best_stream(fmt, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    if (!isType(vidx, AVMEDIA_TYPE_VIDEO))
        vidx = -1;
    aidx = isType(wantAudio, AVMEDIA_TYPE_AUDIO) ? wantAudio : av_find_best_stream(fmt, AVMEDIA_TYPE_AUDIO, -1, vidx, nullptr, 0);
    if (!isType(aidx, AVMEDIA_TYPE_AUDIO))
        aidx = -1;

    if (vidx < 0 && aidx < 0) {
        kind = errors::Unsupported;
        error = tr("No playable audio or video stream.");
        return false;
    }

    QString decoderError;
    if (vidx >= 0) {
        vctx = openDecoder(vidx, decoderError);
        if (vctx) {
            AVStream *st = fmt->streams[vidx];
            vtb = st->time_base;
            const AVRational fps = av_guess_frame_rate(fmt, st, nullptr);
            if (fps.num > 0 && fps.den > 0)
                frameDuration = av_q2d(av_inv_q(fps));
        }
    }
    if (aidx >= 0 && audioOutputOk) {
        actx = openDecoder(aidx, decoderError);
        if (actx) {
            atb = fmt->streams[aidx]->time_base;
            audioActive = true;
        }
    }
    if (!vctx && !audioActive) {
        kind = errors::Unsupported;
        error = !decoderError.isEmpty() ? decoderError : tr("No audio output device is available.");
        return false;
    }

    result.videoStream = vctx ? vidx : -1;
    result.audioStream = aidx;
    result.duration = fmt->duration != AV_NOPTS_VALUE ? Msec(fmt->duration / 1000) : 0;
    result.seekable = result.duration > 0 && fmt->pb && (fmt->pb->seekable & AVIO_SEEKABLE_NORMAL);

    const QString title = tag(fmt->metadata, "title");
    if (!title.isEmpty())
        result.metadata.insert(QLatin1String(meta::Title), title);

    return true;
}

AVCodecContext *Pipeline::Impl::openDecoder(int index, QString &error)
{
    AVStream *st = fmt->streams[index];
    const AVCodec *codec = avcodec_find_decoder(st->codecpar->codec_id);
    if (!codec) {
        error = tr("No decoder for %1.").arg(QString::fromUtf8(avcodec_get_name(st->codecpar->codec_id)));
        return nullptr;
    }

    AVCodecContext *ctx = avcodec_alloc_context3(codec);
    if (!ctx || avcodec_parameters_to_context(ctx, st->codecpar) < 0) {
        avcodec_free_context(&ctx);
        error = tr("Could not set up the %1 decoder.").arg(QString::fromUtf8(codec->name));
        return nullptr;
    }
    ctx->pkt_timebase = st->time_base;
    ctx->thread_count = 0; // let FFmpeg pick

    const int r = avcodec_open2(ctx, codec, nullptr);
    if (r < 0) {
        avcodec_free_context(&ctx);
        error = tr("Could not open the %1 decoder: %2").arg(QString::fromUtf8(codec->name), avErrorString(r));
        return nullptr;
    }
    return ctx;
}

bool Pipeline::Impl::queuesFull() const
{
    if (videoQ.bytes() + audioQ.bytes() > kMaxPacketBytes)
        return true;
    const bool videoEnough = !vctx || videoQ.count() > kEnoughPackets;
    const bool audioEnough = !audioActive || audioQ.count() > kEnoughPackets;
    return videoEnough && audioEnough;
}

void Pipeline::Impl::doSeek(double target, int serial)
{
    const int64_t ts = int64_t((target + startTime) * AV_TIME_BASE);
    // max_ts = ts: land on the keyframe at or before the target, then let
    // the decoders discard up to it for a frame-accurate result.
    const int r = avformat_seek_file(fmt, -1, INT64_MIN, ts, ts, 0);
    if (r < 0)
        qWarning() << "ffmpeg: seek to" << target << "failed:" << avErrorString(r);

    skipUntil.store(target);
    videoQ.clear();
    audioQ.clear();
    if (vctx)
        videoQ.push({nullptr, serial, Packet::Flush});
    if (audioActive)
        audioQ.push({nullptr, serial, Packet::Flush});
}

void Pipeline::Impl::demuxLoop()
{
    OpenResult result;
    ErrorKind kind{};
    QString error;
    if (!open(result, kind, error)) {
        if (!abort && callbacks.failed)
            callbacks.failed(kind, error);
        return;
    }

    audioMaster = audioActive;
    if (!audioActive)
        clock.set(double(startAt) / 1000.0);
    int packetSerial = requestedSerial.load();

    if (startAt > 0 && result.seekable)
        doSeek(double(startAt) / 1000.0, packetSerial);

    if (vctx)
        videoThread = std::thread(&Impl::videoLoop, this);
    if (audioActive)
        audioThread = std::thread(&Impl::audioLoop, this);

    if (callbacks.opened)
        callbacks.opened(result);

    AVPacket *pkt = av_packet_alloc();
    bool eof = false;

    while (!abort) {
        {
            std::unique_lock lock(ctrlMutex);
            if (seekPending) {
                const double target = seekTarget;
                const int serial = requestedSerial.load();
                seekPending = false;
                lock.unlock();
                doSeek(target, serial);
                packetSerial = serial;
                eof = false;
                continue;
            }
            if (eof || queuesFull()) {
                ctrlCv.wait_for(lock, std::chrono::milliseconds(eof ? 100 : 10),
                                [this] { return abort.load() || seekPending; });
                continue;
            }
        }

        const int r = av_read_frame(fmt, pkt);
        if (r < 0) {
            if (abort)
                break;
            if (r == AVERROR(EAGAIN)) {
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
                continue;
            }
            if (r != AVERROR_EOF)
                qWarning() << "ffmpeg: read error, treating as end of stream:" << avErrorString(r);
            if (vctx)
                videoQ.push({nullptr, packetSerial, Packet::Eof});
            if (audioActive)
                audioQ.push({nullptr, packetSerial, Packet::Eof});
            eofSerial = packetSerial;
            eof = true;
            continue;
        }

        PacketQueue *target = nullptr;
        if (pkt->stream_index == vidx && vctx)
            target = &videoQ;
        else if (pkt->stream_index == aidx && audioActive)
            target = &audioQ;

        if (target) {
            AVPacket *owned = av_packet_alloc();
            av_packet_move_ref(owned, pkt);
            target->push({owned, packetSerial, Packet::Data});
        } else {
            av_packet_unref(pkt);
        }
    }

    av_packet_free(&pkt);
}

VideoFramePtr Pipeline::Impl::convert(AVFrame *frame)
{
    auto out = std::make_unique<VideoFrame>();
    const auto format = AVPixelFormat(frame->format);
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(format);
    const bool fromRgb = desc && (desc->flags & AV_PIX_FMT_FLAG_RGB);

    // Colour properties must be read before the frame is moved from.
    out->fullRange = !fromRgb && (frame->color_range == AVCOL_RANGE_JPEG || format == AV_PIX_FMT_YUVJ420P);
    out->bt709 = !fromRgb
                 && (frame->colorspace == AVCOL_SPC_BT709
                     || (frame->colorspace == AVCOL_SPC_UNSPECIFIED && frame->height >= 720));
    const AVRational sar = frame->sample_aspect_ratio.num > 0 ? frame->sample_aspect_ratio : AVRational{1, 1};
    out->aspect = frame->height > 0 ? double(frame->width) * av_q2d(sar) / frame->height : 1.0;

    if (format == AV_PIX_FMT_YUV420P || format == AV_PIX_FMT_YUVJ420P) {
        out->frame = av_frame_alloc();
        av_frame_move_ref(out->frame, frame);
        return out;
    }

    // Everything else (NV12, 10-bit, 4:2:2, RGB...) goes through swscale.
    // Cheap to write, costs CPU; see the notes on doing this in a shader.
    sws = sws_getCachedContext(sws, frame->width, frame->height, format, frame->width, frame->height,
                               AV_PIX_FMT_YUV420P, SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (!sws)
        return {};

    out->frame = av_frame_alloc();
    out->frame->format = AV_PIX_FMT_YUV420P;
    out->frame->width = frame->width;
    out->frame->height = frame->height;
    if (av_frame_get_buffer(out->frame, 0) < 0)
        return {};
    sws_scale(sws, frame->data, frame->linesize, 0, frame->height, out->frame->data, out->frame->linesize);
    av_frame_unref(frame);
    return out;
}

void Pipeline::Impl::videoLoop()
{
    AVFrame *frame = av_frame_alloc();
    int serial = -1;
    double nextPts = 0.0;
    Packet p;

    while (videoQ.pop(p)) {
        if (p.serial != serial) {
            avcodec_flush_buffers(vctx);
            serial = p.serial;
        }
        if (p.kind == Packet::Flush)
            continue;

        const bool isEof = p.kind == Packet::Eof;
        avcodec_send_packet(vctx, isEof ? nullptr : p.pkt);
        av_packet_free(&p.pkt);

        while (!abort) {
            const int r = avcodec_receive_frame(vctx, frame);
            if (r == AVERROR_EOF) {
                videoDrainedSerial = serial;
                break;
            }
            if (r < 0)
                break; // EAGAIN (needs more input) or a corrupt packet

            if (serial != requestedSerial.load()) {
                av_frame_unref(frame);
                continue;
            }

            double pts = toSeconds(frame->best_effort_timestamp, vtb) - startTime;
            if (std::isnan(pts))
                pts = nextPts;
            nextPts = pts + frameDuration;

            if (pts + frameDuration < skipUntil.load()) {
                av_frame_unref(frame);
                continue;
            }

            VideoFramePtr out = convert(frame);
            av_frame_unref(frame);
            if (!out)
                continue;
            out->pts = pts;
            out->duration = frameDuration;
            out->serial = serial;
            frames.push(std::move(out), requestedSerial);
        }
    }

    av_frame_free(&frame);
}

bool Pipeline::Impl::ensureResampler(const AVFrame *frame)
{
    if (swr && frame->format == swrInFormat && frame->sample_rate == swrInRate
        && av_channel_layout_compare(&frame->ch_layout, &swrInLayout) == 0)
        return true;

    swr_free(&swr);
    av_channel_layout_uninit(&swrInLayout);

    AVChannelLayout in{};
    if (frame->ch_layout.order == AV_CHANNEL_ORDER_UNSPEC)
        av_channel_layout_default(&in, frame->ch_layout.nb_channels);
    else
        av_channel_layout_copy(&in, &frame->ch_layout);
    AVChannelLayout out{};
    av_channel_layout_default(&out, outChannels);

    int r = swr_alloc_set_opts2(&swr, &out, AV_SAMPLE_FMT_S16, outRate, &in, AVSampleFormat(frame->format),
                                frame->sample_rate, 0, nullptr);
    if (r >= 0)
        r = swr_init(swr);

    av_channel_layout_uninit(&in);
    av_channel_layout_uninit(&out);

    if (r < 0) {
        qWarning() << "ffmpeg: could not set up resampler:" << avErrorString(r);
        swr_free(&swr);
        return false;
    }
    swrInFormat = frame->format;
    swrInRate = frame->sample_rate;
    av_channel_layout_copy(&swrInLayout, &frame->ch_layout);
    return true;
}

void Pipeline::Impl::audioLoop()
{
    AVFrame *frame = av_frame_alloc();
    int serial = -1;
    double nextPts = 0.0;
    Packet p;

    while (audioQ.pop(p)) {
        if (p.serial != serial) {
            avcodec_flush_buffers(actx);
            serial = p.serial;
        }
        if (p.kind == Packet::Flush)
            continue;

        const bool isEof = p.kind == Packet::Eof;
        avcodec_send_packet(actx, isEof ? nullptr : p.pkt);
        av_packet_free(&p.pkt);

        while (!abort) {
            const int r = avcodec_receive_frame(actx, frame);
            if (r == AVERROR_EOF) {
                audioDrainedSerial = serial;
                break;
            }
            if (r < 0)
                break;

            if (serial != requestedSerial.load() || frame->sample_rate <= 0) {
                av_frame_unref(frame);
                continue;
            }

            double pts = toSeconds(frame->best_effort_timestamp, atb) - startTime;
            if (std::isnan(pts))
                pts = nextPts;
            const double duration = double(frame->nb_samples) / frame->sample_rate;
            nextPts = pts + duration;

            if (pts + duration < skipUntil.load() || !ensureResampler(frame)) {
                av_frame_unref(frame);
                continue;
            }

            const int capacity = swr_get_out_samples(swr, frame->nb_samples);
            QByteArray pcm(qsizetype(capacity) * outChannels * 2, Qt::Uninitialized);
            uint8_t *outPlanes[1] = {reinterpret_cast<uint8_t *>(pcm.data())};
            const int got = swr_convert(swr, outPlanes, capacity,
                                        const_cast<const uint8_t **>(frame->extended_data), frame->nb_samples);
            av_frame_unref(frame);
            if (got <= 0)
                continue;
            pcm.resize(qsizetype(got) * outChannels * 2);

            audio.push({std::move(pcm), 0, pts, serial}, requestedSerial);
        }
    }

    av_frame_free(&frame);
}

void Pipeline::Impl::shutdown()
{
    abort = true;
    {
        std::lock_guard lock(ctrlMutex);
    }
    ctrlCv.notify_all();
    videoQ.abort();
    audioQ.abort();
    frames.abort();
    audio.abort();

    // The demux thread starts the decoders, so it must be joined first.
    if (demuxThread.joinable())
        demuxThread.join();
    if (videoThread.joinable())
        videoThread.join();
    if (audioThread.joinable())
        audioThread.join();

#ifdef VURA_HAVE_QTMULTIMEDIA
    if (sink)
        sink->stop();
    sink.reset();
    device.reset();
#endif

    sws_freeContext(sws);
    sws = nullptr;
    swr_free(&swr);
    av_channel_layout_uninit(&swrInLayout);
    avcodec_free_context(&vctx);
    avcodec_free_context(&actx);
    avformat_close_input(&fmt);
}

// ========================================================== Pipeline ====

Pipeline::Pipeline(const QUrl &url, Msec startAt, int preferredVideo, int preferredAudio, Callbacks callbacks)
    : d(std::make_unique<Impl>())
{
    d->url = url;
    d->startAt = std::max<Msec>(0, startAt);
    d->wantVideo = preferredVideo;
    d->wantAudio = preferredAudio;
    d->callbacks = std::move(callbacks);

#ifdef VURA_HAVE_QTMULTIMEDIA
    // The sink is a QObject, so it is built here on the GUI thread. The
    // decoders only need to know the rate it settled on.
    const QAudioDevice dev = QMediaDevices::defaultAudioOutput();
    if (!dev.isNull()) {
        QAudioFormat format;
        format.setChannelCount(Impl::outChannels);
        format.setSampleFormat(QAudioFormat::Int16);
        for (int rate : {dev.preferredFormat().sampleRate(), 48000, 44100}) {
            if (rate <= 0)
                continue;
            format.setSampleRate(rate);
            if (dev.isFormatSupported(format)) {
                d->outRate = rate;
                d->audioOutputOk = true;
                break;
            }
        }
        if (d->audioOutputOk) {
            d->device = std::make_unique<AudioPullDevice>(&d->audio, &d->requestedSerial,
                                                          d->outRate * Impl::outChannels * 2);
            d->device->open(QIODevice::ReadOnly);
            d->sink = std::make_unique<QAudioSink>(dev, format);
            d->sink->setVolume(d->volume);
            // Ask for a small buffer explicitly. Left unset, Qt passes no
            // attributes and PulseAudio defaults to ~2 s, which it insists
            // on filling before it starts playing -- a second or more of
            // dead air on every play and every seek.
            d->sink->setBufferSize(format.bytesForDuration(150000));
        }
    }
    if (!d->audioOutputOk)
        qWarning() << "ffmpeg: no usable audio output; playing video only";
#endif

    d->audio.setBytesPerSecond(d->outRate * Impl::outChannels * 2);
    // The clock is left unknown here. With audio it becomes known when the
    // first real sample is audible; without, demuxLoop() starts it.
}

Pipeline::~Pipeline()
{
    d->shutdown();
}

void Pipeline::start()
{
    d->demuxThread = std::thread(&Impl::demuxLoop, d.get());
}

void Pipeline::setPaused(bool paused)
{
    d->clock.setPaused(paused);

#ifdef VURA_HAVE_QTMULTIMEDIA
    if (!d->sink || !d->audioMaster)
        return;
    if (paused) {
        if (d->sink->state() == QAudio::ActiveState || d->sink->state() == QAudio::IdleState)
            d->sink->suspend();
        return;
    }
    if (d->sink->state() == QAudio::SuspendedState)
        d->sink->resume();
    else if (d->sink->state() == QAudio::StoppedState)
        d->startSink();
#endif
}

void Pipeline::seek(Msec ms)
{
    const double target = double(std::max<Msec>(0, ms)) / 1000.0;

    // Bumping the serial first makes everything already decoded stale at
    // once, so the old picture never flashes up after the seek.
    ++d->requestedSerial;
    d->frames.clear();
    d->audio.clear();
    if (d->audioMaster) {
        d->clock.invalidate(); // re-anchored once the new audio is audible
#ifdef VURA_HAVE_QTMULTIMEDIA
        // Restarting the sink flushes whatever it still holds; otherwise
        // the old position keeps playing for as long as its buffer lasts.
        // A suspended sink is simply stopped and restarts on resume.
        const bool running = d->sinkRunning();
        d->sink->stop();
        d->device->resetTimeline();
        if (running)
            d->startSink();
#endif
    } else {
        d->clock.set(target);
    }

    {
        std::lock_guard lock(d->ctrlMutex);
        d->seekPending = true;
        d->seekTarget = target;
    }
    d->ctrlCv.notify_all();
}

void Pipeline::setVolume(qreal linear)
{
    d->volume = linear;
#ifdef VURA_HAVE_QTMULTIMEDIA
    if (d->sink)
        d->sink->setVolume(linear);
#endif
}

double Pipeline::clock()
{
#ifdef VURA_HAVE_QTMULTIMEDIA
    if (d->audioMaster && d->sinkRunning()) {
        const double audible = d->device->ptsAt(d->playedBytes(), d->requestedSerial.load());
        if (!std::isnan(audible)) {
            // Hard-set on first sight or a large disagreement; otherwise
            // nudge, so a backend whose processedUSecs() moves in steps
            // (Windows reports bytes written) doesn't make the picture judder.
            const double predicted = d->clock.get();
            if (std::isnan(predicted) || std::abs(audible - predicted) > 0.1)
                d->clock.set(audible);
            else
                d->clock.set(predicted + 0.1 * (audible - predicted));
        }
    }
#endif
    return d->clock.get();
}

int Pipeline::serial() const
{
    return d->requestedSerial.load();
}

VideoFramePtr Pipeline::takeFrame(double clock, bool haveCurrent)
{
    return d->frames.take(clock, haveCurrent, d->requestedSerial.load());
}

double Pipeline::nextFrameDue()
{
    return d->frames.nextPts(d->requestedSerial.load());
}

void Pipeline::dropLateFrames(double clock)
{
    d->frames.dropLate(clock, d->requestedSerial.load());
}

bool Pipeline::finished()
{
    const int s = d->requestedSerial.load();
    if (d->eofSerial.load() != s)
        return false;
    if (d->vctx && !(d->videoDrainedSerial.load() == s && d->frames.empty()))
        return false;
    if (d->audioActive && !(d->audioDrainedSerial.load() == s && d->audio.empty()))
        return false;
#ifdef VURA_HAVE_QTMULTIMEDIA
    // The last samples may still be sitting in the device's buffer.
    if (d->audioMaster && d->sinkRunning() && !d->device->drained(d->playedBytes()))
        return false;
#endif
    return true;
}

}
