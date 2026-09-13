#pragma once

#include "engine.h"

#include <QHash>

QT_BEGIN_NAMESPACE
class QAudioOutput;
class QMediaPlayer;
QT_END_NAMESPACE

namespace media {

/// Adapter over QMediaPlayer (Qt 6.5+, FFmpeg backend recommended).
///
/// The least-friction backend: one codec story on all three platforms and
/// no extra dependency. It gives up pitch-corrected rate changes and
/// side-loaded subtitle files, which is exactly what Capabilities exists to
/// communicate.
///
/// Qt identifies tracks by index into its own list. That numbering is
/// converted to opaque ids here and never escapes the adapter.
class QtEngine : public Engine
{
    Q_OBJECT

public:
    explicit QtEngine(QObject *parent = nullptr);
    ~QtEngine() override;

    QString name() const override { return QStringLiteral("qtmultimedia"); }
    OutputKind requiredOutput() const override { return OutputKind::VideoSink; }
    Capabilities capabilities() const override;

    void setSource(const QUrl &url) override;
    void play() override;
    void pause() override;
    void stop() override;
    void seek(Msec ms) override;

    void setRate(qreal rate) override;
    void setVolume(qreal linear) override;
    void setMuted(bool muted) override;

    QVector<TrackInfo> tracks(TrackType type) const override;
    QString activeTrack(TrackType type) const override;
    void selectTrack(TrackType type, const QString &id) override;

    bool attachOutput(VideoOutput *output) override;
    void detachOutput() override;

private:
    void rebuildTrackCache();
    void publishMetaData();
    int indexForId(TrackType type, const QString &id) const;

    QMediaPlayer *m_player = nullptr;
    QAudioOutput *m_audioOutput = nullptr;

    QHash<TrackType, QVector<TrackInfo>> m_tracks;
};

} // namespace media
