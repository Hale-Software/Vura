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

#pragma once

#include <QWidget>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QVideoSink>
#include <QVideoFrame>
#include <QMediaCaptureSession>
#include <QUrl>
#include <QImage>
#include <QRect>
#include <QPainter>
#include <QDebug>


class VideoPlayer : public QWidget {
    Q_OBJECT

public:
    explicit VideoPlayer(QWidget *parent = nullptr);
    void setSource(const QUrl &source);
    void setAudioOutput(QAudioOutput *audioOutput);

private slots:
    void handleFrame(const QVideoFrame &frame);
    void subtitleTextChanged(const QString &subtitleText);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QMediaPlayer *m_player;
    QAudioOutput *m_audioOutput;
    QVideoSink *m_videoSink;
    QImage m_image;
    QRect m_rect;
    QString m_subtitleText;

};
