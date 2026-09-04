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

#include "VideoControlWidget.h"
#include "ui_VideoControlWidget.h"


VideoControlWidget::VideoControlWidget(QWidget *parent) : QWidget(parent), ui(new Ui::VideoControlWidget)
{
    ui->setupUi(this);

    connect(ui->playButton, &QToolButton::clicked, this, &VideoControlWidget::playButton_Clicked);
    connect(ui->stopButton, &QToolButton::clicked, this, &VideoControlWidget::stop);
    connect(ui->nextButton, &QToolButton::clicked, this, &VideoControlWidget::next);
    connect(ui->previousButton, &QToolButton::clicked, this, &VideoControlWidget::previous);
    connect(ui->fullscreenButton, &QToolButton::clicked, this, &VideoControlWidget::fullScreenClicked);
    connect(ui->playlistButton, &QToolButton::clicked, this, &VideoControlWidget::playlistClicked);
    connect(ui->loopButton, &QToolButton::clicked, this, &VideoControlWidget::loopClicked);
    connect(ui->shuffleButton, &QToolButton::clicked, this, &VideoControlWidget::shuffleClicked);
    connect(ui->volumeLabel, &ClickableLabel::clicked, this, &VideoControlWidget::volumeLabelClicked);
    connect(ui->settingsButton, &QPushButton::clicked, this, &VideoControlWidget::settingsRequested);
    connect(ui->subtitlesButton, &QPushButton::clicked, this, &VideoControlWidget::subtitlesRequested);

    connect(ui->volumeSlider, &QSlider::valueChanged, this, &VideoControlWidget::onVolumeSliderValueChanged);

    setState(QMediaPlayer::StoppedState);

    refreshUI();
}

VideoControlWidget::~VideoControlWidget()
{
    delete ui;
}

QMediaPlayer::PlaybackState VideoControlWidget::state() const
{
    return m_playerState;
}

float VideoControlWidget::volume() const
{
    const qreal linearVolume = QAudio::convertVolume(ui->volumeSlider->value() / static_cast<qreal>(100),
                            QAudio::LogarithmicVolumeScale, QAudio::LinearVolumeScale);

    return linearVolume;
}

bool VideoControlWidget::isMuted() const { return m_playerMuted; }

void VideoControlWidget::setState(const QMediaPlayer::PlaybackState state)
{
    m_playerState = state;
    refreshUI();
}

void VideoControlWidget::setVolume(const double volume)
{
    const qreal logarithmicVolume = QAudio::convertVolume(volume, QAudio::LinearVolumeScale, QAudio::LogarithmicVolumeScale);
    ui->volumeSlider->setValue(qRound(logarithmicVolume * 100));

    refreshUI();
}

void VideoControlWidget::setMuted(const bool muted)
{
    m_playerMuted = muted;
    refreshUI();
}

void VideoControlWidget::refreshUI()
{
    const double volumeDouble = ui->volumeSlider->value();
    const int val = qRound(volumeDouble);
    const QString volumeString = QString::number(val) + "%";
    ui->volumeLabel->setText(volumeString);
    m_volumeLevel = val;

    const QSettings settings;
    const QString theme = settings.value("theme", "System").toString();

    if (m_playerState == QMediaPlayer::StoppedState || m_playerState == QMediaPlayer::PausedState) {
        ui->playButton->setIcon(setButtonIcon("play", theme));
    } else {
        ui->playButton->setIcon(setButtonIcon("pause", theme));
    }

    if (m_loopOption == LoopOption::LoopOne) {
        ui->loopButton->setIcon(setButtonIcon("loop-one", theme));
    } else {
        ui->loopButton->setIcon(setButtonIcon("loop", theme));
    }
/*
    if (m_playerMuted) {
        ui->muteButton->setIcon(setButtonIcon("mute", theme));
    } else {
        if (m_volumeLevel <= 50) {
            ui->muteButton->setIcon(setButtonIcon("volume-low", theme));
        } else {
            ui->muteButton->setIcon(setButtonIcon("volume-high", theme));
        }
    }
*/
    ui->previousButton->setIcon(setButtonIcon("back", theme));
    ui->stopButton->setIcon(setButtonIcon("stop", theme));
    ui->nextButton->setIcon(setButtonIcon("next", theme));
    ui->fullscreenButton->setIcon(setButtonIcon("fullscreen", theme));
    ui->playlistButton->setIcon(setButtonIcon("playlist", theme));
    ui->shuffleButton->setIcon(setButtonIcon("shuffle", theme));

    if (m_loopOption == LoopOption::LoopNone) {
        ui->loopButton->setStyleSheet("QToolButton { border: none; }");
    } else {
        ui->loopButton->setStyleSheet("QToolButton { border-width: 1px; border-style: solid; border-color: rgb(0, 140, 255); }");
    }

    if (m_isShuffle) {
        ui->shuffleButton->setStyleSheet("QToolButton { border-width: 1px; border-style: solid; border-color: rgb(0, 140, 255); }");
    } else {
        ui->shuffleButton->setStyleSheet("QToolButton { border: none; }");
    }
}

