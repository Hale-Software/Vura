/*******************************************************************************
     Copyright (c) 2026.  by Andrew Hale <halea2196@gmail.com>

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
#include <QPainter>
#include <QPaintEvent>
#include <QColor>
#include <QPen>
#include <QBrush>
#include <QEvent>
#include <QTimer>
#include <QMouseEvent>
#include <QDebug>

#include <vura-datafile-handler.h>


class VideoSlider : public QWidget
{
    Q_OBJECT

public:
    explicit VideoSlider(QWidget *parent = nullptr);

    void setMinimum(int mminimum);
    void setMaximum(int mmaximum);
    void setValue(int mvalue);
    void setVideoLoaded(bool isLoaded);
    void setMarkers(QList<MarkersData> markersDataList);
    void jumpToNextMarker(double currentPercent);
    void jumpToPreviousMarker(double currentPercent);

    int minimum() const;
    int maximum() const;
    int value() const;
    bool isSliderDown() const;
    bool isVideoLoaded() const;
    bool showMarkers = true;
    bool showCumshotMarkers = true;
    bool showCyanMarkers = true;
    bool showDialogMarkers = true;
    bool showMagentaMarkers = true;
    bool showOrangeMarkers = true;
    bool showSceneMarkers = true;
    bool showStripMarkers = true;

signals:
    void valueChanged(int value);
    void sliderPressed();
    void sliderMoved(int value);
    void sliderReleased();
    void sliderClicked(int mseconds);
    void markerSelected(int mseconds);

private slots:
    void hideIndicator();

protected:
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QTimer *timer;
    QList<MarkersData> m_markersDataList;
    bool m_showingIndicator = false;
    int m_minimum = 0;
    int m_maximum = 100;
    int m_value = 0;
    bool m_isSliderDown = false;
    bool m_videoLoaded = false;

    QColor m_emptySliderColor = QColor(110, 110, 110);
    QColor m_fullSliderColor = QColor(0, 217, 255);
    QColor m_caretColor = QColor(180, 180, 180);
    QColor m_markerColor = QColor();
    QColor m_sceneMarkerColor = QColor();
    QColor m_cumshotMarkerColor = QColor();
    QColor m_stripMarkerColor = QColor();
    QColor m_dialogMarkerColor = QColor();

};
