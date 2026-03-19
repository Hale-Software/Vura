/*******************************************************************************
     Copyright (c) 2026.  by Andrew Hale <halea2196@gmail.com>

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

#include "videoplayer.h"


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