void VideoControlWidget::playButton_Clicked()
{
    if (m_playerState == QMediaPlayer::PlayingState) {
        emit pause();
    } else {
        emit play();
    }
}

void VideoControlWidget::fullScreenClicked()
{
    emit fullScreen();
}

void VideoControlWidget::playlistClicked()
{
    emit togglePlaylist();
}

void VideoControlWidget::loopClicked()
{
    if (m_loopOption == LoopOption::LoopAll) {
        emit setLoop(1);
        m_loopOption = LoopOption::LoopOne;
    } else if (m_loopOption == LoopOption::LoopOne) {
        emit setLoop(2);
        m_loopOption = LoopOption::LoopNone;
    } else {
        emit setLoop(3);
        m_loopOption = LoopOption::LoopAll;
    }

    refreshUI();
}

void VideoControlWidget::shuffleClicked()
{
    if (m_isShuffle) {
        m_isShuffle = false;
    } else {
        m_isShuffle = true;
    }
    emit shuffle();
    refreshUI();
}

void VideoControlWidget::volumeLabelClicked()
{
    emit changeMuting(!m_playerMuted);
}

void VideoControlWidget::settingsButtonClicked()
{

}

void VideoControlWidget::onVolumeSliderValueChanged()
{
    emit changeVolume(volume());
    refreshUI();
}

QIcon VideoControlWidget::setButtonIcon(const QString &buttonName, const QString &theme)
{
    qDebug() << "Setting button icon...";

    if (buttonName == "play") {
        return QIcon(":/icons/play-white.png");
    }

    if (buttonName == "pause") {
        return QIcon(":/icons/pause-white.png");
    }

    if (buttonName == "back") {
        return QIcon(":/icons/back-white.png");
    }

    if (buttonName == "stop") {
        return QIcon(":/icons/stop-white.png");
    }

    if (buttonName == "next") {
        return QIcon(":/icons/next-white.png");
    }

    if (buttonName == "fullscreen") {
        return QIcon(":/icons/maximize-white.png");
    }

    //if (buttonName == "fullscreen-exit") {
    //    return QIcon(":/icons/-white.png");
    //}

    if (buttonName == "playlist") {
        return QIcon(":/icons/playlist-white.png");
    }

    if (buttonName == "loop") {
        return QIcon(":/icons/repeat-white.png");
    }

    //if (buttonName == "loop-one") {
    //    return QIcon(":/icons/-white.png");
    //}

    if (buttonName == "shuffle") {
        return QIcon(":/icons/shuffle-white.png");
    }

    if (buttonName == "mute") {
        return QIcon(":/icons/mute-white.png");
    }

    if (buttonName == "volume-low") {
        return QIcon(":/icons/low-volume-white.png");
    }

    //if (buttonName == "volume-medium") {
    //    return QIcon(":/icons/-white.png");
    //}

    if (buttonName == "volume-high") {
        return QIcon(":/icons/volume-up-white.png");
    }

    if (buttonName == "subtitles")
        return QIcon(":/icons/subtitles-white.png");

    qCritical() << "Button name doesnt have a file assigned to it.";
    return QIcon();

/*
    if (theme == "System") {
        qDebug() << "Using system theme for video control buttons.";
        if (QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Light) {
            qDebug() << "Windows is in Light Mode.";
            color = QColor(Qt::black);
        } else if (QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark) {
            qDebug() << "Windows is in Dark Mode.";
            color = QColor(Qt::white);
        } else {
            qDebug() << "Could not determine Windows Theme. Using Dark Mode.";
            color = QColor(Qt::black);
        }
    } else if (theme == "Light") {
        qDebug() << "Using Light Theme.";
        color = QColor(Qt::black);
    } else if (theme == "Dark") {
        qDebug() << "Using Dark Theme.";
        color = QColor(Qt::white);
    } else {
        qCritical() << "Could not determine Color Theme.";
        return QIcon();
    }

    QPainter painter(&pixmap);
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn); // This mode colors only the non-transparent parts.
    painter.fillRect(pixmap.rect(), color);
    painter.end();

    return QIcon(pixmap);
*/
}
