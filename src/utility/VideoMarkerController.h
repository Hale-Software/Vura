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

#include <QList>
#include <QObject>
#include <QString>

#include <libvura/io/database-manager.h>
#include <libvura/models/video-marker-record.h>

class QUrl;
class VideoSlider;


class VideoMarkerController : public QObject
{
    Q_OBJECT
public:
    explicit VideoMarkerController(VideoSlider &slider, QObject *parent = nullptr);

    QList<VideoMarkerRecord> getVideoMarkers() const;

    /// The nearest visible marker within the proximity threshold of the
    /// playhead, or a default record (id <= 0) if there isn't one.
    VideoMarkerRecord getSelectedMarker() const;
    bool checkMarkerProximity() const;

signals:
    void markerEdited(const VideoMarkerRecord &videoMarker);
    void loadMarkers(QList<VideoMarkerRecord> markers);
    void updateVideoSlider(QList<VideoMarkerRecord> markers);

public slots:
    void setTypeVisible(const QString &type, bool visible);
    void addMarkerOfType(const QString &type);

    void addVideoMarker(const VideoMarkerRecord &videoMarker);
    void deleteVideoMarker(const VideoMarkerRecord &videoMarker);
    void clearSelectedMarker();
    void clearMarkers();

    void saveVideoMarkers() const;
    void loadVideoMarkers(const QUrl &source);

    void goToNextMarker();
    void goToPreviousMarker();

private:
    double getSliderPercent() const;
    VideoMarkerRecord findNearestVisibleMarker(double sliderPercent) const;
    void refreshVideoMarkers();
    void reloadAndRepaint();

    VideoSlider *m_slider = nullptr;
    DatabaseManager *m_databaseManager = nullptr;

    QList<VideoMarkerRecord> m_videoMarkers;
    QString m_sourceName;
    
};
