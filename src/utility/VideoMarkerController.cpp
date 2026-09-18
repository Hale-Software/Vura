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

#include <QUrl>
#include <QSettings>

#include "VideoMarkerController.h"


VideoMarkerController::VideoMarkerController(VideoSlider &slider, QObject *parent)
    : QObject(parent),
      m_slider(&slider)
{
    m_databaseManager = new DatabaseManager(this);
    m_databaseManager->initializeDatabase();
}

QList<VideoMarkerRecord> VideoMarkerController::getVideoMarkers() const { return m_videoMarkers; }

void VideoMarkerController::setCumshotMarkerVisibility(const bool visible)
{
    m_cumshotMarkerVisible = visible;
    m_slider->setMarkerTypeVisible("cumshot", visible);
}

void VideoMarkerController::setCyanMarkerVisibility(const bool visible)
{
    m_cyanMarkerVisible = visible;
    m_slider->setMarkerTypeVisible("cyan", visible);
}

void VideoMarkerController::setDialogMarkerVisibility(const bool visible)
{
    m_dialogMarkerVisible = visible;
    m_slider->setMarkerTypeVisible("dialog", visible);
}

void VideoMarkerController::setMagentaMarkerVisibility(const bool visible)
{
    m_magentaMarkerVisible = visible;
    m_slider->setMarkerTypeVisible("magenta", visible);
}

void VideoMarkerController::setMarkerVisibility(const bool visible)
{
    m_markerVisible = visible;
    m_slider->setMarkerTypeVisible("marker", visible);
}

void VideoMarkerController::setOrangeMarkerVisibility(const bool visible)
{
    m_orangeMarkerVisible = visible;
    m_slider->setMarkerTypeVisible("orange", visible);
}

void VideoMarkerController::setSceneMarkerVisibility(const bool visible)
{
    m_sceneMarkerVisible = visible;
    m_slider->setMarkerTypeVisible("scene", visible);
}

void VideoMarkerController::setStripMarkerVisibility(const bool visible)
{
    m_stripMarkerVisible = visible;
    m_slider->setMarkerTypeVisible("strip", visible);
}

void VideoMarkerController::saveVideoMarkers() const
{
    m_databaseManager->saveVideoMarkers(m_videoMarkers);
    //VideoMarkers::write(m_videoMarkersFile, m_sourceName, m_videoMarkers);
}

void VideoMarkerController::loadVideoMarkers(const QUrl &source)
{
    if (m_videoMarkers.size() > 0) {
        saveVideoMarkers();
    }

    //m_videoMarkers.clear();

    //QString hash = Crypto::fingerprintFile(source.toString());
    //m_videoMarkers = VideoMarkers::read(m_videoMarkersFile, hash);
    //m_sourceName = hash;
    //m_videoMarkers = VideoMarkers::read(m_videoMarkersFile, source.toString());
    //m_videoMarkers = m_databaseManager->getVideoMarkers(source.toString());
    m_sourceName = source.toString();
    refreshVideoMarkers();

    m_slider->loadVideoMarkers(m_videoMarkers);
}

void VideoMarkerController::addCumshotMarker()
{
    const double timestamp = getSliderPercent();
    VideoMarkerRecord marker;
    marker.fileName = m_sourceName;
    marker.markerType = "cumshot";
    marker.timestampMs = timestamp;
    m_databaseManager->saveVideoMarker(marker);
    refreshVideoMarkers();

    m_slider->updateVideoSlider(m_videoMarkers);
}

void VideoMarkerController::addCyanMarker()
{
    const double timestamp = getSliderPercent();
    VideoMarkerRecord marker;
    marker.fileName = m_sourceName;
    marker.markerType = "cyan";
    marker.timestampMs = timestamp;
    m_databaseManager->saveVideoMarker(marker);
    refreshVideoMarkers();

    m_slider->updateVideoSlider(m_videoMarkers);
}

void VideoMarkerController::addDialogMarker()
{
    const double timestamp = getSliderPercent();
    VideoMarkerRecord marker;
    marker.fileName = m_sourceName;
    marker.markerType = "dialog";
    marker.timestampMs = timestamp;
    m_databaseManager->saveVideoMarker(marker);
    refreshVideoMarkers();

    m_slider->updateVideoSlider(m_videoMarkers);
}

