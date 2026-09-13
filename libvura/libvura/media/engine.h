#pragma once

#include "types.h"
#include "video-output.h"

#include <QObject>
#include <QUrl>
#include <QVariantMap>
#include <QVector>

namespace media {

class Engine : public QObject
{
    Q_OBJECT
public:
    explicit Engine(QObject *parent = nullptr);
    ~Engine() override;

    virtual QString name() const = 0;
    virtual Capabilities capabilities() const = 0;

    virtual OutputKind requiredOutput() const = 0;

    QUrl source() const { return m_source; }
    Msec position() const { return m_position; }
    Msec duration() const { return m_duration; }
    PlaybackState playbackState() const { return m_state; }
    MediaStatus mediaStatus() const { return m_status; }
    qreal rate() const { return m_rate; }
    qreal volume() const { return m_volume; }
    bool isMuted() const { return m_muted; }
    QVariantMap metadata() const { return m_metadata; }
    bool isSeekable() const { return m_seekable; }

    virtual void setSource(const QUrl &url) = 0;
    virtual void play() = 0;
    virtual void pause() = 0;
    virtual void stop() = 0;
    virtual void seek(Msec ms) = 0;

    void seekRelative(Msec deltaMs);

    virtual void setRate(qreal rate) = 0;
    virtual void setVolume(qreal linear) = 0;
    virtual void setMuted(bool muted) = 0;

    virtual QVector<TrackInfo> tracks(TrackType type) const = 0;
    virtual QString activeTrack(TrackType type) const = 0;

    virtual void selectTrack(TrackType type, const QString &id) = 0;

    virtual bool loadExternalSubtitle(const QUrl &url);

    virtual bool attachOutput(VideoOutput *output) = 0;
    virtual void detachOutput() = 0;
    VideoOutput *output() const { return m_output; }

signals:
    void sourceChanged(const QUrl &url);
    void positionChanged(Msec ms);
    void durationChanged(Msec ms);
    void playbackStateChanged(PlaybackState state);
    void mediaStatusChanged(MediaStatus status);
    void seekableChanged(bool seekable);
    void rateChanged(qreal rate);
    void volumeChanged(qreal linear);
    void mutedChanged(bool muted);
    void tracksChanged();
    void metaDataChanged(const QVariantMap &data);
    void errorOccurred(ErrorKind kind, const QString &detail);

protected:
    void updateSource(const QUrl &url);
    void updatePosition(Msec ms);
    void updateDuration(Msec ms);
    void updatePlaybackState(PlaybackState state);
    void updateMediaStatus(MediaStatus status);
    void updateSeekable(bool seekable);
    void updateRate(qreal rate);
    void updateVolume(qreal linear);
    void updateMuted(bool muted);
    void updateMetaData(const QVariantMap &data);
    void reportError(ErrorKind kind, const QString &detail);

    void setOutputPointer(VideoOutput *output) { m_output = output; }

    void resetForNewSource(const QUrl &url);

private:
    QUrl m_source;
    Msec m_position = 0;
    Msec m_duration = 0;
    PlaybackState m_state = PlaybackState::Stopped;
    MediaStatus m_status = MediaStatus::NoMedia;
    qreal m_rate = 1.0;
    qreal m_volume = 1.0;
    bool m_muted = false;
    bool m_seekable = false;
    QVariantMap m_metadata;
    VideoOutput *m_output = nullptr;

};

}
