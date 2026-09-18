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

#pragma once

#include "engine.h"
#include "../playback/subtitle-track.h"
#include "../models/subtitle-cue.h"

#include <QHash>
#include <QVideoFrame>

QT_BEGIN_NAMESPACE
class QAudioOutput;
class QMediaDevices;
class QMediaPlayer;
class QVideoSink;
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
    bool loadExternalSubtitle(const QUrl &url) override;

    bool attachOutput(VideoOutput *output) override;
    void detachOutput() override;

    QVector<AudioDeviceInfo> audioDevices() const override;
    QString activeAudioDevice() const override;
    void setAudioDevice(const QString &id) override;

private slots:
    void videoFrameChanged(const QVideoFrame &frame);

private:
    void rebuildTrackCache();
    void publishMetaData();
    int indexForId(TrackType type, const QString &id) const;

    QMediaPlayer *m_player = nullptr;
    QAudioOutput *m_audioOutput = nullptr;
    QVideoSink *m_videoSink = nullptr;
    QMediaDevices *m_mediaDevices = nullptr;
    QHash<TrackType, QVector<TrackInfo>> m_tracks;

    SubtitleTrack *m_subtitleTrack = nullptr;
    SubtitleCue *m_currentCue = nullptr;

    bool m_followSystemDefaultAudioDevice = true;
    bool m_subtitlesEnabled = false;
    qint64 m_subtitleOffsetMs = 0;

};

} // namespace media
