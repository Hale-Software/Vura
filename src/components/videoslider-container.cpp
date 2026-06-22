#include "videoslider-container.h"
#include "ui_VideoSlider-Container.h"


VideoSliderContainer::VideoSliderContainer(VideoSlider &videoSlider, QMediaPlayer &mediaPlayer, QWidget *parent)
    : QWidget(parent), ui(new Ui::VideoSliderContainer), m_videoSlider(&videoSlider), m_mediaPlayer(&mediaPlayer)
{
    ui->setupUi(this);

    connect(ui->position, &ClickableLabel::clicked, this, &VideoSliderContainer::positionLabel_Clicked);
    connect(m_mediaPlayer, &QMediaPlayer::mediaStatusChanged, this, &VideoSliderContainer::statusChanged);
    connect(m_mediaPlayer, &QMediaPlayer::playbackRateChanged, this, &VideoSliderContainer::playbackRateChanged);
    connect(m_mediaPlayer, &QMediaPlayer::durationChanged, this, &VideoSliderContainer::durationChanged);
    connect(m_mediaPlayer, &QMediaPlayer::positionChanged, this, &VideoSliderContainer::positionChanged);

    ui->position->setToolTip(tr("Elapsed time"));
    ui->duration->setToolTip(tr("Total/Remaining time\n -Click to toggle between total and remaining time"));
    ui->playbackRate->setToolTip(tr("Playback speed"));

    ui->horizontalLayout->removeWidget(ui->placeholder);
    ui->horizontalLayout->insertWidget(1, m_videoSlider);
    ui->horizontalLayout->setStretch(1, 2);
}

VideoSliderContainer::~VideoSliderContainer()
{
    delete ui;
}

void VideoSliderContainer::positionLabel_Clicked()
{
    if (m_positionLabelShowRemainingTime) {
        m_positionLabelShowRemainingTime = false;

    } else {
        m_positionLabelShowRemainingTime = true;
    }

    if (m_lastPosition > 0)
        updateTimestamps(m_lastPosition);
}

void VideoSliderContainer::statusChanged(const QMediaPlayer::MediaStatus status)
{
    switch (status) {
        case QMediaPlayer::NoMedia:
            clearTimestamps();
            break;

        case QMediaPlayer::LoadedMedia:
            updateTimestamps(m_mediaPlayer->position() / 1000);
            break;

        case QMediaPlayer::InvalidMedia:
            clearTimestamps();
            break;

        default:
            break;
    }
}

void VideoSliderContainer::durationChanged(const qint64 duration)
{
    m_duration = duration / 1000;
    m_videoSlider->setMaximum(static_cast<int>(duration));
}

void VideoSliderContainer::positionChanged(const qint64 position)
{
    if (!m_videoSlider->GetSliderPressed())
        m_videoSlider->setValue(static_cast<int>(position));

    updateTimestamps(position / 1000);
}

void VideoSliderContainer::playbackRateChanged(const qreal playbackRate) const
{
    ui->playbackRate->setText("x" + QString::number(playbackRate));
}

void VideoSliderContainer::updateTimestamps(const qint64 currentPosition)
{
    QString durationString;
    QString positionString;

    if (currentPosition || m_duration) {
        m_lastPosition = currentPosition;
        const int position = static_cast<int>(currentPosition);
        const int duration = static_cast<int>(m_duration);

        QString format = "mm:ss";
        if (m_duration > 3600)
            format = "hh:mm:ss";

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

void VideoSliderContainer::clearTimestamps()
{
    ui->duration->setText("--:--");
    ui->position->setText("--:--");
    m_lastPosition = 0;
    m_duration = 0;
}
