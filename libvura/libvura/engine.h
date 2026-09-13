#pragma once

#include <QObject>
#include <QUrl>
#include <QVector>

#include "video-output.h"
#include "enums.h"
#include "models/capabilities.h"
#include "models/track-info.h"


class Engine : public QObject
{
    Q_OBJECT
public:
    ~Engine() override = default;

    virtual Capabilities capabilities() const = 0;

    virtual void setSource(const QUrl &url) = 0;
    virtual void play() = 0;
    virtual void pause() = 0;
    virtual void stop() = 0;
    virtual void seek(qint64 ms) = 0;

    virtual void setRate(qreal rate) = 0;
    virtual void setVolume(qreal linear) = 0;
    virtual void setMuted(bool muted) = 0;

    virtual QVector<TrackInfo> tracks(TrackType type) const = 0;
    virtual void selectTrack(TrackType type, const QString &id) = 0;

    virtual void attachOutput(VideoOutput *out) = 0;
    virtual void detachOutput() = 0;

signals:
    void positionChanged(qint64 ms);
    void durationChanged(qint64 ms);
    void playbackStateChanged(PlaybackState state);
    void mediaStatusChanged(MediaStatus status);
    void tracksChanged();
    void metaDataChanged(const QVariantMap &data);
    void errorOccurred(ErrorKind kind, const QString &detail);

};
