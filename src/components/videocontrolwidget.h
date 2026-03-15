#ifndef HPLAYER_VIDEOCONTROLWIDGET_H
#define HPLAYER_VIDEOCONTROLWIDGET_H

#include <QWidget>
#include <QSettings>
#include <QMediaPlayer>
#include <QSlider>
#include <QToolButton>
#include <QLabel>
#include <QIcon>
#include <QStyle>
#include <QPixmap>
#include <QPainter>
#include <QColor>
#include <QGuiApplication>
#include <QStyleHints>
#include <QSvgRenderer>
#include <QDebug>

QT_BEGIN_NAMESPACE

namespace Ui {
    class VideoControlWidget;
}

QT_END_NAMESPACE

enum class LoopOption {
    LoopAll,
    LoopOne,
    LoopNone
};

class VideoControlWidget : public QWidget {
    Q_OBJECT

public:
    explicit VideoControlWidget(QWidget *parent = nullptr);
    ~VideoControlWidget() override;

    QMediaPlayer::PlaybackState state() const;
    float volume() const;
    bool isMuted() const;

public slots:
    void setState(QMediaPlayer::PlaybackState state);
    void setVolume(float volume);
    void setMuted(bool muted);
    void refreshUI();

signals:
    void play();
    void pause();
    void stop();
    void next();
    void previous();
    void fullScreen();
    void togglePlaylist();
    void loopAll();
    void loopOne();
    void loopNone();
    void shuffle();
    void changeVolume(float volume);
    void changeMuting(bool muting);
    void changeRate(qreal rate);

private slots:
    void playClicked();
    void fullScreenClicked();
    void playlistClicked();
    void loopClicked();
    void shuffleClicked();
    void muteClicked();
    void onVolumeSliderValueChanged();

private:
    Ui::VideoControlWidget *ui;
    QMediaPlayer::PlaybackState m_playerState = QMediaPlayer::StoppedState;
    bool m_playerMuted = false;
    LoopOption m_loopOption = LoopOption::LoopAll;
    bool m_isShuffle = false;
    QSlider *m_volumeSlider;
    QSlider *m_progressSlider;
    int m_volumeLevel = 100;

    QIcon setButtonIcon(const QString &buttonName, const QString &theme);

};


#endif //HPLAYER_VIDEOCONTROLWIDGET_H