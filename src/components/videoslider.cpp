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
constexpr int m_sliderBarHeight = 5;
constexpr int m_markerHeight = 20;
constexpr int m_markerWidth = 2;
constexpr int m_indicatorSideLength = 14;
constexpr int m_leftRightMargin = 1;
constexpr int m_bottomMargin = 8;
}


VideoSlider::VideoSlider(QList<VuraVideoMarker> *videoMarkers, QWidget *parent)
    : QWidget(parent),
    m_videoMarkers(videoMarkers),
    m_minimum(0),
    m_maximum(0),
    m_value(0),
    m_showingMarkers(true),
    m_showingCumshotMarkers(true),
    m_showingCyanMarkers(true),
    m_showingDialogMarkers(true),
    m_showingMagentaMarkers(true),
    m_showingOrangeMarkers(true),
    m_showingSceneMarkers(true),
    m_showingStripMarkers(true),
    m_sliderPressed(false),
    m_emptySliderColor(QColor(0xD0, 0xD0, 0xD0)),
    m_fullSliderColor(QColor(0, 217, 255)),
    m_caretColor(QColor(0xFA, 0xFA, 0xFA)),
    m_markerColor(QColor()),
    m_sceneMarkerColor(QColor()),
    m_cumshotMarkerColor(QColor()),
    m_stripMarkerColor(QColor()),
    m_dialogMarkerColor(QColor())
{
    setMouseTracking(true);
    this->setFixedHeight(30);

    m_sliderBarHeightValue = (height() - m_sliderBarHeight) - m_bottomMargin;
}

void VideoSlider::paintEvent(QPaintEvent *event)
{
    m_sliderBarHeightValue = (height() - m_sliderBarHeight) - m_bottomMargin;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Draw Slider Bar
    const auto sliderBarRect =
        QRectF(m_leftRightMargin, (height() - m_sliderBarHeight) - m_bottomMargin, width() - m_leftRightMargin * 2, m_sliderBarHeight);

    QPen pen(1);
    painter.setPen(pen);
    painter.setBrush(m_emptySliderColor);
    painter.drawRoundedRect(sliderBarRect, 1, 1);

    // Draw Markers
    for (const VuraVideoMarker &marker : *m_videoMarkers) {
        if (marker.markerType == "marker" && m_showingMarkers) {
            if (marker.timestamp > m_minimum && marker.timestamp < m_maximum) {
                painter.setPen(QPen(Qt::green, m_markerWidth));
                const int x = (marker.timestamp * validLength() + m_leftRightMargin) + (m_indicatorSideLength / 2);
                painter.drawLine(x, 0, x, m_markerHeight);
            }

        } else if (marker.markerType == "scene" && m_showingSceneMarkers) {
            if (marker.timestamp > m_minimum && marker.timestamp < m_maximum) {
                painter.setPen(QPen(Qt::blue, m_markerWidth));
                const int x = (marker.timestamp * validLength() + m_leftRightMargin) + (m_indicatorSideLength / 2);
                painter.drawLine(x, 0, x, m_markerHeight);
            }

        } else if (marker.markerType == "cumshot" && m_showingCumshotMarkers) {
            if (marker.timestamp > m_minimum && marker.timestamp < m_maximum) {
                painter.setPen(QPen(Qt::white, m_markerWidth));
                const int x = (marker.timestamp * validLength() + m_leftRightMargin) + (m_indicatorSideLength / 2);
                painter.drawLine(x, 0, x, m_markerHeight);
            }

        } else if (marker.markerType == "strip" && m_showingStripMarkers) {
            if (marker.timestamp > m_minimum && marker.timestamp < m_maximum) {
                painter.setPen(QPen(Qt::red, m_markerWidth));
                const int x = (marker.timestamp * validLength() + m_leftRightMargin) + (m_indicatorSideLength / 2);
                painter.drawLine(x, 0, x, m_markerHeight);
            }

        } else if (marker.markerType == "dialog" && m_showingDialogMarkers) {
            if (marker.timestamp > m_minimum && marker.timestamp < m_maximum) {
                painter.setPen(QPen(Qt::yellow, m_markerWidth));
                const int x = (marker.timestamp * validLength() + m_leftRightMargin) + (m_indicatorSideLength / 2);
                painter.drawLine(x, 0, x, m_markerHeight);
            }

        } else if (marker.markerType == "cyan" && m_showingCyanMarkers) {
            if (marker.timestamp > m_minimum && marker.timestamp < m_maximum) {
                painter.setPen(QPen(Qt::cyan, m_markerWidth));
                const int x = (marker.timestamp * validLength() + m_leftRightMargin) + (m_indicatorSideLength / 2);
                painter.drawLine(x, 0, x, m_markerHeight);
            }

        } else if (marker.markerType == "magenta" && m_showingMagentaMarkers) {
            if (marker.timestamp > m_minimum && marker.timestamp < m_maximum) {
                painter.setPen(QPen(Qt::magenta, m_markerWidth));
                const int x = (marker.timestamp * validLength() + m_leftRightMargin) + (m_indicatorSideLength / 2);
                painter.drawLine(x, 0, x, m_markerHeight);
            }

        } else if (marker.markerType == "orange" && m_showingOrangeMarkers) {
            if (marker.timestamp > m_minimum && marker.timestamp < m_maximum) {
                painter.setPen(QPen(Qt::darkYellow, m_markerWidth));
                const int x = (marker.timestamp * validLength() + m_leftRightMargin) + (m_indicatorSideLength / 2);
                painter.drawLine(x, 0, x, m_markerHeight);
            }
        }
    }

    // Draw Carrot
    pen.setColor(m_caretColor);
    pen.setWidth(1);
    painter.setPen(pen);
    painter.setBrush(m_caretColor);
    painter.drawRoundedRect(carrotHandleRect(), 2, 2);
}

