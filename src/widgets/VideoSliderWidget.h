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

#include <QWidget>
#include <QLabel>
#include <QMediaPlayer>
#include <QPropertyAnimation>

#include "ClickableLabel.h"
#include "VideoSlider.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class VideoSliderWidget;
}
QT_END_NAMESPACE


class VideoSliderWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VideoSliderWidget(VideoSlider &videoSlider, QMediaPlayer &mediaPlayer, QWidget *parent = nullptr);
    ~VideoSliderWidget() override;

    void setVisible(bool visible) override;

private slots:
    void onAnimationFinished();
    void positionLabel_Clicked();
    void playbackRateChanged(qreal playbackRate) const;
    void updateTimestamps(qint64 currentPosition);
    void durationChanged(qint64 duration);
    void positionChanged(qint64 position);
    void statusChanged(QMediaPlayer::MediaStatus status);
    void clearTimestamps();

private:
    Ui::VideoSliderWidget *ui;
    VideoSlider *m_videoSlider;
    QMediaPlayer *m_mediaPlayer;

    QPropertyAnimation *animation;
    int targetHeight = 0;
    bool isTransitioning = false; // Prevents infinite loops

    bool m_positionLabelShowRemainingTime = false;
    double m_playbackSpeed = 1.0;
    qint64 m_lastPosition = 0;
    qint64 m_duration = 0;

};
