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

#include "PlaybackController.h"


PlaybackController::PlaybackController(QStackedWidget* container, QVideoWidget* videoWidget, const int volume, const double playbackRate, QObject* parent)
    : QObject(parent),
    m_player(new QMediaPlayer(this)),
    m_audioOutput(new QAudioOutput(this)),
    m_container(container),
    m_videoWidget(videoWidget),
    m_volume(volume),
    m_playbackRate(playbackRate)
{
    m_player->setAudioOutput(m_audioOutput);
    m_player->setVideoOutput(m_videoWidget);

    const float linearVolume = QAudio::convertVolume(m_volume / 100.0f, QAudio::LogarithmicVolumeScale, QAudio::LinearVolumeScale);
    m_audioOutput->setVolume(linearVolume);

    m_player->setPlaybackRate(m_playbackRate);

    setupConnections();
}

PlaybackController::~PlaybackController()
{
    // QObject parent-child relationship handles deletion of m_player and m_audioOutput
}

void PlaybackController::setupConnections()
{
    connect(m_player, &QMediaPlayer::positionChanged, this, &PlaybackController::positionChanged);
    connect(m_player, &QMediaPlayer::durationChanged, this, &PlaybackController::durationChanged);
    connect(m_player, &QMediaPlayer::playbackStateChanged, this, &PlaybackController::stateChanged);
    connect(m_player, &QMediaPlayer::bufferProgressChanged, this, &PlaybackController::bufferProgressChanged);
    connect(m_player, &QMediaPlayer::playbackRateChanged, this, &PlaybackController::playbackRateChanged);
    connect(m_player, &QMediaPlayer::playingChanged, this, &PlaybackController::playingChanged);
    connect(m_player, &QMediaPlayer::sourceChanged, this, &PlaybackController::sourceChanged);
    connect(m_player, &QMediaPlayer::tracksChanged, this, &PlaybackController::tracksChanged);
    connect(m_player, &QMediaPlayer::mediaStatusChanged, this, &PlaybackController::mediaStatusChanged);

    // Handle errors cleanly
    connect(m_player, &QMediaPlayer::errorOccurred, this, [this](const QMediaPlayer::Error error, const QString &errorString) {
        Q_UNUSED(error);
        emit errorOccurred(errorString);
    });
}

void PlaybackController::showPlaylistContextMenu(const QPoint &pos)
{
    
}

void PlaybackController::playTrack(const QUrl &mediaUrl)
{
    if (mediaUrl.isEmpty())
        return;

    const bool wasPlaying = m_player->playbackState() == QMediaPlayer::PlayingState;

    m_player->stop();
    m_player->setSource(mediaUrl);
    emit sourceChanged(mediaUrl);
    if (wasPlaying)
        m_player->play();
}

void PlaybackController::loadMedia(const QUrl& url) const
{
    if (url.isEmpty())
        return;

    const bool wasPlaying = m_player->playbackState() == QMediaPlayer::PlayingState;

    m_player->stop();
    m_player->setSource(url);
    if (wasPlaying)
        m_player->play();
}

void PlaybackController::loadMedia(const QString& fileName) const
{
    const bool wasPlaying = m_player->playbackState() == QMediaPlayer::PlayingState;
    m_player->setSource(QUrl::fromLocalFile(fileName));
    if (wasPlaying)
        m_player->play();
}

void PlaybackController::loadRemoteMedia(const QString &url) const
{
    const bool wasPlaying = m_player->playbackState() == QMediaPlayer::PlayingState;
    m_player->setSource(QUrl(url));
    if (wasPlaying)
        m_player->play();
}

void PlaybackController::loadRemoteMedia(const QUrl &url) const
{
    const bool wasPlaying = m_player->playbackState() == QMediaPlayer::PlayingState;
    m_player->setSource(url);
    if (wasPlaying)
        m_player->play();
}

void PlaybackController::play() const
{
    if (m_player->playbackState() != QMediaPlayer::PlayingState) {
        m_player->play();
    }
}

void PlaybackController::pause() const
{
    if (m_player->playbackState() == QMediaPlayer::PlayingState) {
        m_player->pause();
    }
}

void PlaybackController::togglePlayPause() const
{
    if (m_player->playbackState() == QMediaPlayer::PlayingState) {
        m_player->pause();
    } else {
        m_player->play();
    }
}

void PlaybackController::setPaused(const bool paused) const
{
    if (paused) {
        m_player->pause();

    } else {
        m_player->play();
    }
}

void PlaybackController::stop() const
{
    m_player->stop();
}

void PlaybackController::restart()
{
    m_player->setPosition(0);
    m_player->play();
    emit jumpCompleted();
}

void PlaybackController::setPosition(const qint64 position)
{
    m_player->setPosition(position);
    emit jumpCompleted();
}

