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
#include <QMediaPlayer>
#include <QPointer>
#include <QDebug>

#include <libvura/data/video-markers.h>

#include "MarkerEditDialog.h"
#include "VideoSliderWidget.h"
#include "VideoSlider.h"


class MarkerController : public QObject
{
    Q_OBJECT

    friend class MarkerEditDialog;

public:
    explicit MarkerController(QObject* parent = nullptr);
    ~MarkerController() override;

signals:
    //void markerEdited(const VuraVideoMarker &videoMarker);

public slots:
    void loadVideoMarkers(const QList<VuraVideoMarker> &videoMarkers);
    void addMarker();

private:
    QList<VuraVideoMarker> m_videoMarkers;
    VideoSlider *m_videoSlider = nullptr;
    VideoSliderWidget *m_videoSliderWidget = nullptr;

    QPointer<MarkerEditDialog> m_markerEditDialog;

};
