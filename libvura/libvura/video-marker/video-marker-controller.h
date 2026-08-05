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

#include <QObject>
#include <QStandardPaths>
#include <QDebug>

#include <libvura/video-marker/video-markers.h>


class VideoMarkerController : public QObject
{
    Q_OBJECT

public:
    explicit VideoMarkerController(QObject* parent = nullptr);

    QList<VuraVideoMarker> getVideoMarkers() const;

signals:
    void markersLoaded();
    void markerAdded();
    void markerEdited(const VuraVideoMarker &videoMarker);

public slots:
    void saveVideoMarkers();
    void loadVideoMarkers(const QUrl &source);
    void addCumshotMarker(double timestamp);
    void addCyanMarker(double timestamp);
    void addDialogMarker(double timestamp);
    void addMagentaMarker(double timestamp);
    void addMarker(double timestamp);
    void addOrangeMarker(double timestamp);
    void addSceneMarker(double timestamp);
    void addStripMarker(double timestamp);

private:
    QList<VuraVideoMarker> m_videoMarkers;
    QString m_videoMarkersFile;
    QString m_sourceName;

};