void PlaybackController::setPlaybackRate(const double rate)
{
    if (rate > 0.0) {
        m_player->setPlaybackRate(rate);
    } else {
        m_player->setPlaybackRate(0.0);
    }
    emit jumpCompleted();
}

void PlaybackController::changeVolume(const int newVolume) const
{
    // Qt6 QAudioOutput volume is a linear float from 0.0 to 1.0
    // Assuming UI sends a value from 0 to 100
    const float linearVolume = QAudio::convertVolume(newVolume / 100.0f, QAudio::LogarithmicVolumeScale, QAudio::LinearVolumeScale);
    m_audioOutput->setVolume(linearVolume);
}

void PlaybackController::volumeUp() const
{
    const float currentVolume = m_audioOutput->volume();
    if (currentVolume >= 1.0) return;
    m_audioOutput->setVolume(currentVolume + 0.1);
}

void PlaybackController::volumeDown() const
{
    const float currentVolume = m_audioOutput->volume();
    if (currentVolume <= 0.1) return;
    m_audioOutput->setVolume(currentVolume - 0.1);
}

void PlaybackController::toggleMute() const
{
    m_audioOutput->setMuted(!m_audioOutput->isMuted());
}

void PlaybackController::setMute(const bool mute) const
{
    m_audioOutput->setMuted(mute);
}

void PlaybackController::seek(const qint64 position)
{
    m_player->setPosition(position);
    emit jumpCompleted();
}

void PlaybackController::jumpForwardExtraLarge()
{
    const qint64 currentPosition = m_player->position();
    const qint64 duration = m_player->duration();
    qint64 jumpTo = currentPosition + 90000;
    if (jumpTo > duration) jumpTo = duration;
    m_player->setPosition(jumpTo);
    emit jumpCompleted();
}

void PlaybackController::jumpBackwardExtraLarge()
{
    const qint64 currentPosition = m_player->position();
    qint64 jumpTo = currentPosition - 90000;
    if (jumpTo < 0) jumpTo = 0;
    m_player->setPosition(jumpTo);
    emit jumpCompleted();
}

void PlaybackController::jumpForwardLarge()
{
    const qint64 currentPosition = m_player->position();
    const qint64 duration = m_player->duration();
    qint64 jumpTo = currentPosition + 60000;
    if (jumpTo > duration) jumpTo = duration;
    m_player->setPosition(jumpTo);
    emit jumpCompleted();
}

void PlaybackController::jumpBackwardLarge()
{
    const qint64 currentPosition = m_player->position();
    qint64 jumpTo = currentPosition - 60000;
    if (jumpTo < 0) jumpTo = 0;
    m_player->setPosition(jumpTo);
    emit jumpCompleted();
}

void PlaybackController::jumpForwardMedium()
{
    const qint64 currentPosition = m_player->position();
    const qint64 duration = m_player->duration();
    qint64 jumpTo = currentPosition + 30000;
    if (jumpTo > duration) jumpTo = duration;
    m_player->setPosition(jumpTo);
    emit jumpCompleted();
}

void PlaybackController::jumpBackwardMedium()
{
    const qint64 currentPosition = m_player->position();
    qint64 jumpTo = currentPosition - 30000;
    if (jumpTo < 0) jumpTo = 0;
    m_player->setPosition(jumpTo);
    emit jumpCompleted();
}

void PlaybackController::jumpForwardSmall()
{
    const qint64 currentPosition = m_player->position();
    const qint64 duration = m_player->duration();
    qint64 jumpTo = currentPosition + 15000;
    if (jumpTo > duration) jumpTo = duration;
    m_player->setPosition(jumpTo);
    emit jumpCompleted();
}

void PlaybackController::jumpBackwardSmall()
{
    const qint64 currentPosition = m_player->position();
    qint64 jumpTo = currentPosition - 15000;
    if (jumpTo < 0) jumpTo = 0;
    m_player->setPosition(jumpTo);
    emit jumpCompleted();
}

void PlaybackController::jumpForwardExtraSmall()
{
    const qint64 currentPosition = m_player->position();
    const qint64 duration = m_player->duration();
    qint64 jumpTo = currentPosition + 5000;
    if (jumpTo > duration) jumpTo = duration;
    m_player->setPosition(jumpTo);
    emit jumpCompleted();
}

void PlaybackController::jumpBackwardExtraSmall()
{
    const qint64 currentPosition = m_player->position();
    qint64 jumpTo = currentPosition - 5000;
    if (jumpTo < 0) jumpTo = 0;
    m_player->setPosition(jumpTo);
    emit jumpCompleted();
}

void PlaybackController::mediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::NoMedia) {
        m_container->setCurrentIndex(0);
    } else {
        m_container->setCurrentIndex(1);
    }
}
