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

#include <QString>
#include <QDataStream>
#include <QList>
#include <QFile>
#include <QDebug>

struct VideoMarker
{
    qint64 id;
    QString fileName;
    QString markerName;
    QString markerType;
    QString comments;
    qint64 timestampMs;

    friend QDataStream& operator<<(QDataStream& out, const VideoMarker& marker)
    {
        out << marker.id << marker.fileName << marker.markerName << marker.markerType << marker.comments << marker.timestampMs;
        return out;
    }

    friend QDataStream& operator>>(QDataStream& in, VideoMarker& marker)
    {
        in >> marker.id >> marker.fileName >> marker.markerName >> marker.markerType >> marker.comments >> marker.timestampMs;
        return in;
    }
};

class VideoMarkers
{
public:
    static bool saveMarkersToFile(const QString &fileName, const QList<VideoMarker> &markers);
    static QList<VideoMarker> readMarkersFromFile(const QString &fileName);

};
