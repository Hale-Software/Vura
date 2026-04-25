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

#include "videoslider.h"

namespace {
const int m_sliderBarHeight = 5;
const int m_markerHeight = 10;
const int m_markerWidth = 2;
const int m_indicatorSideLength = 11;
const int m_leftRightMargin = 1;
}


VideoSlider::VideoSlider(QList<VuraVideoMarker> *videoMarkers, QWidget *parent)
    : QWidget(parent),
    m_videoMarkers(videoMarkers),
    m_minimum(0),
    m_maximum(100),
    m_value(0),
    m_showingMarkers(true),
    m_showingCumshotMarkers(true),
    m_showingCyanMarkers(true),
    m_showingDialogMarkers(true),
    m_showingMagentaMarkers(true),
    m_showingOrangeMarkers(true),
    m_showingSceneMarkers(true),
    m_showingStripMarkers(true),
    m_showingIndicator(false),
    m_sliderPercent(0.0),
    m_sliderClicked(false),
    m_videoLoaded(false),
    m_backgroundColor(QColor(0x1E, 0x90, 0xFF)),
    m_emptySliderColor(QColor(110, 110, 110)),
    m_fullSliderColor(QColor(0, 217, 255)),
    m_caretColor(QColor(180, 180, 180)),
    m_markerColor(QColor()),
    m_sceneMarkerColor(QColor()),
    m_cumshotMarkerColor(QColor()),
    m_stripMarkerColor(QColor()),
    m_dialogMarkerColor(QColor())
{
    //setAttribute(Qt::WA_Hover, true);
    setMouseTracking(true);
    this->setFixedHeight(30);

    //timer = new QTimer(this);
    //connect(timer, &QTimer::timeout, this, &VideoSlider::hideIndicator);
}

void VideoSlider::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Draw Groove (Background)
    painter.setBrush(Qt::lightGray);
    painter.drawRoundedRect(rect().adjusted(0, 10, 0, -10), 5, 5);

    // Calculate Handle Position
    double ratio = static_cast<double>(m_value) / m_maximum;
    int handleX = ratio * (width() - 20); // 20 is handle width

    // Draw Handle
    painter.setBrush(Qt::blue);
    painter.drawEllipse(handleX, 5, 20, 20);
}

void VideoSlider::mousePressEvent(QMouseEvent *event) {
    m_value = valueFromPos(event->pos().x());
    update();
}

void VideoSlider::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        m_value = valueFromPos(event->pos().x());
        update();
    }
}

void VideoSlider::mouseReleaseEvent(QMouseEvent *event)
{
    m_sliderClicked = false;
}

QRectF VideoSlider::carrotHandleRect() const
{
    const double distanceFromMin = (m_value - m_minimum);
    const double sliderRange = (m_maximum - m_minimum);
    const double sliderPercent = (distanceFromMin / sliderRange);
    return handleRect(sliderPercent * validLength() + m_leftRightMargin);
}

QRectF VideoSlider::handleRect(const int value) const
{
    return QRect(value, (height() - m_indicatorSideLength) / 2, m_indicatorSideLength, m_indicatorSideLength);
}

int VideoSlider::validLength() const
{
    int len = width();
    return len - m_leftRightMargin * 2 - m_indicatorSideLength * (1);
}

int VideoSlider::valueFromPos(const int x) const
{
    const int val = static_cast<double>(x) / width() * m_maximum;
    return qBound(0, val, m_maximum);
}

QSize VideoSlider::minimumSizeHint() const
{
    return QSize(m_indicatorSideLength * 2 + m_leftRightMargin * 2, m_indicatorSideLength);
}

int VideoSlider::GetMinimun() const
{
    return m_minimum;
}

void VideoSlider::SetMinimum(const int minimum)
{
    setMinimum(minimum);
}

int VideoSlider::GetMaximun() const
{
    return m_maximum;
}

void VideoSlider::SetMaximum(const int maximum)
{
    setMaximum(maximum);
}

int VideoSlider::GetValue() const
{
    return m_value;
}

void VideoSlider::SetValue(const int value)
{
    setValue(value);
}

void VideoSlider::SetRange(const int minimum, const int maximum)
{
    if (maximum < m_minimum) {
        qWarning() << "Failed to set video slider maximum: " << QString::number(maximum) << ". Minimum Value: " << m_minimum;
        return;
    }

    if (minimum > m_maximum) {
        qWarning() << "Failed to set video slider minimum: " << QString::number(minimum) << ". Maximum Value: " << m_maximum;
        return;
    }

    m_minimum = minimum;
    m_maximum = maximum;
    emit rangeChanged(m_minimum, m_maximum);
    update();
}

double VideoSlider::GetSliderPercent() const
{
    return m_sliderPercent;
}

