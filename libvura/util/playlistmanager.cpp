/*******************************************************************************
     Copyright (c) 2026. by Andrew Hale <halea2196@gmail.com>

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

#include "playlistmanager.h"


PlaylistManager::PlaylistManager(QMediaPlayer *p, VuraPlaylistModel *m) : player(p), model(m)
{
    connect(player, &QMediaPlayer::mediaStatusChanged, this, [this](const QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::EndOfMedia) playNext();
    });
}

void PlaylistManager::scanMediaDuration(const int row)
{
    durationScanner = new QMediaPlayer(this);
    connect(durationScanner, &QMediaPlayer::durationChanged, this, [=](const qint64 ms) {
        QString format = "mm:ss";
        if (ms > 3600)
            format = "hh:mm:ss";

        const QTime currentTime(
            (ms / 3600) % 60,
            (ms / 60) % 60,
            ms % 60,
            (ms * 1000) % 1000);

        const QString durationStr = currentTime.toString(format);
        model->updateDuration(row, durationStr);
        durationScanner->deleteLater();
    });
    
    durationScanner->setSource(model->m_media[row].url);
}

void PlaylistManager::playTrack(const int index)
{
    bool wasPlaying = player->isPlaying();

    if (index >= 0 && index < model->m_media.size()) {
        currentIndex = index;
        player->setSource(model->m_media[index].url);

        if (wasPlaying)
            player->play();

    } else {
        currentIndex = -1;
        player->stop();
    }
}

void PlaylistManager::playNext()
{
    playTrack(currentIndex + 1);
}

void PlaylistManager::playPrevious()
{
    playTrack(currentIndex - 1);
}
