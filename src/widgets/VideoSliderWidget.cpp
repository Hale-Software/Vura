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

#include "VideoSliderWidget.h"
#include "ui_VideoSliderWidget.h"

#include <QEasingCurve>


VideoSliderWidget::VideoSliderWidget(VideoSlider &videoSlider, PlaybackController &playbackController, QWidget *parent)
    : QWidget(parent),
      ui(new Ui::VideoSliderWidget),
      m_videoSlider(&videoSlider),
      m_playbackController(&playbackController)
{
    ui->setupUi(this);

    animation = new QPropertyAnimation(this, "maximumHeight", this);
    animation->setDuration(300);
    animation->setEasingCurve(QEasingCurve::InOutQuad);
    connect(animation, &QPropertyAnimation::finished, this, &VideoSliderWidget::onAnimationFinished);

    connect(ui->position, &ClickableLabel::clicked, this, &VideoSliderWidget::positionLabel_Clicked);
    connect(m_playbackController, &PlaybackController::playbackRateChanged, this, &VideoSliderWidget::playbackRateChanged);
    connect(m_playbackController, &PlaybackController::durationChanged, this, &VideoSliderWidget::durationChanged);
    connect(m_playbackController, &PlaybackController::positionChanged, this, &VideoSliderWidget::positionChanged);

    ui->position->setToolTip(tr("Elapsed time"));
    ui->duration->setToolTip(tr("Total/Remaining time\n -Click to toggle between total and remaining time"));
    ui->playbackRate->setToolTip(tr("Playback speed"));

    ui->horizontalLayout->removeWidget(ui->placeholder);
    ui->horizontalLayout->insertWidget(1, m_videoSlider);
    ui->horizontalLayout->setStretch(1, 2);
}

VideoSliderWidget::~VideoSliderWidget()
{
    delete ui;
}

void VideoSliderWidget::setVisible(const bool visible)
{
    if (isTransitioning) {
        QWidget::setVisible(visible);
        return;
    }

    if (visible) {
        if (targetHeight <= 0) {
            targetHeight = sizeHint().height();
        }

        isTransitioning = true;
        QWidget::setVisible(true);
        isTransitioning = false;
        animation->stop();
        animation->setStartValue(height());
        animation->setEndValue(targetHeight);
        animation->start();
    } else {
        targetHeight = sizeHint().height();
        animation->stop();
        animation->setStartValue(height());
        animation->setEndValue(0);
        animation->start();
    }
}

void VideoSliderWidget::onAnimationFinished()
{
    if (maximumHeight() == 0) {
        isTransitioning = true;
        QWidget::setVisible(false);
        isTransitioning = false;
        setMaximumHeight(16777215);
    }
}

void VideoSliderWidget::positionLabel_Clicked()
{
    if (m_positionLabelShowRemainingTime) {
        m_positionLabelShowRemainingTime = false;
    } else {
        m_positionLabelShowRemainingTime = true;
    }

    if (m_lastPosition > 0)
        updateTimestamps(m_lastPosition);
}

void VideoSliderWidget::statusChanged(const QMediaPlayer::MediaStatus status)
{
    switch (status) {
        case QMediaPlayer::NoMedia:
            clearTimestamps();
            break;

        case QMediaPlayer::LoadedMedia:
            //updateTimestamps(m_mediaPlayer->position() / 1000);
            break;

        case QMediaPlayer::InvalidMedia:
            clearTimestamps();
            break;

        default:
            break;
    }
}

void VideoSliderWidget::durationChanged(const qint64 duration)
{
    m_duration = duration / 1000;
    m_videoSlider->setMaximum(static_cast<int>(duration));
}

void VideoSliderWidget::positionChanged(const qint64 position)
{
    if (!m_videoSlider->GetSliderPressed())
        m_videoSlider->setValue(static_cast<int>(position));

    updateTimestamps(position / 1000);
}

void VideoSliderWidget::playbackRateChanged(const qreal playbackRate)
{
    m_playbackSpeed = playbackRate;
    ui->playbackRate->setText("x" + QString::number(m_playbackSpeed));
}

void VideoSliderWidget::updateTimestamps(const qint64 currentPosition)
{
    QString durationString;
    QString positionString;

    if (currentPosition || m_duration) {
        m_lastPosition = currentPosition;
        const int position = static_cast<int>(currentPosition);
        const int duration = static_cast<int>(m_duration);

        const QString format = "hh:mm:ss";

        if (m_positionLabelShowRemainingTime) {
            const int remainingInfo = duration - position;

            const QTime remainingTime(
                (remainingInfo / 3600) % 60,
                (remainingInfo / 60) % 60,
                remainingInfo % 60,
                (remainingInfo * 1000) % 1000);

            positionString = remainingTime.toString(format);

        } else {
            const QTime currentTime(
            (position / 3600) % 60,
            (position / 60) % 60,
            position % 60,
            (position * 1000) % 1000);

            positionString = currentTime.toString(format);
        }

        const QTime totalTime(
                (duration / 3600) % 60,
                (duration / 60) % 60,
                duration % 60,
                (duration * 1000) % 1000);

        durationString = totalTime.toString(format);

    } else {
        positionString = "--:--";
        durationString = "--:--";
    }

    ui->duration->setText(durationString);
    ui->position->setText(positionString);
    ui->playbackRate->setText("x" + QString::number(m_playbackSpeed));
}

void VideoSliderWidget::clearTimestamps()
{
    ui->duration->setText("--:--");
    ui->position->setText("--:--");
    m_lastPosition = 0;
    m_duration = 0;
}
