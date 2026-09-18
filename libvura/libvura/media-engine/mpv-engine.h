#pragma once

#include "engine.h"

#include <QHash>

struct mpv_handle;
struct mpv_render_context;

namespace media {

/// Adapter over libmpv.
///
/// What most serious players use, because correct subtitle rendering,
/// hardware decoding and gapless playback come for free instead of being
/// six months of work.
///
/// Threading: libmpv delivers events on its own thread. Nothing in here
/// touches Engine state from that thread — the wakeup callback only posts a
/// queued call to processEvents(), which runs on the GUI thread and does all
/// the draining. That is what makes the Engine signal contract true.
class MpvEngine : public Engine
{
    Q_OBJECT

public:
    /// Throws std::runtime_error if libmpv cannot be initialised, so the
    /// factory can fall back to another backend.
    explicit MpvEngine(QObject *parent = nullptr);
    ~MpvEngine() override;

    QString name() const override { return QStringLiteral("mpv"); }
    OutputKind requiredOutput() const override { return OutputKind::OpenGL; }
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

private:
    // Called on the GUI thread only.
    void processEvents();
    void handlePropertyChange(quint64 id, void *data, int format);
    void applyTrackList(const QVariant &list);
    void refreshStatus();
    void renderFrame(const QSize &sizeInPixels, unsigned int fbo);

    void setProperty(const char *name, const QVariant &value);
    void command(const QStringList &args);

    static void onWakeup(void *ctx);
    static void onRenderUpdate(void *ctx);
    static void *onGetProcAddress(void *ctx, const char *name);

    mpv_handle *m_mpv = nullptr;
    mpv_render_context *m_renderContext = nullptr;
    GLVideoOutput *m_glOutput = nullptr;

    QHash<TrackType, QVector<TrackInfo>> m_tracks;
    QHash<TrackType, QString> m_activeTracks;

    bool m_paused = true;
    bool m_coreIdle = true;
    bool m_eofReached = false;
    bool m_pausedForCache = false;
    bool m_fileLoaded = false;
};

} // namespace media
