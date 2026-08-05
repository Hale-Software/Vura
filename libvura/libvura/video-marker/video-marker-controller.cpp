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

#include "video-marker-controller.h"
#include <libvura/config.h>


VideoMarkerController::VideoMarkerController(QObject *parent) : QObject(parent)
{
    m_videoMarkersFile = (QString(VURA_BUILD_TYPE) == "Debug") ? "debug/global.json" :
                          QString("%1/global.json").arg(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
}

QList<VuraVideoMarker> VideoMarkerController::getVideoMarkers() const
{
    return m_videoMarkers;
}

void VideoMarkerController::saveVideoMarkers()
{
    VideoMarkers::write(m_videoMarkersFile, m_sourceName, m_videoMarkers);
}

void VideoMarkerController::loadVideoMarkers(const QUrl &source)
{
    if (m_videoMarkers.size() > 0) {
        saveVideoMarkers();
    }

    m_videoMarkers.clear();

    m_videoMarkers = VideoMarkers::read(m_videoMarkersFile, source.toString());
    m_sourceName = source.toString();

    emit markersLoaded();
}

void VideoMarkerController::addCumshotMarker(const double timestamp)
{
    VuraVideoMarker marker;
    marker.id = VideoMarkers::generateMarkerID(m_videoMarkersFile);
    marker.fileName = m_sourceName;
    marker.markerType = "cumshot";
    marker.timestamp = timestamp;
    m_videoMarkers.append(marker);

    qDebug() << "Added new marker: [Marker ID] " << marker.id << " [Marker Filename] " << marker.fileName << " [Marker Type] " << marker.markerType << " [Marker Timestamp] " << marker.timestamp;
    emit markerAdded();
}

void VideoMarkerController::addCyanMarker(const double timestamp)
{
    VuraVideoMarker marker;
    marker.id = VideoMarkers::generateMarkerID(m_videoMarkersFile);
    marker.fileName = m_sourceName;
    marker.markerType = "cyan";
    marker.timestamp = timestamp;
    m_videoMarkers.append(marker);

    qDebug() << "Added new marker: [Marker ID] " << marker.id << " [Marker Filename] " << marker.fileName << " [Marker Type] " << marker.markerType << " [Marker Timestamp] " << marker.timestamp;
    emit markerAdded();
}

void VideoMarkerController::addDialogMarker(const double timestamp)
{
    VuraVideoMarker marker;
    marker.id = VideoMarkers::generateMarkerID(m_videoMarkersFile);
    marker.fileName = m_sourceName;
    marker.markerType = "dialog";
    marker.timestamp = timestamp;
    m_videoMarkers.append(marker);

    qDebug() << "Added new marker: [Marker ID] " << marker.id << " [Marker Filename] " << marker.fileName << " [Marker Type] " << marker.markerType << " [Marker Timestamp] " << marker.timestamp;
    emit markerAdded();
}

void VideoMarkerController::addMagentaMarker(const double timestamp)
{
    VuraVideoMarker marker;
    marker.id = VideoMarkers::generateMarkerID(m_videoMarkersFile);
    marker.fileName = m_sourceName;
    marker.markerType = "magenta";
    marker.timestamp = timestamp;
    m_videoMarkers.append(marker);

    qDebug() << "Added new marker: [Marker ID] " << marker.id << " [Marker Filename] " << marker.fileName << " [Marker Type] " << marker.markerType << " [Marker Timestamp] " << marker.timestamp;
    emit markerAdded();
}

void VideoMarkerController::addMarker(const double timestamp)
{
    VuraVideoMarker marker;
    marker.id = VideoMarkers::generateMarkerID(m_videoMarkersFile);
    marker.fileName = m_sourceName;
    marker.markerType = "marker";
    marker.timestamp = timestamp;
    m_videoMarkers.append(marker);

    qDebug() << "Added new marker: [Marker ID] " << marker.id << " [Marker Filename] " << marker.fileName << " [Marker Type] " << marker.markerType << " [Marker Timestamp] " << marker.timestamp;
    emit markerAdded();
}

void VideoMarkerController::addOrangeMarker(const double timestamp)
{
    VuraVideoMarker marker;
    marker.id = VideoMarkers::generateMarkerID(m_videoMarkersFile);
    marker.fileName = m_sourceName;
    marker.markerType = "orange";
    marker.timestamp = timestamp;
    m_videoMarkers.append(marker);

    qDebug() << "Added new marker: [Marker ID] " << marker.id << " [Marker Filename] " << marker.fileName << " [Marker Type] " << marker.markerType << " [Marker Timestamp] " << marker.timestamp;
    emit markerAdded();
}

void VideoMarkerController::addSceneMarker(const double timestamp)
{
    VuraVideoMarker marker;
    marker.id = VideoMarkers::generateMarkerID(m_videoMarkersFile);
    marker.fileName = m_sourceName;
    marker.markerType = "scene";
    marker.timestamp = timestamp;
    m_videoMarkers.append(marker);

    qDebug() << "Added new marker: [Marker ID] " << marker.id << " [Marker Filename] " << marker.fileName << " [Marker Type] " << marker.markerType << " [Marker Timestamp] " << marker.timestamp;
    emit markerAdded();
}

void VideoMarkerController::addStripMarker(const double timestamp)
{
    VuraVideoMarker marker;
    marker.id = VideoMarkers::generateMarkerID(m_videoMarkersFile);
    marker.fileName = m_sourceName;
    marker.markerType = "strip";
    marker.timestamp = timestamp;
    m_videoMarkers.append(marker);

    qDebug() << "Added new marker: [Marker ID] " << marker.id << " [Marker Filename] " << marker.fileName << " [Marker Type] " << marker.markerType << " [Marker Timestamp] " << marker.timestamp;
    emit markerAdded();
}
