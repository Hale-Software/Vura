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


class VideoSlider : public QWidget
{
    Q_OBJECT

public:
    explicit VideoSlider(QWidget *parent = nullptr);

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

    /// Emitted when a marker is left-clicked. The seek itself is already
    /// requested via scrubFinished(); this is for anything extra the window
    /// wants to do (status bar text, selection highlight, ...).
    void markerActivated(const VideoMarkerRecord &marker);
    void markerEditRequested(const VideoMarkerRecord &marker);
    void markerDeleteRequested(const VideoMarkerRecord &marker);
    void markerTypeHidden(const QString &markerType);

public slots:
    void loadVideoMarkers(QList<VideoMarkerRecord> markers);
    void updateVideoSlider(QList<VideoMarkerRecord> markers);
    void setValue(int val);
    void setMinimum(int minimum);
    void setMaximum(int maximum);
    void setRange(int minimum, int maximum);
    void goToNextMarker(double currentPercent);
    void goToPreviousMarker(double currentPercent);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;
    void leaveEvent(QEvent *event) override;
    bool event(QEvent *event) override;

private:
    // Track geometry. Everything that maps between pixels and time goes
    // through these so the playhead, the markers and the hit testing can
    // never drift apart again.
    int trackPadding() const;
    int trackWidth() const;
    int xForPercent(double percent) const;
    double percentForX(int x) const;

    int valueForPosition(int x) const;
    qint64 msForMarker(const VideoMarkerRecord &marker) const;

    /// Index into m_videoMarkers of the visible marker under pos, or -1.
    int markerIndexAt(const QPoint &pos) const;
    QString markerToolTip(const VideoMarkerRecord &marker) const;
    void seekToMarker(const VideoMarkerRecord &marker);

    QList<VideoMarkerRecord> m_videoMarkers;
    float m_sliderPercent = 0.0f;
    int m_minimum;
    int m_maximum;
    int m_value;
    int m_hoveredMarker = -1;
    bool m_showingMarkers;
    bool m_showingCumshotMarkers;
    bool m_showingCyanMarkers;
    bool m_showingDialogMarkers;
    bool m_showingMagentaMarkers;
    bool m_showingOrangeMarkers;
    bool m_showingSceneMarkers;
    bool m_showingStripMarkers;
    bool m_scrubbing = false;

};