#pragma once

#include <QObject>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QVideoSink>
#include <QString>
#include <QUrl>
#include <QDebug>


class PlaybackController : public QObject {
    Q_OBJECT

public:
    explicit PlaybackController(QObject* parent = nullptr, int volume = 100, double playbackRate = 1.0);
    ~PlaybackController() override;

    QMediaPlayer* getPlayer() const { return m_player; }
    QAudioOutput* getAudioOutput() const { return m_audioOutput; }

public slots:
    void showPlaylistContextMenu(const QPoint& pos);
    void playTrack(const QUrl& mediaUrl);
    void loadMedia(const QUrl& url);
    void loadMedia(const QString& fileName);
    void loadRemoteMedia(const QString& url);
    void loadRemoteMedia(const QUrl& url);
    void play() const;
    void pause() const;
    void togglePlayPause() const;
    void setPaused(bool paused) const;
    void stop() const;
    void restart() const;

    void setPlaybackRate(double rate) const;

    void changeVolume(int newVolume);
    void toggleMute() const;
    void setMute(bool mute) const;

    void seek(qint64 position) const;

signals:
    void positionChanged(qint64 position);
    void durationChanged(qint64 duration);
    void stateChanged(QMediaPlayer::PlaybackState state);
    void bufferProgressChanged(float filled);
    void playbackRateChanged(qreal rate);
    void playingChanged(bool playing);
    void sourceChanged(const QUrl& media);
    void tracksChanged();
    void errorOccurred(const QString& errorMessage);

private:
    void setupConnections();

    QMediaPlayer* m_player;
    QAudioOutput* m_audioOutput;

    int m_volume;
    double m_playbackRate;

};
