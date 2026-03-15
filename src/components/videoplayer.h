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
