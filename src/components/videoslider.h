/*******************************************************************************
     Copyright (c) 2026. by Andrew Hale <halea2196@gmail.com>

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
#include <QString>
#include <QList>
#include <QDebug>

#include <data/video-markers.h>


class VideoSlider : public QWidget
{
    Q_OBJECT

public:
    explicit VideoSlider(QList<VuraVideoMarker> *videoMarkers, QWidget *parent = nullptr);

    QSize minimumSizeHint() const override;

    int GetMinimun() const;
    void SetMinimum(int minimum);

    int GetMaximun() const;
    void SetMaximum(int maximum);

    int GetValue() const;
    void SetValue(int value);

    void SetRange(int minimum, int maximum);

    double GetSliderPercent() const;
    void setSliderPercent(double percent);

    bool GetVideoLoaded() const;
    void SetVideoLoaded(bool value);

    bool GetIsSliderDown() const;
    void SetIsSliderDown(bool value);

    bool GetShowingMarkers() const;
    void SetShowingMarkers(bool value);

    bool GetShowingCumshotMarkers() const;
    void SetShowingCumshotMarkers(bool value);

    bool GetShowingCyanMarkers() const;
    void SetShowingCyanMarkers(bool value);

    bool GetShowingDialogMarkers() const;
    void SetShowingDialogMarkers(bool value);

    bool GetShowingMagentaMarkers() const;
    void SetShowingMagentaMarkers(bool value);

    bool GetShowingOrangeMarkers() const;
    void SetShowingOrangeMarkers(bool value);

    bool GetShowingSceneMarkers() const;
    void SetShowingSceneMarkers(bool value);

    bool GetShowingStripMarkers() const;
    void SetShowingStripMarkers(bool value);

signals:
    void rangeChanged(int minimum, int maximum);
    void valueChanged(int value);
    void sliderPressed();
    void sliderMoved(int value);
    void sliderReleased();
    void sliderClicked(int mseconds);

public slots:
    void updateVideoSlider();
    void setValue(int value);
    void setMinimum(int minimum);
    void setMaximum(int maximum);
    void goToNextMarker(double currentPercent);
    void goToPreviousMarker(double currentPercent);

//private slots:
//    void hideIndicator();

protected:
    void paintEvent(QPaintEvent *event) override;
    //void enterEvent(QEnterEvent *event) override;
    //void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    QRectF carrotHandleRect() const;
    QRectF handleRect(int value) const;

private:
    Q_DISABLE_COPY(VideoSlider)
    int validLength() const;
    int valueFromPos(int x) const;

    QTimer *timer;
    QList<VuraVideoMarker> *m_videoMarkers;
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
    bool m_showingIndicator;
    double m_sliderPercent;
    bool m_sliderClicked;
    bool m_videoLoaded;
    QColor m_backgroundColor;
    QColor m_emptySliderColor;
    QColor m_fullSliderColor;
    QColor m_caretColor;
    QColor m_markerColor;
    QColor m_sceneMarkerColor;
    QColor m_cumshotMarkerColor;
    QColor m_stripMarkerColor;
    QColor m_dialogMarkerColor;
    bool m_firstHandlePressed;
    bool m_secondHandlePressed;
    int m_delta;
    int m_interval;

};