void VideoSlider::setSliderPercent(const double percent)
{
    m_sliderPercent = percent;
}

bool VideoSlider::GetIsSliderDown() const
{
    return m_sliderClicked;
}

void VideoSlider::SetIsSliderDown(const bool value)
{
    m_sliderClicked = value;
}

bool VideoSlider::GetVideoLoaded() const
{
    return m_videoLoaded;
}

void VideoSlider::SetVideoLoaded(const bool value)
{
    m_videoLoaded = value;
}

bool VideoSlider::GetShowingMarkers() const
{
    return m_showingMarkers;
}

void VideoSlider::SetShowingMarkers(const bool value)
{
    m_showingMarkers = value;
    update();
}

bool VideoSlider::GetShowingCumshotMarkers() const
{
    return m_showingCumshotMarkers;
}

void VideoSlider::SetShowingCumshotMarkers(const bool value)
{
    m_showingCumshotMarkers = value;
    update();
}

bool VideoSlider::GetShowingCyanMarkers() const
{
    return m_showingCyanMarkers;
}

void VideoSlider::SetShowingCyanMarkers(const bool value)
{
    m_showingCyanMarkers = value;
    update();
}

bool VideoSlider::GetShowingDialogMarkers() const
{
    return m_showingDialogMarkers;
}

void VideoSlider::SetShowingDialogMarkers(const bool value)
{
    m_showingDialogMarkers = value;
    update();
}

bool VideoSlider::GetShowingMagentaMarkers() const
{
    return m_showingMagentaMarkers;
}

void VideoSlider::SetShowingMagentaMarkers(const bool value)
{
    m_showingMagentaMarkers = value;
    update();
}

bool VideoSlider::GetShowingOrangeMarkers() const
{
    return m_showingOrangeMarkers;
}

void VideoSlider::SetShowingOrangeMarkers(const bool value)
{
    m_showingOrangeMarkers = value;
    update();
}

bool VideoSlider::GetShowingSceneMarkers() const
{
    return m_showingSceneMarkers;
}

void VideoSlider::SetShowingSceneMarkers(const bool value)
{
    m_showingSceneMarkers = value;
    update();
}

bool VideoSlider::GetShowingStripMarkers() const
{
    return m_showingStripMarkers;
}

void VideoSlider::SetShowingStripMarkers(const bool value)
{
    m_showingStripMarkers = value;
    update();
}

// PUBLIC SLOTS
void VideoSlider::updateVideoSlider()
{
    update();
}

void VideoSlider::setValue(const int value)
{
    if (value >= m_minimum && value <= m_maximum) {
        m_value = value;

    } else {
        if (value < m_minimum) {
            m_value = m_minimum;

        } else if (value > m_maximum) {
            m_value = m_maximum;

        } else {
            qWarning() << "Failed to set video slider value: " << QString::number(value) << ". Maximum Value: " << m_maximum << ". Minimum Value: " << m_minimum;
            return;
        }
    }

    emit valueChanged(m_value);
    update();
}

void VideoSlider::setMinimum(const int minimum)
{
    if (minimum <= m_maximum) {
        m_minimum = minimum;

    } else {
        qWarning() << "Failed to set video slider minimum: " << QString::number(minimum) << ". Maximum Value: " << m_maximum;
        return;
    }

    emit rangeChanged(m_minimum, m_maximum);
    update();
}

void VideoSlider::setMaximum(const int maximum)
{
    if (maximum >= m_minimum) {
        m_maximum = maximum;

    } else {
        qWarning() << "Failed to set video slider maximum: " << QString::number(maximum) << ". Minimum Value: " << m_minimum;
        return;
    }

    emit rangeChanged(m_minimum, m_maximum);
    update();
}

