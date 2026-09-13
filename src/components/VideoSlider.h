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

#include <QWidget>
#include <QList>
#include <QMouseEvent>
#include <QSize>

#include <libvura/models/video-marker-record.h>
#include <libvura/video-marker/video-marker-controller.h>


class VideoSlider : public QWidget
{
    Q_OBJECT

public:
    explicit VideoSlider(VideoMarkerController *videoMarkerController, QWidget *parent = nullptr);

    QSize minimumSizeHint() const override;

    int minimum() const { return m_minimum; }
    int maximum() const { return m_maximum; }
    int value() const { return m_value; }

    bool isScrubbing() const { return m_scrubbing; }
    void setPositionFromEngine(qint64 ms);

    bool getMarkerTypesVisible(const QString& markerType) const;
    void setMarkerTypeVisible(const QString& markerType, bool visible);

signals:
    void scrubbed(qint64 ms);
    void scrubFinished(qint64 ms);
    void requestThumbnail(int64_t hoverTimestamp);

public slots:
    void loadVideoMarkers();
    void updateVideoSlider();
    void setValue(int val);
    void setMinimum(int minimum);
    void setMaximum(int maximum);
    void setRange(int minimum, int maximum);
    void goToNextMarker(double currentPercent);
    void goToPreviousMarker(double currentPercent);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    QRectF carrotHandleRect() const;
    QRectF handleRect(int value) const;

private:
    int valueForPosition(int x) const;
    int validLength() const;
    int valueFromPos(int x) const;

    VideoMarkerController *m_videoMarkerController;
    QList<VideoMarkerRecord> m_videoMarkers;
    float m_sliderPercent = std::clamp(0.0f, 0.0f, 1.0f);
    int m_minimum;
    int m_maximum;
    int m_value;
    bool m_showingMarkers;
    bool m_showingCumshotMarkers;
    bool m_showingCyanMarkers;
    bool m_showingDialogMarkers;
    bool m_showingMagentaMarkers;
    bool m_showingOrangeMarkers;
    bool m_showingSceneMarkers;
    bool m_showingStripMarkers;
    int m_delta;
    int m_interval;
    double m_sliderBarHeightValue;
    bool m_scrubbing = false;

};
