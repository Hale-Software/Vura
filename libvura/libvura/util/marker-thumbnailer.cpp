#include "marker-thumbnailer.h"

#include <QImage>
#include <QMediaPlayer>
#include <QTimer>
#include <QVideoFrame>
#include <QVideoSink>

namespace {
constexpr int kTimeoutMs = 4000;
constexpr qint64 kFrameToleranceMs = 1500;   // keyframe seeks can land a bit early
constexpr QSize kGrabSize(320, 180);
}

MarkerThumbnailer::MarkerThumbnailer(QObject *parent)
    : QObject(parent),
      m_player(new QMediaPlayer(this)),
      m_sink(new QVideoSink(this)),
      m_timeout(new QTimer(this))
{
    // No QAudioOutput is attached, so this player is silent.
    m_player->setVideoSink(m_sink);

    m_timeout->setSingleShot(true);
    m_timeout->setInterval(kTimeoutMs);
    connect(m_timeout, &QTimer::timeout, this, [this] { finishCurrent(QImage()); });

    connect(m_player, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status) {
        switch (status) {
        case QMediaPlayer::LoadedMedia:
        case QMediaPlayer::BufferedMedia:
            if (!m_ready) {
                m_ready = true;
                processNext();
            }
            break;
        case QMediaPlayer::InvalidMedia:
        case QMediaPlayer::NoMedia:
            m_ready = false;
            m_queue.clear();
            break;
        default:
            break;
        }
    });

    connect(m_sink, &QVideoSink::videoFrameChanged, this, &MarkerThumbnailer::onFrame);
}

void MarkerThumbnailer::setSource(const QUrl &source)
{
    cancelAll();
    m_ready = false;
    m_player->stop();
    // Only thumbnail local files; decoding a network stream a second time is wasteful.
    m_player->setSource(source.isLocalFile() ? source : QUrl());
}

void MarkerThumbnailer::request(int markerId, qint64 positionMs)
{
    for (Job &job : m_queue) {
        if (job.markerId == markerId) {
            job.positionMs = positionMs;
            return;
        }
    }
    m_queue.enqueue({markerId, positionMs});
    processNext();
}

void MarkerThumbnailer::cancelAll()
{
    m_queue.clear();
    m_current.reset();
    m_timeout->stop();
    m_player->pause();
}

void MarkerThumbnailer::processNext()
{
    if (!m_ready || m_current || m_queue.isEmpty())
        return;

    m_current = m_queue.dequeue();
    m_player->setPosition(m_current->positionMs);
    m_player->play();          // most backends only deliver a frame while playing
    m_timeout->start();
}

void MarkerThumbnailer::onFrame(const QVideoFrame &frame)
{
    if (!m_current || !frame.isValid())
        return;

    // Ignore stale frames from before the seek.
    const qint64 frameMs = frame.startTime() / 1000;
    if (frameMs >= 0 && qAbs(frameMs - m_current->positionMs) > kFrameToleranceMs)
        return;

    const QImage image = frame.toImage();
    if (image.isNull())
        return;

    finishCurrent(image.scaled(kGrabSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void MarkerThumbnailer::finishCurrent(const QImage &image)
{
    if (!m_current)
        return;

    m_timeout->stop();
    m_player->pause();

    const Job job = *m_current;
    m_current.reset();

    if (!image.isNull())
        emit thumbnailReady(job.markerId, job.positionMs, image);

    QTimer::singleShot(0, this, &MarkerThumbnailer::processNext);
}
