#pragma once

#include <QWidget>
#include <QLabel>
#include <QMediaPlayer>

#include "ClickableLabel.h"
#include "videoslider.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class VideoSliderContainer;
}
QT_END_NAMESPACE

class VideoSliderContainer : public QWidget
{
    Q_OBJECT

public:
    explicit VideoSliderContainer(VideoSlider &videoSlider, QMediaPlayer &mediaPlayer, QWidget *parent = nullptr);
    ~VideoSliderContainer() override;

private slots:
    void positionLabel_Clicked();
    void playbackRateChanged(qreal playbackRate) const;
    void updateTimestamps(qint64 currentPosition);
    void durationChanged(qint64 duration);
    void positionChanged(qint64 position);
    void statusChanged(QMediaPlayer::MediaStatus status);
    void clearTimestamps();

private:
    Ui::VideoSliderContainer *ui;
    VideoSlider *m_videoSlider;
    QMediaPlayer *m_mediaPlayer;

    bool m_positionLabelShowRemainingTime = false;
    double m_playbackSpeed = 1.0;
    qint64 m_lastPosition = 0;
    qint64 m_duration = 0;

};