void VideoSlider::mousePressEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        if (!m_sliderPressed) {
            m_sliderPressed = true;
            emit sliderPressed(m_sliderPressed);
            const int value = valueFromPos(event->pos().x());
            setValue(value);
        }
    }
}

void VideoSlider::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        const int value = valueFromPos(event->pos().x());
        setValue(value);
    }
}

void VideoSlider::mouseReleaseEvent(QMouseEvent *event)
{
    m_sliderPressed = false;
    emit sliderPressed(m_sliderPressed);
}

QRectF VideoSlider::carrotHandleRect() const
{
    const double distanceFromMin = (GetValue() - GetMinimun());
    const double sliderRange = (GetMaximun() - GetMinimun());
    const double sliderPercent = (distanceFromMin / sliderRange);

    return handleRect(sliderPercent * validLength() + m_leftRightMargin);
}

QRectF VideoSlider::handleRect(const int value) const
{
    return QRect(value, ((m_sliderBarHeightValue + m_indicatorSideLength) / 2)-3, m_indicatorSideLength, m_indicatorSideLength);
}

int VideoSlider::validLength() const
{
    const int len = width();
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
    update();
}

bool VideoSlider::GetSliderPressed() const
{
    return m_sliderPressed;
}

void VideoSlider::SetSliderPressed(const bool value)
{
    m_sliderPressed = value;
}

bool VideoSlider::getMarkerTypesVisible(const QString &markerType) const
{
    if (markerType == "marker") return m_showingMarkers;
    if (markerType == "cumshot") return m_showingCumshotMarkers;
    if (markerType == "cyan") return m_showingCyanMarkers;
    if (markerType == "dialog") return m_showingDialogMarkers;
    if (markerType == "magenta") return m_showingMagentaMarkers;
    if (markerType == "orange") return m_showingOrangeMarkers;
    if (markerType == "scene") return m_showingSceneMarkers;
    if (markerType == "strip") return m_showingStripMarkers;

    return false;
}

void VideoSlider::setMarkerTypeVisible(const QString& markerType, const bool visible)
{
    if (markerType == "marker") m_showingMarkers = visible;
    if (markerType == "cumshot") m_showingCumshotMarkers = visible;
    if (markerType == "cyan") m_showingCyanMarkers = visible;
    if (markerType == "dialog") m_showingDialogMarkers = visible;
    if (markerType == "magenta") m_showingMagentaMarkers = visible;
    if (markerType == "orange") m_showingOrangeMarkers = visible;
    if (markerType == "scene") m_showingSceneMarkers = visible;
    if (markerType == "strip") m_showingStripMarkers = visible;

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
    if (m_sliderPressed)
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
        emit valueChanged(x);
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
        emit valueChanged(x);
    }
}
