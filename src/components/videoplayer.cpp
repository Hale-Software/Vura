#include "videoplayer.h"

#include <QPainter>


VideoPlayer::VideoPlayer(QWidget *parent) : QWidget(parent)
{
    m_player = new QMediaPlayer(this);
    m_audioOutput = new QAudioOutput(this);
    m_videoSink = new QVideoSink(this);

    m_player->setAudioOutput(m_audioOutput);
    m_player->setVideoOutput(m_videoSink);

    connect(m_videoSink, &QVideoSink::videoFrameChanged, this, &VideoPlayer::handleFrame);

    m_rect = QRect((this->width() / 2), 15, (this->width() - 30), 50);
}

void VideoPlayer::setSource(const QUrl &source)
{
    m_player->setSource(source);
    m_player->play();
    m_videoSink->setSubtitleText("Hello");
    m_subtitleText = "Hello";
}

void VideoPlayer::setAudioOutput(QAudioOutput *audioOutput)
{
    m_audioOutput = audioOutput;
}

void VideoPlayer::handleFrame(const QVideoFrame &frame)
{
    if (frame.isValid()) {
        m_image = frame.toImage();
        update();
    }
}

void VideoPlayer::subtitleTextChanged(const QString &subtitleText)
{
    //m_subtitleText = subtitleText;
    //update();
}

void VideoPlayer::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    QPixmap pixmap(QPixmap::fromImage(m_image));
    painter.drawPixmap(this->rect(), pixmap);

    //painter.setPen(Qt::red);
    //painter.drawText(m_rect, Qt::AlignHCenter | Qt::AlignBottom | Qt::TextWordWrap, m_subtitleText);
}
