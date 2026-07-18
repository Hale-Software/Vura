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
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QVideoSink>
#include <QString>
#include <QUrl>
#include <QDebug>


class PlaybackController : public QObject {
    Q_OBJECT

public:
    explicit PlaybackController(QObject* parent = nullptr, int volume = 100, double playbackRate = 1.0);
    ~PlaybackController() override;

    QMediaPlayer* getPlayer() const { return m_player; }
    QAudioOutput* getAudioOutput() const { return m_audioOutput; }

public slots:
    void showPlaylistContextMenu(const QPoint& pos);
    void playTrack(const QUrl& mediaUrl);
    void loadMedia(const QUrl& url) const;
    void loadMedia(const QString& fileName) const;
    void loadRemoteMedia(const QString& url) const;
    void loadRemoteMedia(const QUrl& url) const;
    void play() const;
    void pause() const;
    void togglePlayPause() const;
    void setPaused(bool paused) const;
    void stop() const;
    void restart() const;
    void setPosition(qint64 position) const;

    void setPlaybackRate(double rate) const;

    void changeVolume(int newVolume) const;
    void volumeUp() const;
    void volumeDown() const;
    void toggleMute() const;
    void setMute(bool mute) const;

    void seek(qint64 position) const;

    void jumpForwardExtraLarge() const;
    void jumpBackwardExtraLarge() const;
    void jumpForwardLarge() const;
    void jumpBackwardLarge() const;
    void jumpForwardMedium() const;
    void jumpBackwardMedium() const;
    void jumpForwardSmall() const;
    void jumpBackwardSmall() const;
    void jumpForwardExtraSmall() const;
    void jumpBackwardExtraSmall() const;

signals:
    void positionChanged(qint64 position);
    void durationChanged(qint64 duration);
    void stateChanged(QMediaPlayer::PlaybackState state);
    void bufferProgressChanged(float filled);
    void playbackRateChanged(qreal rate);
    void playingChanged(bool playing);
    void sourceChanged(const QUrl& media);
    void tracksChanged();
    void errorOccurred(const QString& errorMessage);

private:
    void setupConnections();

    QMediaPlayer* m_player;
    QAudioOutput* m_audioOutput;

    int m_volume;
    double m_playbackRate;

};
