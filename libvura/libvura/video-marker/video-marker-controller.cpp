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

QList<VuraVideoMarker> VideoMarkerController::getVideoMarkers() const { return m_videoMarkers; }

void VideoMarkerController::setCumshotMarkerVisibility(bool visible) { m_cumshotMarkerVisible = visible; }

void VideoMarkerController::setCyanMarkerVisibility(bool visible) { m_cyanMarkerVisible = visible; }

void VideoMarkerController::setDialogMarkerVisibility(bool visible) { m_dialogMarkerVisible = visible; }

void VideoMarkerController::setMagentaMarkerVisibility(bool visible) { m_magentaMarkerVisible = visible; }

void VideoMarkerController::setMarkerVisibility(bool visible) { m_markerVisible = visible; }

void VideoMarkerController::setOrangeMarkerVisibility(bool visible) { m_orangeMarkerVisible = visible; }

void VideoMarkerController::setSceneMarkerVisibility(bool visible) { m_sceneMarkerVisible = visible; }

void VideoMarkerController::setStripMarkerVisibility(bool visible) { m_stripMarkerVisible = visible; }

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

VuraVideoMarker VideoMarkerController::getSelectedMarker(double sliderPercent)
{
    VuraVideoMarker selectedMarker = findNearestMarker(sliderPercent);
    if (selectedMarker.id.isEmpty()) return selectedMarker;
    if (std::isnan(selectedMarker.timestamp)) return selectedMarker;

    int index = 0;
    int markerIndex = -1;

    QListIterator<VuraVideoMarker> it(m_videoMarkers);
    while (it.hasNext()) {
        VuraVideoMarker marker = it.next();
        if (marker.id == selectedMarker.id) {
            markerIndex = index;
            break;
        }
        index++;
    }

    if (markerIndex != -1) {
        return m_videoMarkers[markerIndex];
    }
    return selectedMarker;
}

void VideoMarkerController::clearSelectedMarker(double sliderPercent)
{
    VuraVideoMarker selectedMarker = findNearestMarker(sliderPercent);
    if (selectedMarker.id.isEmpty()) return;
    if (std::isnan(selectedMarker.timestamp)) return;

    int index = 0;
    int markerIndex = -1;

    QListIterator<VuraVideoMarker> it(m_videoMarkers);
    while (it.hasNext()) {
        VuraVideoMarker marker = it.next();
        if (marker.id == selectedMarker.id) {
            markerIndex = index;
            break;
        }
        index++;
    }

    if (markerIndex != -1) {
        m_videoMarkers.removeAt(markerIndex);
    }
    emit markersUpdated();
}

void VideoMarkerController::clearMarkers()
{
    m_videoMarkers.clear();
    emit markersUpdated();
}

VuraVideoMarker VideoMarkerController::findNearestMarker(const double sliderPercent)
{
    const double markerRange = 0.005;
    VuraVideoMarker best;
    best.timestamp = std::numeric_limits<double>::quiet_NaN();

    for (const VuraVideoMarker &marker : m_videoMarkers) {
        if (marker.markerType == "cumshot" && !m_cumshotMarkerVisible) continue;
        if (marker.markerType == "cyan" && !m_cyanMarkerVisible) continue;
        if (marker.markerType == "dialog" && !m_dialogMarkerVisible) continue;
        if (marker.markerType == "magenta" && !m_magentaMarkerVisible) continue;
        if (marker.markerType == "marker" && !m_markerVisible) continue;
        if (marker.markerType == "orange" && !m_orangeMarkerVisible) continue;
        if (marker.markerType == "scene" && !m_sceneMarkerVisible) continue;
        if (marker.markerType == "strip" && !m_stripMarkerVisible) continue;

        const double dist = std::abs(marker.timestamp - sliderPercent);
        if (dist > markerRange) continue;
        if (std::isnan(best.timestamp) || dist < std::abs(best.timestamp - sliderPercent))
            best = marker;
    }
    return best;
}
