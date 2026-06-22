// ThumbnailWorker.h
#pragma once
#include <QThread>
#include <QImage>
#include <QMutex>
#include <QString>
#include <atomic>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libavutil/imgutils.h>
}

class ThumbnailWorker : public QThread {
    Q_OBJECT

public:
    explicit ThumbnailWorker(const QString& filePath, QObject* parent = nullptr);
    ~ThumbnailWorker();

    // Call this from the UI when the mouse moves
    void requestThumbnail(int64_t timestamp);

signals:
    void thumbnailReady(QImage image, int64_t requestedTimestamp);

protected:
    void run() override;

private:
    QString m_filePath;
    QMutex m_mutex;
    std::atomic<bool> m_abort;
    int64_t m_targetTimestamp;
    bool m_hasNewRequest;

};