void VideoSlider::goToNextMarker(const double currentPercent)
{
    double newVal = 0.0;

    for (const VuraVideoMarker &marker : *m_videoMarkers) {
        if (marker.markerType == "marker" && m_showingMarkers) {
            if (marker.timestamp > currentPercent + 0.001) {
                if (newVal != 0.0) {
                    if (marker.timestamp < newVal)
                        newVal = marker.timestamp;

                } else {
                    newVal = marker.timestamp;
                }
            }
        } else if (marker.markerType == "scene" && m_showingSceneMarkers) {
            if (marker.timestamp > currentPercent + 0.001) {
                if (newVal != 0.0) {
                    if (marker.timestamp < newVal)
                        newVal = marker.timestamp;

                } else {
                    newVal = marker.timestamp;
                }
            }
        } else if (marker.markerType == "cumshot" && m_showingCumshotMarkers) {
            if (marker.timestamp > currentPercent + 0.001) {
                if (newVal != 0.0) {
                    if (marker.timestamp < newVal)
                        newVal = marker.timestamp;

                } else {
                    newVal = marker.timestamp;
                }
            }
        } else if (marker.markerType == "strip" && m_showingStripMarkers) {
            if (marker.timestamp > currentPercent + 0.001) {
                if (newVal != 0.0) {
                    if (marker.timestamp < newVal)
                        newVal = marker.timestamp;

                } else {
                    newVal = marker.timestamp;
                }
            }
        } else if (marker.markerType == "dialog" && m_showingDialogMarkers) {
            if (marker.timestamp > currentPercent + 0.001) {
                if (newVal != 0.0) {
                    if (marker.timestamp < newVal)
                        newVal = marker.timestamp;

                } else {
                    newVal = marker.timestamp;
                }
            }
        } else if (marker.markerType == "cyan" && m_showingCyanMarkers) {
            if (marker.timestamp > currentPercent + 0.001) {
                if (newVal != 0.0) {
                    if (marker.timestamp < newVal)
                        newVal = marker.timestamp;

                } else {
                    newVal = marker.timestamp;
                }
            }
        } else if (marker.markerType == "magenta" && m_showingMagentaMarkers) {
            if (marker.timestamp > currentPercent + 0.001) {
                if (newVal != 0.0) {
                    if (marker.timestamp < newVal)
                        newVal = marker.timestamp;

                } else {
                    newVal = marker.timestamp;
                }
            }
        } else if (marker.markerType == "orange" && m_showingOrangeMarkers) {
            if (marker.timestamp > currentPercent + 0.001) {
                if (newVal != 0.0) {
                    if (marker.timestamp < newVal)
                        newVal = marker.timestamp;

                } else {
                    newVal = marker.timestamp;
                }
            }
        }
    }

    if (newVal != 0.0) {
        int x = newVal * (m_maximum - m_minimum);
        setValue(x);
    }
}

void VideoSlider::goToPreviousMarker(const double currentPercent)
{
    double newVal = 0.0;

    for (const VuraVideoMarker &marker : *m_videoMarkers) {
        if (marker.markerType == "marker" && m_showingMarkers) {
            if (marker.timestamp < currentPercent - 0.001) {
                if (newVal != 0.0) {
                    if (marker.timestamp > newVal)
                        newVal = marker.timestamp;

                } else {
                    newVal = marker.timestamp;
                }
            }
        } else if (marker.markerType == "scene" && m_showingSceneMarkers) {
            if (marker.timestamp < currentPercent - 0.001) {
                if (newVal != 0.0) {
                    if (marker.timestamp > newVal)
                        newVal = marker.timestamp;

                } else {
                    newVal = marker.timestamp;
                }
            }
        } else if (marker.markerType == "cumshot" && m_showingCumshotMarkers) {
            if (marker.timestamp < currentPercent - 0.001) {
                if (newVal != 0.0) {
                    if (marker.timestamp > newVal)
                        newVal = marker.timestamp;

                } else {
                    newVal = marker.timestamp;
                }
            }
        } else if (marker.markerType == "strip" && m_showingStripMarkers) {
            if (marker.timestamp < currentPercent - 0.001) {
                if (newVal != 0.0) {
                    if (marker.timestamp > newVal)
                        newVal = marker.timestamp;

                } else {
                    newVal = marker.timestamp;
                }
            }
        } else if (marker.markerType == "dialog" && m_showingDialogMarkers) {
            if (marker.timestamp < currentPercent - 0.001) {
                if (newVal != 0.0) {
                    if (marker.timestamp > newVal)
                        newVal = marker.timestamp;

                } else {
                    newVal = marker.timestamp;
                }
            }
        } else if (marker.markerType == "cyan" && m_showingCyanMarkers) {
            if (marker.timestamp < currentPercent - 0.001) {
                if (newVal != 0.0) {
                    if (marker.timestamp > newVal)
                        newVal = marker.timestamp;

                } else {
                    newVal = marker.timestamp;
                }
            }
        } else if (marker.markerType == "magenta" && m_showingMagentaMarkers) {
            if (marker.timestamp < currentPercent - 0.001) {
                if (newVal != 0.0) {
                    if (marker.timestamp > newVal)
                        newVal = marker.timestamp;

                } else {
                    newVal = marker.timestamp;
                }
            }
        } else if (marker.markerType == "orange" && m_showingOrangeMarkers) {
            if (marker.timestamp < currentPercent - 0.001) {
                if (newVal != 0.0) {
                    if (marker.timestamp > newVal)
                        newVal = marker.timestamp;

                } else {
                    newVal = marker.timestamp;
                }
            }
        }
    }

    if (newVal != 0.0) {
        int x = newVal * (m_maximum - m_minimum);
        setValue(x);
    }
}
