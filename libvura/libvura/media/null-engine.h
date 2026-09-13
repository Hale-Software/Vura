#pragma once

#include "engine.h"

QT_BEGIN_NAMESPACE
class QTimer;
QT_END_NAMESPACE

namespace media {

/// A backend that decodes nothing.
///
/// Build this first. It lets the controller, playlist, resume store and the
/// entire UI be written and tested before a real decoder is linked in, and
/// it keeps its value afterwards because it is the only engine whose
/// behaviour is fully deterministic — which makes it the fixture every unit
/// test runs against.
///
/// Duration and fake track layout come from the URL so tests can ask for a
/// specific shape:
///
///     null:///clip?duration=180000&audio=2&subs=1&fail=notfound
///
/// `tick` controls the simulated clock interval in ms (default 100).
class NullEngine : public Engine
{
    Q_OBJECT

public:
    explicit NullEngine(QObject *parent = nullptr);
    ~NullEngine() override;

    QString name() const override { return QStringLiteral("null"); }
    OutputKind requiredOutput() const override { return OutputKind::None; }
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
    bool loadExternalSubtitle(const QUrl &url) override;

    bool attachOutput(VideoOutput *output) override;
    void detachOutput() override;

    /// Advance the simulated clock by hand. Tests use this instead of
    /// waiting on the timer, so they stay fast and deterministic.
    void advance(Msec ms);

private:
    void onTick();
    void rebuildTracks(int audioCount, int subtitleCount, bool hasVideo);

    QTimer *m_timer = nullptr;
    int m_tickMs = 100;
    QVector<TrackInfo> m_video;
    QVector<TrackInfo> m_audio;
    QVector<TrackInfo> m_subtitles;
    QString m_activeVideo;
    QString m_activeAudio;
    QString m_activeSubtitle;
    int m_externalSubCount = 0;
};

} // namespace media