void VideoMarkerController::addMagentaMarker()
{
    const double timestamp = getSliderPercent();
    VideoMarkerRecord marker;
    marker.fileName = m_sourceName;
    marker.markerType = "magenta";
    marker.timestampMs = timestamp;
    m_databaseManager->saveVideoMarker(marker);
    refreshVideoMarkers();

    m_slider->updateVideoSlider(m_videoMarkers);
}

void VideoMarkerController::addMarker()
{
    const double timestamp = getSliderPercent();
    VideoMarkerRecord marker;
    marker.fileName = m_sourceName;
    marker.markerType = "marker";
    marker.timestampMs = timestamp;
    m_databaseManager->saveVideoMarker(marker);
    refreshVideoMarkers();

    m_slider->updateVideoSlider(m_videoMarkers);
}

void VideoMarkerController::addOrangeMarker()
{
    const double timestamp = getSliderPercent();
    VideoMarkerRecord marker;
    marker.fileName = m_sourceName;
    marker.markerType = "orange";
    marker.timestampMs = timestamp;
    m_databaseManager->saveVideoMarker(marker);
    refreshVideoMarkers();

    m_slider->updateVideoSlider(m_videoMarkers);
}

void VideoMarkerController::addSceneMarker()
{
    const double timestamp = getSliderPercent();
    VideoMarkerRecord marker;
    marker.fileName = m_sourceName;
    marker.markerType = "scene";
    marker.timestampMs = timestamp;
    m_databaseManager->saveVideoMarker(marker);
    refreshVideoMarkers();

    m_slider->updateVideoSlider(m_videoMarkers);
}

void VideoMarkerController::addStripMarker()
{
    const double timestamp = getSliderPercent();
    VideoMarkerRecord marker;
    marker.fileName = m_sourceName;
    marker.markerType = "strip";
    marker.timestampMs = timestamp;
    m_databaseManager->saveVideoMarker(marker);
    refreshVideoMarkers();

    m_slider->updateVideoSlider(m_videoMarkers);
}

void VideoMarkerController::addVideoMarker(const VideoMarkerRecord &videoMarker)
{
    m_databaseManager->saveVideoMarker(videoMarker);
    refreshVideoMarkers();

    m_slider->updateVideoSlider(m_videoMarkers);
}

