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

#include <QObject>
#include <QSettings>
#include <QWidget>
#include <QStackedWidget>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QVideoWidget>
#include <QVideoSink>
#include <QString>
#include <QUrl>
#include <QMediaMetaData>
#include <QDebug>

#include "../media-engine/media-engine.h"
#include "../models/metadata.h"


class PlaybackController : public QObject {
    Q_OBJECT

public:
    explicit PlaybackController(QStackedWidget* container, QObject* parent = nullptr, int volume = 100, double playbackRate = 1.0);
    ~PlaybackController() override;

    void setVideoWidget(QVideoWidget* videoWidget);
    void setOpenGLWidget(VuraMediaEngine* openGLWidget);

    QMediaPlayer* getVideoWidget() const { return m_player; }
    QAudioOutput* getAudioOutput() const { return m_audioOutput; }
    MetaData* getMetadata() const { return m_metadata; }

signals:
    void positionChanged(qint64 position);
    void durationChanged(qint64 duration);
    void stateChanged(PlaybackState state);
    void bufferProgressChanged(float filled);
    void playbackRateChanged(qreal rate);
    void playingChanged(bool playing);
    void sourceChanged(const QUrl& media);
    void tracksChanged();
    void errorOccurred(const QString& errorMessage);
    void jumpCompleted();
    void mediaEnded();

public slots:
    void playTrack(const QUrl& mediaUrl);
    void loadMedia(const QUrl& url);
    void loadMedia(const QString& fileName);
    void loadRemoteMedia(const QString& url) const;
    void loadRemoteMedia(const QUrl& url) const;
    void play() const;
    void pause() const;
    void togglePlayPause() const;
    void setPaused(bool paused) const;
    void stop();
    void restart();
    void setPosition(qint64 position);

    void setPlaybackRate(double rate);
    void playbackRateFaster();
    void playbackRateFasterFine();
    void playbackRateNormal();
    void playbackRateSlower();
    void playbackRateSlowerFine();

    void changeVolume(int newVolume) const;
    void volumeUp() const;
    void volumeDown() const;
    void toggleMute() const;
    void setMute(bool mute) const;

    void seek(qint64 position);
    void jumpForward(qint64 mseconds);
    void jumpBackward(qint64 mseconds);

    void jumpForwardExtraLarge();
    void jumpBackwardExtraLarge();
    void jumpForwardLarge();
    void jumpBackwardLarge();
    void jumpForwardMedium();
    void jumpBackwardMedium();
    void jumpForwardSmall();
    void jumpBackwardSmall();
    void jumpForwardExtraSmall();
    void jumpBackwardExtraSmall();

private slots:
    void mediaStatusChanged(QMediaPlayer::MediaStatus status);
    void videoWidgetStateChanged(QMediaPlayer::PlaybackState state);
    void setMetaData(const QUrl& media);

private:
    QMediaPlayer* m_player = nullptr;
    QAudioOutput* m_audioOutput = nullptr;
    QStackedWidget* m_container;
    QVideoWidget* m_videoWidget = nullptr;
    MetaData* m_metadata = nullptr;

    VuraMediaEngine* m_openGLWidget = nullptr;

    int m_volume;
    double m_playbackRate;

    bool m_usingVideoWidget = true;

};
