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

#include <QObject>
#include <QString>
#include <QMap>
#include <QDebug>

#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

struct ApplicationDataHeader {
    uint32_t signature = 0x80F6E4F4;
    uint32_t version = 0x31;
};

struct ADVideoMarkerAlias {
    char videoFile[120];
    char markerFile[120];
};

struct VideoMarkerHeader {
    uint32_t signature;
    uint32_t version;
};

struct VideoMarker {
    char type[25];
    double position;
};


class VideoMarkerManager : public QObject {
    Q_OBJECT

public:
    explicit VideoMarkerManager(QObject *parent = nullptr);

    bool loadApplicationFile();
    bool loadVideoMarkerFile(const QString &video);
    bool createVideoMarkerFile(const QString &video);
    void closeVideoMarkerFile();
    bool addVideoMarker(const VideoMarker &marker);
    QList<VideoMarker> getVideoMarkers();
    bool editVideoMarker(const VideoMarker &marker, const VideoMarker &newMarker);
    bool removeVideoMarker(const VideoMarker &marker);

    VideoMarker getByPosition(double position);
    VideoMarker getPreviousByPosition(double position);
    VideoMarker getNextByPosition(double position);

    QList<VideoMarker> getAllByTypeMarkers(const QString &type);

};
