#include "remuxer.h"

#include <QDebug>
#include <QFile>
#include <QtGlobal>

#include <memory>
#include <vector>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
}


struct InputCtxDeleter
{
    void operator()(AVFormatContext *ctx) const
    {
        avformat_close_input(&ctx);
    }
};

struct OutputCtxDeleter {
    void operator()(AVFormatContext *ctx) const {
        if (!ctx) return;
        if (!(ctx->oformat->flags & AVFMT_NOFILE))
            avio_closep(&ctx->pb);
        avformat_free_context(ctx);
    }
};

struct PacketDeleter {
    void operator()(AVPacket *p) const
    {
        av_packet_free(&p);
    }
};

using InputCtx  = std::unique_ptr<AVFormatContext, InputCtxDeleter>;
using OutputCtx = std::unique_ptr<AVFormatContext, OutputCtxDeleter>;
using Packet    = std::unique_ptr<AVPacket, PacketDeleter>;


bool Remuxer::remux(const QString &inputPath, const QString &outputPath)
{
    m_cancel = false;
    m_lastError.clear();

    // FFmpeg expects UTF-8 paths (this also works on Windows).
    const QByteArray inPath  = inputPath.toUtf8();
    const QByteArray outPath = outputPath.toUtf8();

    // ---- 1. Open the input and read stream info ---------------------------
    AVFormatContext *rawIn = nullptr;
    int ret = avformat_open_input(&rawIn, inPath.constData(), nullptr, nullptr);
    if (ret < 0)
        return fail(tr("Cannot open input"), ret);
    InputCtx inCtx(rawIn);

    ret = avformat_find_stream_info(inCtx.get(), nullptr);
    if (ret < 0)
        return fail(tr("Cannot read stream info"), ret);

    // ---- 2. Create the output context (format guessed from extension) -----
    AVFormatContext *rawOut = nullptr;
    ret = avformat_alloc_output_context2(&rawOut, nullptr, nullptr, outPath.constData());
    if (ret < 0 || !rawOut)
        return fail(tr("Cannot determine output format from file name"), ret);
    OutputCtx outCtx(rawOut);

    // ---- 3. Map input streams to output streams ----------------------------
    std::vector<int> streamMap(inCtx->nb_streams, -1);
    int nextOutIndex = 0;

    for (unsigned i = 0; i < inCtx->nb_streams; ++i) {
        AVStream *inStream = inCtx->streams[i];
        const AVCodecParameters *inPar = inStream->codecpar;

        if (inPar->codec_type != AVMEDIA_TYPE_VIDEO &&
            inPar->codec_type != AVMEDIA_TYPE_AUDIO &&
            inPar->codec_type != AVMEDIA_TYPE_SUBTITLE)
            continue; // skip data/attachment streams

        // 0 = definitely unsupported by this container; <0 = unknown (try anyway)
        if (avformat_query_codec(outCtx->oformat, inPar->codec_id,
                                 FF_COMPLIANCE_NORMAL) == 0) {
            qWarning() << "Skipping stream" << i << "codec"
                       << avcodec_get_name(inPar->codec_id)
                       << "not supported by" << outCtx->oformat->name;
            continue;
        }

        AVStream *outStream = avformat_new_stream(outCtx.get(), nullptr);
        if (!outStream)
            return fail(tr("Cannot allocate output stream"), AVERROR(ENOMEM));

        ret = avcodec_parameters_copy(outStream->codecpar, inPar);
        if (ret < 0)
            return fail(tr("Cannot copy codec parameters"), ret);

        // Let the muxer pick the right tag for the new container
        // (e.g. MKV's tag is invalid in MP4).
        outStream->codecpar->codec_tag = 0;
        outStream->time_base   = inStream->time_base; // hint; muxer may change it
        outStream->disposition = inStream->disposition;
        av_dict_copy(&outStream->metadata, inStream->metadata, 0);

        streamMap[i] = nextOutIndex++;
    }

    if (nextOutIndex == 0)
        return fail(tr("No streams can be stored in the output container"));

    av_dict_copy(&outCtx->metadata, inCtx->metadata, 0);

    // ---- 4. Open the output file and write the header ----------------------
    if (!(outCtx->oformat->flags & AVFMT_NOFILE)) {
        ret = avio_open(&outCtx->pb, outPath.constData(), AVIO_FLAG_WRITE);
        if (ret < 0)
            return fail(tr("Cannot open output file"), ret);
    }

    AVDictionary *muxOpts = nullptr;
    if (outputPath.endsWith(".mp4", Qt::CaseInsensitive) ||
        outputPath.endsWith(".mov", Qt::CaseInsensitive) ||
        outputPath.endsWith(".m4a", Qt::CaseInsensitive))
        av_dict_set(&muxOpts, "movflags", "+faststart", 0); // web-friendly MP4

    ret = avformat_write_header(outCtx.get(), &muxOpts);
    av_dict_free(&muxOpts);
    if (ret < 0) {
        outCtx.reset();
        QFile::remove(outputPath);
        return fail(tr("Cannot write header"), ret);
    }

    // ---- 5. Copy packets ---------------------------------------------------
    Packet pkt(av_packet_alloc());
    if (!pkt)
        return fail(tr("Cannot allocate packet"), AVERROR(ENOMEM));

    const int64_t totalDuration = inCtx->duration; // AV_TIME_BASE units
    int lastPercent = -1;

    while (true) {
        if (m_cancel) { ret = AVERROR_EXIT; break; }

        ret = av_read_frame(inCtx.get(), pkt.get());
        if (ret == AVERROR_EOF) { ret = 0; break; }
        if (ret < 0) break;

        const int inIndex = pkt->stream_index;
        if (inIndex < 0 || inIndex >= int(streamMap.size()) || streamMap[inIndex] < 0) {
            av_packet_unref(pkt.get());
            continue;
        }

        AVStream *inStream  = inCtx->streams[inIndex];
        AVStream *outStream = outCtx->streams[streamMap[inIndex]];

        // Progress (based on the packet timestamp vs. total duration).
        if (totalDuration > 0 && pkt->pts != AV_NOPTS_VALUE) {
            const int64_t start = inStream->start_time != AV_NOPTS_VALUE ? inStream->start_time : 0;
            const int64_t t = av_rescale_q(pkt->pts - start, inStream->time_base, AV_TIME_BASE_Q);
            const int64_t rawPct = av_rescale(t, 100, totalDuration);
            const int pct = std::clamp(static_cast<int>(rawPct), 0, 100);

            // Ensure monotonic progress reporting to avoid flickering/regressions
            if (pct > lastPercent) {
                lastPercent = pct;
                emit progress(pct);
            }
        }

        pkt->stream_index = streamMap[inIndex];
        av_packet_rescale_ts(pkt.get(), inStream->time_base, outStream->time_base);
        pkt->pos = -1;

        // Takes ownership of the packet's data and unrefs it.
        ret = av_interleaved_write_frame(outCtx.get(), pkt.get());
        if (ret < 0) break;
    }

    // ---- 6. Finish ---------------------------------------------------------
    const int trailerRet = av_write_trailer(outCtx.get());

    if (ret < 0 || trailerRet < 0) {
        outCtx.reset();               // close the file before deleting it
        QFile::remove(outputPath);    // don't leave a broken partial file
        if (ret == AVERROR_EXIT)
            return fail(tr("Cancelled"));
        return fail(ret < 0 ? tr("Error while remuxing") : tr("Cannot write trailer"),
                    ret < 0 ? ret : trailerRet);
    }

    emit progress(100);
    emit finished(true, QString());
    return true;
}

bool Remuxer::fail(const QString &msg, int avError)
{
    m_lastError = avError ? QStringLiteral("%1: %2").arg(msg, avErrorString(avError))
                          : msg;
    emit finished(false, m_lastError);
    return false;
}

QString Remuxer::avErrorString(int err)
{
    char buf[AV_ERROR_MAX_STRING_SIZE] = {};
    av_strerror(err, buf, sizeof(buf));
    return QString::fromUtf8(buf);
}
