#pragma once

#include <QObject>
#include <QQueue>
#include <QUrl>

#include <optional>

class QImage;
class QMediaPlayer;
class QTimer;
class QVideoFrame;
class QVideoSink;

/// Grabs one frame per marker using a hidden, silent QMediaPlayer, one job at
/// a time. It is completely separate from the playback engine, so it never
/// disturbs what the user is watching. Swap the internals for your own engine
/// (e.g. an mpv "screenshot" or an FFmpeg decoder) if you prefer.
class MarkerThumbnailer : public QObject
{
    Q_OBJECT
public:
    explicit MarkerThumbnailer(QObject *parent = nullptr);

    void setSource(const QUrl &source);
    void request(int markerId, qint64 positionMs);
    void cancelAll();

    signals:
        void thumbnailReady(int markerId, qint64 positionMs, const QImage &image);

private:
    struct Job
    {
        int markerId = 0;
        qint64 positionMs = 0;
    };

    void processNext();
    void onFrame(const QVideoFrame &frame);
    void finishCurrent(const QImage &image);

    QMediaPlayer *m_player = nullptr;
    QVideoSink *m_sink = nullptr;
    QTimer *m_timeout = nullptr;
    QQueue<Job> m_queue;
    std::optional<Job> m_current;
    bool m_ready = false;
};
