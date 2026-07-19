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

#include "VideoMarker.h"


bool VideoMarkerHandler::saveMarkersToFile(const QString &fileName, const QList<VideoMarker> &markers)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Could not open file:" << fileName;
        return false;
    }

    QDataStream stream(&file);
    stream << (quint32)0x564D524B;
    stream << (qint32)1;
    stream << markers;
    file.close();
    return true;
}

QList<VideoMarker> VideoMarkerHandler::loadAllMarkersFromFile(const QString &fileName)
{
    QList<VideoMarker> markers;
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open file:" << fileName;
        return markers;
    }

    QDataStream stream(&file);
    quint32 signature;
    stream >> signature;
    if (signature != 0x564D524B) {
        qWarning() << "Invalid signature:" << signature;
        return markers;
    }

    qint32 version;
    stream >> version;
    if (version == 1) {
        stream >> markers;
    } else {
        qWarning() << "Unsupported version:" << version;
    }
    file.close();
    return markers;
}

QList<VideoMarker> VideoMarkerHandler::loadMarkersFromFile(const QString &fileName, const QString &videoFileName)
{
    QList<VideoMarker> markers;
    QList<VideoMarker> videoMarkers;
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open file:" << fileName;
        return markers;
    }

    QDataStream stream(&file);
    quint32 signature;
    stream >> signature;
    if (signature != 0x564D524B) {
        qWarning() << "Invalid signature:" << signature;
        return markers;
    }

    qint32 version;
    stream >> version;
    if (version == 1) {
        stream >> markers;
    } else {
        qWarning() << "Unsupported version:" << version;
    }
    file.close();

    for (VideoMarker &marker : markers) {
        if (marker.fileName == videoFileName)
            videoMarkers.append(marker);
    }

    return videoMarkers;
}
