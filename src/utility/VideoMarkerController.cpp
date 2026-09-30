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

#include "VideoMarkerController.h"
#include "VideoSlider.h"

#include <QSettings>
#include <QUrl>

#include <cmath>


namespace {

constexpr double kDefaultMarkerRange = 0.005;

/// How close (as a fraction of the slider) the playhead must be to a marker
/// for that marker to count as "selected".
double markerRange()
{
    return QSettings().value("markerProximityThreshold", kDefaultMarkerRange).toDouble();
}

}


VideoMarkerController::VideoMarkerController(VideoSlider &slider, QObject *parent)
    : QObject(parent),
      m_slider(&slider)
{
    m_databaseManager = new DatabaseManager(this);
    m_databaseManager->initializeDatabase();
}

QList<VideoMarkerRecord> VideoMarkerController::getVideoMarkers() const { return m_videoMarkers; }

// Visibility is owned by the slider, which paints the markers. Keeping a
// second copy here let the two disagree, so the controller just forwards.
void VideoMarkerController::setTypeVisible(const QString &type, const bool visible)
{
    m_slider->setMarkerTypeVisible(type, visible);
}

void VideoMarkerController::addMarkerOfType(const QString &type)
{
    VideoMarkerRecord marker;
    marker.fileName = m_sourceName;
    marker.markerType = type;
    marker.timestampMs = getSliderPercent();
    addVideoMarker(marker);
}

void VideoMarkerController::addVideoMarker(const VideoMarkerRecord &videoMarker)
{
    m_databaseManager->saveVideoMarker(videoMarker);
    reloadAndRepaint();
}

void VideoMarkerController::deleteVideoMarker(const VideoMarkerRecord &videoMarker)
{
    m_databaseManager->deleteVideoMarker(videoMarker.id);
    reloadAndRepaint();
}

void VideoMarkerController::clearSelectedMarker()
{
    const VideoMarkerRecord selected = getSelectedMarker();
    if (selected.id > 0)
        deleteVideoMarker(selected);
}

void VideoMarkerController::clearMarkers()
{
    m_databaseManager->deleteVideoMarkers(m_sourceName);
    reloadAndRepaint();
}

VideoMarkerRecord VideoMarkerController::getSelectedMarker() const
{
    return findNearestVisibleMarker(getSliderPercent());
}

bool VideoMarkerController::checkMarkerProximity() const
{
    return getSelectedMarker().id > 0;
}

void VideoMarkerController::saveVideoMarkers() const
{
    m_databaseManager->saveVideoMarkers(m_videoMarkers);
}

void VideoMarkerController::loadVideoMarkers(const QUrl &source)
{
    if (!m_videoMarkers.isEmpty())
        saveVideoMarkers();

    m_sourceName = source.toString();
    refreshVideoMarkers();
    m_slider->loadVideoMarkers(m_videoMarkers);
}

void VideoMarkerController::goToNextMarker()
{
    m_slider->goToNextMarker(getSliderPercent());
}

void VideoMarkerController::goToPreviousMarker()
{
    m_slider->goToPreviousMarker(getSliderPercent());
}

double VideoMarkerController::getSliderPercent() const
{
    const double distanceFromMin = m_slider->value() - m_slider->minimum();
    const double sliderRange = m_slider->maximum() - m_slider->minimum();
    return distanceFromMin / sliderRange;
}

VideoMarkerRecord VideoMarkerController::findNearestVisibleMarker(const double sliderPercent) const
{
    const VideoMarkerRecord *best = nullptr;
    double bestDistance = markerRange();

    for (const VideoMarkerRecord &marker : m_videoMarkers) {
        if (!m_slider->getMarkerTypesVisible(marker.markerType))
            continue;

        const double distance = std::abs(marker.timestampMs - sliderPercent);
        // First match within range wins ties, same as before.
        if (distance < bestDistance || (!best && distance <= bestDistance)) {
            best = &marker;
            bestDistance = distance;
        }
    }

    return best ? *best : VideoMarkerRecord{};
}

void VideoMarkerController::refreshVideoMarkers()
{
    m_videoMarkers = m_databaseManager->getVideoMarkers(m_sourceName);
}

void VideoMarkerController::reloadAndRepaint()
{
    refreshVideoMarkers();
    m_slider->updateVideoSlider(m_videoMarkers);
}
