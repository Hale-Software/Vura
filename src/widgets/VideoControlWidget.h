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
#include <QIcon>

#include <libvura/models/types.h>

#include "ClickableLabel.h"


QT_BEGIN_NAMESPACE
namespace Ui { class VideoControlWidget; }
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

    media::PlaybackState state() const;
    int volume() const;
    bool isMuted() const;

public slots:
    void setState(media::PlaybackState state);
    void setVolume(int volume);
    void setMuted(bool muted);
    void refreshUI();

signals:
    void play();
    void pause();
    void stop();
    void next();
    void previous();
    void fullScreen();
    void settingsRequested();
    void subtitlesRequested();
    void togglePlaylist();
    void setLoop(int loopOption);
    void shuffle();
    void changeVolume(int volume);
    void changeMuting(bool muting);
    void changeRate(qreal rate);

private slots:
    void playButton_Clicked();
    void fullScreenClicked();
    void playlistClicked();
    void loopClicked();
    void shuffleClicked();
    void volumeLabelClicked();
    void settingsButtonClicked();

private:
    static QIcon setButtonIcon(const QString &buttonName, const int &theme);

    Ui::VideoControlWidget *ui;
    media::PlaybackState m_playerState = media::PlaybackState::Stopped;
    bool m_playerMuted = false;
    LoopOption m_loopOption = LoopOption::LoopAll;
    bool m_isShuffle = false;
    int m_volumeLevel = 100;

};
