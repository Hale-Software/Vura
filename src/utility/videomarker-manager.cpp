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

#include "videomarker-manager.h"


VideoMarkerManager::VideoMarkerManager(QObject *parent) : QObject(parent) {}

bool VideoMarkerManager::loadApplicationFile()
{
    qDebug() << "Loading application data file...";
}

bool VideoMarkerManager::loadVideoMarkerFile(const QString &video)
{
    qDebug() << "Loading video marker file for video: " << video << "...";

    VideoMarkerHeader header;
    header.signature = 0xE8EDF2EB;
    header.version = 0x31;

    vector<VideoMarker> videoMarkers;

    
}

bool VideoMarkerManager::createVideoMarkerFile(const QString &video)
{
    VideoMarkerHeader header;
    header.signature = 0xE8EDF2EB;
    header.version = 0x31;

    qDebug() << "Creating video marker file for video: " << video << "...";
    ofstream file(video.toStdString().c_str(), ios::binary);
    if (file.bad()) {
        qCritical() << "Unable to create video marker file for video: " << video;
        return false;
    }

    file.write(reinterpret_cast<char *>(&header), sizeof(VideoMarkerHeader));
    file.close();

    qDebug() << "Video marker file created for video: " << video;
    return true;
}

void VideoMarkerManager::closeVideoMarkerFile()
{
    qDebug() << "Closing video marker file";
}

bool VideoMarkerManager::addVideoMarker(const VideoMarker &marker)
{

}

QList<VideoMarker> VideoMarkerManager::getVideoMarkers() {}

bool VideoMarkerManager::editVideoMarker(const VideoMarker &marker, const VideoMarker &newMarker)
{

}

bool VideoMarkerManager::removeVideoMarker(const VideoMarker &marker)
{

}

VideoMarker VideoMarkerManager::getByPosition(double position)
{

}

VideoMarker VideoMarkerManager::getPreviousByPosition(double position)
{

}

VideoMarker VideoMarkerManager::getNextByPosition(double position) {}

QList<VideoMarker> VideoMarkerManager::getAllByTypeMarkers(const QString &type)
{

}
