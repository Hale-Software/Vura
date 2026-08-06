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
#include <QSettings>
#include <QPainter>
#include <QPaintEvent>
#include <QColor>
#include <QPen>
#include <QBrush>
#include <QEvent>
#include <QTimer>
#include <QMouseEvent>
#include <QString>
#include <QList>
#include <QLabel>
#include <QDebug>

#include <libvura/video-marker/video-markers.h>
#include <libvura/video-marker/video-marker-controller.h>


class VideoSlider : public QWidget
{
    Q_OBJECT

public:
    explicit VideoSlider(VideoMarkerController *videoMarkerController, QWidget *parent = nullptr);

    QSize minimumSizeHint() const override;

    int GetMinimun() const;
    void SetMinimum(int minimum);

    int GetMaximun() const;
    void SetMaximum(int maximum);

    int GetValue() const;
    void SetValue(int value);

    void SetRange(int minimum, int maximum);

    bool GetSliderPressed() const;
    void SetSliderPressed(bool value);

    bool getMarkerTypesVisible(const QString& markerType) const;
    void setMarkerTypeVisible(const QString& markerType, bool visible);

signals:
    void valueChanged(int value);
    void sliderPressed(bool pressed);
    void requestThumbnail(int64_t hoverTimestamp);

public slots:
    void loadVideoMarkers();
    void updateVideoSlider();
    void setValue(int value);
    void setMinimum(int minimum);
    void setMaximum(int maximum);
    void goToNextMarker(double currentPercent);
    void goToPreviousMarker(double currentPercent);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    QRectF carrotHandleRect() const;
    QRectF handleRect(int value) const;

private:
    VideoMarkerController *m_videoMarkerController;
    int validLength() const;
    int valueFromPos(int x) const;

    QList<VuraVideoMarker> m_videoMarkers;
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
    bool m_sliderPressed;
    int m_delta;
    int m_interval;
    double m_sliderBarHeightValue;

};