VideoMarkerRecord VideoMarkerController::getSelectedMarker()
{
    double sliderPercent = getSliderPercent();
    VideoMarkerRecord selectedMarker = findNearestMarker(sliderPercent);
    if (selectedMarker.id <= 0) return selectedMarker;
    //if (std::isnan(selectedMarker.timestampMs)) return selectedMarker;

    int index = 0;
    int markerIndex = -1;

    QListIterator<VideoMarkerRecord> it(m_videoMarkers);
    while (it.hasNext()) {
        VideoMarkerRecord marker = it.next();
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

void VideoMarkerController::deleteVideoMarker(const VideoMarkerRecord &videoMarker)
{
    m_databaseManager->deleteVideoMarker(videoMarker.id);
    refreshVideoMarkers();
    m_slider->updateVideoSlider(m_videoMarkers);
}

void VideoMarkerController::clearSelectedMarker()
{
    double sliderPercent = getSliderPercent();
    VideoMarkerRecord selectedMarker = findNearestMarker(sliderPercent);
    if (selectedMarker.id <= 0) return;

    m_databaseManager->deleteVideoMarker(selectedMarker.id);
    refreshVideoMarkers();
    m_slider->updateVideoSlider(m_videoMarkers);
    /*

    int index = 0;
    int markerIndex = -1;

    QListIterator<VideoMarkerRecord> it(m_videoMarkers);
    while (it.hasNext()) {
        VideoMarkerRecord marker = it.next();
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
    */
}

void VideoMarkerController::clearMarkers()
{
    //m_videoMarkers.clear();
    m_databaseManager->deleteVideoMarkers(m_sourceName);
    refreshVideoMarkers();
    m_slider->updateVideoSlider(m_videoMarkers);
}

void VideoMarkerController::goToNextMarker()
{
    double sliderPercent = getSliderPercent();
    m_slider->goToNextMarker(sliderPercent);
}

void VideoMarkerController::goToPreviousMarker()
{
    double sliderPercent = getSliderPercent();
    m_slider->goToPreviousMarker(sliderPercent);
}

VideoMarkerRecord VideoMarkerController::findNearestVisibleMarker(const double sliderPercent, const double markerRange) const
{
    VideoMarkerRecord best;
    best.timestampMs = std::numeric_limits<double>::quiet_NaN();

    for (const VideoMarkerRecord &marker : getVideoMarkers()) {
        if (!m_slider->getMarkerTypesVisible(marker.markerType)) continue;
        const double dist = std::abs(marker.timestampMs - sliderPercent);
        if (dist > markerRange) continue;
        if (std::isnan(best.timestampMs) || dist < std::abs(best.timestampMs - sliderPercent))
            best = marker;
    }
    return best;
}

double VideoMarkerController::getSliderPercent() const
{
    const double distanceFromMin = m_slider->value() - m_slider->minimum();
    const double sliderRange = m_slider->maximum() - m_slider->minimum();
    return distanceFromMin / sliderRange;
}

bool VideoMarkerController::checkMarkerProximity() const
{
    const QSettings settings;
    const double sliderPercent = getSliderPercent();
    double markerProximityThreshold = settings.value("markerProximityThreshold", 0.005).toDouble();

    const auto isVisibleMarkerNearSlider = [this, sliderPercent, markerProximityThreshold](const VideoMarkerRecord &marker) {
        if (!m_slider->getMarkerTypesVisible(marker.markerType)) {
            return false;
        }

        const double distanceToSlider = std::abs(marker.timestampMs - sliderPercent);
        return distanceToSlider <= markerProximityThreshold;
    };

    return std::any_of(
        getVideoMarkers().cbegin(),
        getVideoMarkers().cend(),
        isVisibleMarkerNearSlider);
}

bool VideoMarkerController::isPreviousMarkerAvailable(const VideoMarkerRecord &videoMarker) const
{
    VideoMarkerRecord previousMarker;
    previousMarker.timestampMs = std::numeric_limits<double>::quiet_NaN();

    for (const VideoMarkerRecord &marker : getVideoMarkers()) {
        if (marker.timestampMs < videoMarker.timestampMs) {
            if (std::isnan(previousMarker.timestampMs)) {
                previousMarker = marker;

            } else {
                if (marker.timestampMs > previousMarker.timestampMs) {
                    previousMarker = marker;
                }
            }
        }
    }

    if (std::isnan(previousMarker.timestampMs)) {
        return false;
    }
    return true;
}

bool VideoMarkerController::isNextMarkerAvailable(const VideoMarkerRecord &videoMarker) const
{
    VideoMarkerRecord nextMarker;
    nextMarker.timestampMs = std::numeric_limits<double>::quiet_NaN();

    for (const VideoMarkerRecord &marker : getVideoMarkers()) {
        if (marker.timestampMs > videoMarker.timestampMs) {
            if (std::isnan(nextMarker.timestampMs)) {
                nextMarker = marker;

            } else {
                if (marker.timestampMs < nextMarker.timestampMs) {
                    nextMarker = marker;
                }
            }
        }
    }

    if (std::isnan(nextMarker.timestampMs)) {
        return false;
    }
    return true;
}

VideoMarkerRecord VideoMarkerController::findNearestMarker(const double sliderPercent)
{
    const double markerRange = 0.005;
    VideoMarkerRecord best;
    //best.timestampMs = std::numeric_limits<double>::quiet_NaN();

    for (const VideoMarkerRecord &marker : m_videoMarkers) {
        if (marker.markerType == "cumshot" && !m_cumshotMarkerVisible) continue;
        if (marker.markerType == "cyan" && !m_cyanMarkerVisible) continue;
        if (marker.markerType == "dialog" && !m_dialogMarkerVisible) continue;
        if (marker.markerType == "magenta" && !m_magentaMarkerVisible) continue;
        if (marker.markerType == "marker" && !m_markerVisible) continue;
        if (marker.markerType == "orange" && !m_orangeMarkerVisible) continue;
        if (marker.markerType == "scene" && !m_sceneMarkerVisible) continue;
        if (marker.markerType == "strip" && !m_stripMarkerVisible) continue;

        const double dist = std::abs(marker.timestampMs - sliderPercent);
        if (dist > markerRange) continue;
        //if (std::isnan(best.timestampMs) || dist < std::abs(best.timestampMs - sliderPercent))
        //    best = marker;
        if (best.id <= 0 || dist < std::abs(best.timestampMs - sliderPercent))
            best = marker;
    }
    return best;
}

void VideoMarkerController::refreshVideoMarkers()
{
    m_videoMarkers.clear();
    m_videoMarkers = m_databaseManager->getVideoMarkers(m_sourceName);
}
