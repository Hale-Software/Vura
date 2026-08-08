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

#include "playback-controller.h"


PlaybackController::PlaybackController(QStackedWidget* container, QObject* parent, const int volume, const double playbackRate)
    : QObject(parent),
    m_container(container),
    m_volume(volume),
    m_playbackRate(playbackRate) {}

PlaybackController::~PlaybackController()
{
    // QObject parent-child relationship handles deletion of m_player and m_audioOutput
}

void PlaybackController::setVideoWidget(QVideoWidget *videoWidget)
{
    m_player = new QMediaPlayer(this);
    m_audioOutput = new QAudioOutput(this);
    m_videoWidget = videoWidget;

    m_player->setAudioOutput(m_audioOutput);
    m_player->setVideoOutput(m_videoWidget);

    const float linearVolume = QAudio::convertVolume(m_volume / 100.0f, QAudio::LogarithmicVolumeScale, QAudio::LinearVolumeScale);
    m_audioOutput->setVolume(linearVolume);

    m_player->setPlaybackRate(m_playbackRate);

    m_usingVideoWidget = true;

    connect(m_player, &QMediaPlayer::positionChanged, this, &PlaybackController::positionChanged);
    connect(m_player, &QMediaPlayer::durationChanged, this, &PlaybackController::durationChanged);
    connect(m_player, &QMediaPlayer::playbackStateChanged, this, &PlaybackController::videoWidgetStateChanged);
    //connect(m_player, &QMediaPlayer::bufferProgressChanged, this, &PlaybackController::bufferProgressChanged);
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

void PlaybackController::setOpenGLWidget(VuraMediaEngine *openGLWidget)
{
    m_openGLWidget = openGLWidget;
    m_audioOutput = new QAudioOutput(this);
    const float linearVolume = QAudio::convertVolume(m_volume / 100.0f, QAudio::LogarithmicVolumeScale, QAudio::LinearVolumeScale);
    m_audioOutput->setVolume(linearVolume);

    m_usingVideoWidget = false;

    connect(m_openGLWidget, &VuraMediaEngine::positionChanged, this, &PlaybackController::positionChanged);
    connect(m_openGLWidget, &VuraMediaEngine::durationChanged, this, &PlaybackController::durationChanged);
    connect(m_openGLWidget, &VuraMediaEngine::playbackStateChanged, this, &PlaybackController::stateChanged);
    //connect(m_openGLWidget, &VuraMediaEngine::bufferProgressChanged, this, &PlaybackController::bufferProgressChanged);
    //connect(m_openGLWidget, &VuraMediaEngine::playbackRateChanged, this, &PlaybackController::playbackRateChanged);
    //connect(m_openGLWidget, &VuraMediaEngine::playingChanged, this, &PlaybackController::playingChanged);
    //connect(m_openGLWidget, &VuraMediaEngine::sourceChanged, this, &PlaybackController::sourceChanged);
    //connect(m_openGLWidget, &VuraMediaEngine::tracksChanged, this, &PlaybackController::tracksChanged);
    //connect(m_openGLWidget, &VuraMediaEngine::mediaStatusChanged, this, &PlaybackController::mediaStatusChanged);

    // Handle errors cleanly
    connect(m_openGLWidget, &VuraMediaEngine::errorOccurred, this, [this](const QString &errorString) {
        emit errorOccurred(errorString);
    });
}

void PlaybackController::playTrack(const QUrl &mediaUrl)
{
    if (mediaUrl.isEmpty())
        return;

    if (m_usingVideoWidget) {
        m_player->stop();
        m_player->setSource(mediaUrl);
        emit sourceChanged(mediaUrl);
        m_player->play();

    } else {
        m_openGLWidget->stop();
        m_openGLWidget->loadMedia(mediaUrl);
        emit sourceChanged(mediaUrl);
        m_openGLWidget->play();
    }
}

void PlaybackController::loadMedia(const QUrl& url)
{
    if (url.isEmpty())
        return;

    if (m_usingVideoWidget) {
        const bool wasPlaying = m_player->playbackState() == QMediaPlayer::PlayingState;

        m_player->stop();
        m_player->setSource(url);
        emit sourceChanged(url.toLocalFile());
        if (wasPlaying)
            m_player->play();

    } else {
        const bool wasPlaying = m_openGLWidget->currentState() == PlaybackState::Playing;

        m_openGLWidget->stop();
        m_openGLWidget->loadMedia(url);
        emit sourceChanged(url);
        if (wasPlaying)
            m_openGLWidget->play();
    }
}

void PlaybackController::loadMedia(const QString& fileName)
{
    if (fileName.isEmpty())
        return;

    if (m_usingVideoWidget) {
        const bool wasPlaying = m_player->playbackState() == QMediaPlayer::PlayingState;

        m_player->stop();
        m_player->setSource(QUrl::fromLocalFile(fileName));
        emit sourceChanged(fileName);
        if (wasPlaying)
            m_player->play();

    } else {
        const bool wasPlaying = m_openGLWidget->currentState() == PlaybackState::Playing;

        m_openGLWidget->stop();
        m_openGLWidget->loadMedia(QUrl::fromLocalFile(fileName));
        emit sourceChanged(fileName);
        if (wasPlaying)
            m_openGLWidget->play();
    }
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
    if (m_usingVideoWidget) {
        if (m_player->playbackState() != QMediaPlayer::PlayingState) {
            m_player->play();
        }
    } else {
        if (m_openGLWidget->currentState() != PlaybackState::Playing) {
            m_openGLWidget->play();
        }
    }
}

void PlaybackController::pause() const
{
    if (m_usingVideoWidget) {
        if (m_player->playbackState() != QMediaPlayer::PausedState) {
            m_player->pause();
        }
    } else {
        if (m_openGLWidget->currentState() != PlaybackState::Paused) {
            m_openGLWidget->pause();
        }
    }
}

void PlaybackController::togglePlayPause() const
{
    if (m_usingVideoWidget) {
        if (m_player->playbackState() == QMediaPlayer::PlayingState) {
            m_player->pause();
        } else {
            m_player->play();
        }
    } else {
        if (m_openGLWidget->currentState() == Playing) {
            m_openGLWidget->pause();
        } else {
            m_openGLWidget->play();
        }
    }
}

void PlaybackController::setPaused(const bool paused) const
{
    if (m_usingVideoWidget) {
        if (paused) {
            m_player->pause();
        } else {
            m_player->play();
        }
    } else {
        if (paused) {
            m_openGLWidget->pause();
        } else {
            m_openGLWidget->play();
        }
    }
}

void PlaybackController::stop() const
{
    if (m_usingVideoWidget) {
        if (m_player->playbackState() != QMediaPlayer::StoppedState) {
            m_player->stop();
        }
    } else {
        if (m_openGLWidget->currentState() != Stopped) {
            m_openGLWidget->stop();
        }
    }
}

void PlaybackController::restart()
{
    if (m_usingVideoWidget) {
        m_player->setPosition(0);
        m_player->play();
    } else {
        m_openGLWidget->seek(0);
        m_openGLWidget->play();
    }
    //emit jumpCompleted();
}

void PlaybackController::setPosition(const qint64 position)
{
    if (m_usingVideoWidget) {
        m_player->setPosition(position);
    } else {
        m_openGLWidget->seek(position);
    }
    //emit jumpCompleted();
}

void PlaybackController::setPlaybackRate(const double rate)
{
    if (m_usingVideoWidget) {
        if (rate > 0.0) {
            m_player->setPlaybackRate(rate);
        } else {
            m_player->setPlaybackRate(0.0);
        }
    } else {

    }
    //emit jumpCompleted();
}

void PlaybackController::playbackRateFaster()
{
    const QSettings settings;
    const double playbackRateStep = settings.value("playbackSpeedAdjustment", 0.5).toDouble();
    const double maxPlaybackRate = settings.value("playbackSpeedMax", 10.0).toDouble();

    if (m_usingVideoWidget) {
        const double currentPlaybackRate = m_player->playbackRate();
        double newPlaybackRate = currentPlaybackRate + playbackRateStep;
        if (newPlaybackRate > maxPlaybackRate)
            newPlaybackRate = maxPlaybackRate;
        m_player->setPlaybackRate(newPlaybackRate);
    }

    emit playbackRateChanged(m_player->playbackRate());
}

void PlaybackController::playbackRateFasterFine()
{
    const QSettings settings;
    const double playbackRateStep = settings.value("playbackSpeedAdjustmentFine", 0.25).toDouble();
    const double maxPlaybackRate = settings.value("playbackSpeedMax", 10.0).toDouble();

    if (m_usingVideoWidget) {
        const double currentPlaybackRate = m_player->playbackRate();
        double newPlaybackRate = currentPlaybackRate + playbackRateStep;
        if (newPlaybackRate > maxPlaybackRate)
            newPlaybackRate = maxPlaybackRate;
        m_player->setPlaybackRate(newPlaybackRate);
    }

    emit playbackRateChanged(m_player->playbackRate());
}

void PlaybackController::playbackRateNormal()
{
    if (m_usingVideoWidget) {
        m_player->setPlaybackRate(1.0);
    }
    emit playbackRateChanged(m_player->playbackRate());
}

void PlaybackController::playbackRateSlower()
{
    const QSettings settings;
    const double playbackRateStep = settings.value("playbackSpeedAdjustment", 0.5).toDouble();

    if (m_usingVideoWidget) {
        const double currentPlaybackRate = m_player->playbackRate();
        double newPlaybackRate = currentPlaybackRate - playbackRateStep;
        if (newPlaybackRate < 0.1)
            newPlaybackRate = 0.1;
        m_player->setPlaybackRate(newPlaybackRate);
    }

    emit playbackRateChanged(m_player->playbackRate());
}

void PlaybackController::playbackRateSlowerFine()
{
    const QSettings settings;
    const double playbackRateStep = settings.value("playbackSpeedAdjustmentFine", 0.25).toDouble();

    if (m_usingVideoWidget) {
        const double currentPlaybackRate = m_player->playbackRate();
        double newPlaybackRate = currentPlaybackRate - playbackRateStep;
        if (newPlaybackRate < 0.1)
            newPlaybackRate = 0.1;
        m_player->setPlaybackRate(newPlaybackRate);
    }

    emit playbackRateChanged(m_player->playbackRate());
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
    QSettings settings;
    double volumeStep = settings.value("volumeStep", 0.1).toDouble();
    const float currentVolume = m_audioOutput->volume();
    if (currentVolume >= 1.0) return;
    m_audioOutput->setVolume(currentVolume + volumeStep);
}

void PlaybackController::volumeDown() const
{
    QSettings settings;
    double volumeStep = settings.value("volumeStep", 0.1).toDouble();
    const float currentVolume = m_audioOutput->volume();
    if (currentVolume <= 0.1) return;
    m_audioOutput->setVolume(currentVolume - volumeStep);
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
    if (m_usingVideoWidget) {
        m_player->setPosition(position);
    } else {
        m_openGLWidget->seek(position);
    }
    //emit jumpCompleted();
}

void PlaybackController::jumpForward(const qint64 mseconds)
{
    if (m_usingVideoWidget) {
        const qint64 currentPosition = m_player->position();
        const qint64 duration = m_player->duration();
        qint64 jumpTo = currentPosition + mseconds;
        if (jumpTo > duration) jumpTo = duration;
        m_player->setPosition(jumpTo);

    } else {
        const qint64 currentPosition = m_openGLWidget->position();
        const qint64 duration = m_openGLWidget->duration();
        qint64 jumpTo = currentPosition + mseconds;
        if (jumpTo > duration) jumpTo = duration;
        m_openGLWidget->seek(jumpTo);
    }

    //emit jumpCompleted();
}

void PlaybackController::jumpBackward(qint64 mseconds)
{
    if (m_usingVideoWidget) {
        const qint64 currentPosition = m_player->position();
        qint64 jumpTo = currentPosition - mseconds;
        if (jumpTo < 0) jumpTo = 0;
        m_player->setPosition(jumpTo);

    } else {
        const qint64 currentPosition = m_openGLWidget->position();
        qint64 jumpTo = currentPosition - mseconds;
        if (jumpTo < 0) jumpTo = 0;
        m_openGLWidget->seek(jumpTo);
    }

    //emit jumpCompleted();
}

void PlaybackController::jumpForwardExtraLarge()
{
    const QSettings settings;
    const int jump = settings.value("extraLargeJump", 90).toInt();
    jumpForward(jump * 1000);
}

void PlaybackController::jumpBackwardExtraLarge()
{
    const QSettings settings;
    const int jump = settings.value("extraLargeJump", 90).toInt();
    jumpBackward(jump * 1000);
}

void PlaybackController::jumpForwardLarge()
{
    const QSettings settings;
    const int jump = settings.value("largeJump", 30).toInt();
    jumpForward(jump * 1000);
}

void PlaybackController::jumpBackwardLarge()
{
    const QSettings settings;
    const int jump = settings.value("largeJump", 30).toInt();
    jumpBackward(jump * 1000);
}

void PlaybackController::jumpForwardMedium()
{
    const QSettings settings;
    const int jump = settings.value("mediumJump", 15).toInt();
    jumpForward(jump * 1000);
}

void PlaybackController::jumpBackwardMedium()
{
    const QSettings settings;
    const int jump = settings.value("mediumJump", 15).toInt();
    jumpBackward(jump * 1000);
}

void PlaybackController::jumpForwardSmall()
{
    const QSettings settings;
    const int jump = settings.value("smallJump", 5).toInt();
    jumpForward(jump * 1000);
}

void PlaybackController::jumpBackwardSmall()
{
    const QSettings settings;
    const int jump = settings.value("smallJump", 5).toInt();
    jumpBackward(jump * 1000);
}

void PlaybackController::jumpForwardExtraSmall()
{
    const QSettings settings;
    const int jump = settings.value("extraSmallJump", 1).toInt();
    jumpForward(jump * 1000);
}

void PlaybackController::jumpBackwardExtraSmall()
{
    const QSettings settings;
    const int jump = settings.value("extraSmallJump", 1).toInt();
    jumpBackward(jump * 1000);
}

void PlaybackController::mediaStatusChanged(const QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::EndOfMedia) {
        emit mediaEnded();
    } else if (status == QMediaPlayer::NoMedia) {
        m_container->setCurrentIndex(0);
    } else {
        if (m_usingVideoWidget) {
            m_container->setCurrentIndex(1);
        } else {
            m_container->setCurrentIndex(2);
        }
    }
}

void PlaybackController::videoWidgetStateChanged(const QMediaPlayer::PlaybackState state)
{
    if (state == QMediaPlayer::StoppedState) {
        emit stateChanged(Stopped);
    } else if (state == QMediaPlayer::PausedState) {
        emit stateChanged(Paused);
    } else if (state == QMediaPlayer::PlayingState) {
        emit stateChanged(Playing);
    }
}
