#pragma once

#include <QWidget>
#include <QLabel>
#include <QMediaPlayer>

#include "../components/ClickableLabel.h"
#include "../components/VideoSlider.h"


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

private slots:
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

    bool m_positionLabelShowRemainingTime = false;
    double m_playbackSpeed = 1.0;
    qint64 m_lastPosition = 0;
    qint64 m_duration = 0;

};
