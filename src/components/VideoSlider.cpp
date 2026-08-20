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

#include "VideoSlider.h"


VideoSlider::VideoSlider(VideoMarkerController *videoMarkerController, QWidget *parent)
    : QWidget(parent),
    m_videoMarkerController(videoMarkerController),
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
    m_sliderPressed(false)
{
    setMouseTracking(true);
    this->setFixedHeight(20);
}

void VideoSlider::paintEvent(QPaintEvent *event)
{
    QSettings settings;
    int sliderHeight = settings.value("sliderHeight", 6).toInt();
    int videoMarkerHeight = settings.value("videoMarkerHeight", 10).toInt();
    int videoMarkerWidth = settings.value("videoMarkerWidth", 2).toInt();
    int videoMarkerSideLength = settings.value("videoMarkerSideLength", 14).toInt();
    int videoMarkerVerticalMargin = settings.value("videoMarkerVerticalMargin", 0).toInt();
    int handleRadius = settings.value("sliderHandleRadius", 8).toInt();
    int padding = handleRadius + settings.value("sliderPadding", 2).toInt();
    int sliderLeftRightMargin = settings.value("sliderLeftRightMargin", 1).toInt();

    QString emptySliderColor = settings.value("emptySliderColor", "#000000").toString();
    QString fullSliderColor = settings.value("fullSliderColor", "#00d9ff").toString();
    QString caretColor = settings.value("carretColor", "#ffffff").toString();
    QString backgroundTrackColor = settings.value("backgroundTrackColor", "#323232").toString();
    QString progressFillColor = settings.value("progressFillColor", "#0076d7").toString();
    QString handlePlayheadColor = settings.value("handlePlayheadColor", "#ffffff").toString();
    QString markerColor = settings.value("markerColor", "#03c200").toString();
    QString sceneMarkerColor = settings.value("sceneMarkerColor", "#000eab").toString();
    QString cumshotMarkerColor = settings.value("cumshotMarkerColor", "#ffffff").toString();
    QString stripMarkerColor = settings.value("stripMarkerColor", "#cf0202").toString();
    QString dialogMarkerColor = settings.value("dialogMarkerColor", "#e0f500").toString();
    QString cyanMarkerColor = settings.value("cyanMarkerColor", "#00edf5").toString();
    QString magentaMarkerColor = settings.value("magentaMarkerColor", "#f5007e").toString();
    QString orangeMarkerColor = settings.value("orangeMarkerColor", "#f56a00").toString();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const int yCenter = height() / 2;
    const int trackWidth = width() - (padding * 2);

    // Draw the Background Track
    const QRectF bgRect(padding, yCenter - (sliderHeight / 2.0), trackWidth, sliderHeight);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(backgroundTrackColor));
    painter.drawRoundedRect(bgRect, sliderHeight / 2.0, sliderHeight / 2.0);

    // Draw the Progress Fill
    const float fillWidth = trackWidth * m_sliderPercent;
    const QRectF fillRect(padding, yCenter - (sliderHeight / 2.0), fillWidth, sliderHeight);
    painter.setBrush(QColor(progressFillColor));
    painter.drawRoundedRect(fillRect, sliderHeight / 2.0, sliderHeight / 2.0);

    // Draw the Handle / Playhead
    const float handleX = padding + fillWidth;
    painter.setBrush(handlePlayheadColor);
    painter.drawEllipse(QPointF(handleX, yCenter), handleRadius, handleRadius);

    // Draw Markers
    for (const VideoMarkerRecord &marker : m_videoMarkers) {
        if (marker.markerType == "marker" && m_showingMarkers) {
            if (marker.timestampMs > m_minimum && marker.timestampMs < m_maximum) {
                painter.setPen(QPen(QColor(markerColor), videoMarkerWidth));
                const int x = (marker.timestampMs * validLength() + sliderLeftRightMargin) + (videoMarkerSideLength / 2);
                painter.drawLine(x, videoMarkerVerticalMargin, x, videoMarkerHeight);
            }
        } else if (marker.markerType == "scene" && m_showingSceneMarkers) {
            if (marker.timestampMs > m_minimum && marker.timestampMs < m_maximum) {
                painter.setPen(QPen(QColor(sceneMarkerColor), videoMarkerWidth));
                const int x = (marker.timestampMs * validLength() + sliderLeftRightMargin) + (videoMarkerSideLength / 2);
                painter.drawLine(x, videoMarkerVerticalMargin, x, videoMarkerHeight);
            }
        } else if (marker.markerType == "cumshot" && m_showingCumshotMarkers) {
            if (marker.timestampMs > m_minimum && marker.timestampMs < m_maximum) {
                painter.setPen(QPen(QColor(cumshotMarkerColor), videoMarkerWidth));
                const int x = (marker.timestampMs * validLength() + sliderLeftRightMargin) + (videoMarkerSideLength / 2);
                painter.drawLine(x, videoMarkerVerticalMargin, x, videoMarkerHeight);
            }
        } else if (marker.markerType == "strip" && m_showingStripMarkers) {
            if (marker.timestampMs > m_minimum && marker.timestampMs < m_maximum) {
                painter.setPen(QPen(QColor(stripMarkerColor), videoMarkerWidth));
                const int x = (marker.timestampMs * validLength() + sliderLeftRightMargin) + (videoMarkerSideLength / 2);
                painter.drawLine(x, videoMarkerVerticalMargin, x, videoMarkerHeight);
            }
        } else if (marker.markerType == "dialog" && m_showingDialogMarkers) {
            if (marker.timestampMs > m_minimum && marker.timestampMs < m_maximum) {
                painter.setPen(QPen(QColor(dialogMarkerColor), videoMarkerWidth));
                const int x = (marker.timestampMs * validLength() + sliderLeftRightMargin) + (videoMarkerSideLength / 2);
                painter.drawLine(x, videoMarkerVerticalMargin, x, videoMarkerHeight);
            }
        } else if (marker.markerType == "cyan" && m_showingCyanMarkers) {
            if (marker.timestampMs > m_minimum && marker.timestampMs < m_maximum) {
                painter.setPen(QPen(QColor(cyanMarkerColor), videoMarkerWidth));
                const int x = (marker.timestampMs * validLength() + sliderLeftRightMargin) + (videoMarkerSideLength / 2);
                painter.drawLine(x, videoMarkerVerticalMargin, x, videoMarkerHeight);
            }
        } else if (marker.markerType == "magenta" && m_showingMagentaMarkers) {
            if (marker.timestampMs > m_minimum && marker.timestampMs < m_maximum) {
                painter.setPen(QPen(QColor(magentaMarkerColor), videoMarkerWidth));
                const int x = (marker.timestampMs * validLength() + sliderLeftRightMargin) + (videoMarkerSideLength / 2);
                painter.drawLine(x, videoMarkerVerticalMargin, x, videoMarkerHeight);
            }
        } else if (marker.markerType == "orange" && m_showingOrangeMarkers) {
            if (marker.timestampMs > m_minimum && marker.timestampMs < m_maximum) {
                painter.setPen(QPen(QColor(orangeMarkerColor), videoMarkerWidth));
                const int x = (marker.timestampMs * validLength() + sliderLeftRightMargin) + (videoMarkerSideLength / 2);
                painter.drawLine(x, videoMarkerVerticalMargin, x, videoMarkerHeight);
            }
        }
    }
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
    QWidget::mouseMoveEvent(event);
}

void VideoSlider::mouseReleaseEvent(QMouseEvent *event)
{
    m_sliderPressed = false;
    emit sliderPressed(m_sliderPressed);
}

QRectF VideoSlider::carrotHandleRect() const
{
    const QSettings settings;
    const int sliderLeftRightMargin = settings.value("sliderLeftRightMargin", 1).toInt();

    const double distanceFromMin = (GetValue() - GetMinimun());
    const double sliderRange = (GetMaximun() - GetMinimun());
    const double sliderPercent = (distanceFromMin / sliderRange);

    return handleRect(sliderPercent * validLength() + sliderLeftRightMargin);
}

QRectF VideoSlider::handleRect(const int value) const
{
    const QSettings settings;
    const int videoMarkerSideLength = settings.value("videoMarkerSideLength", 14).toInt();
    return QRect(value, ((m_sliderBarHeightValue + videoMarkerSideLength) / 2)-3, videoMarkerSideLength, videoMarkerSideLength);
}

int VideoSlider::validLength() const
{
    const QSettings settings;
    const int videoMarkerSideLength = settings.value("videoMarkerSideLength", 14).toInt();
    const int sliderLeftRightMargin = settings.value("sliderLeftRightMargin", 1).toInt();
    const int len = width();
    return len - sliderLeftRightMargin * 2 - videoMarkerSideLength * (1);
}

int VideoSlider::valueFromPos(const int x) const
{
    const int val = static_cast<int>((static_cast<double>(x) / width()) * m_maximum);
    return qBound(0, val, m_maximum);
}

QSize VideoSlider::minimumSizeHint() const
{
    const QSettings settings;
    int videoMarkerSideLength = settings.value("videoMarkerSideLength", 14).toInt();
    const int sliderLeftRightMargin = settings.value("sliderLeftRightMargin", 1).toInt();
    return {videoMarkerSideLength * 2 + sliderLeftRightMargin * 2, videoMarkerSideLength};
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

void VideoSlider::updateVideoSlider()
{
    m_videoMarkers.clear();
    m_videoMarkers = m_videoMarkerController->getVideoMarkers();
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

    constexpr int padding = 8;
    int trackWidth = width() - (padding * 2);

    const double distanceFromMin = (GetValue() - GetMinimun());
    const double sliderRange = (GetMaximun() - GetMinimun());
    const double sliderPercent = (distanceFromMin / sliderRange);
    const auto percentage = static_cast<float>(sliderPercent);

    // Clamp between 0.0 and 1.0
    m_sliderPercent = std::clamp(percentage, 0.0f, 1.0f);
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

    for (const VideoMarkerRecord &marker : m_videoMarkers) {
        if (marker.markerType == "marker" && m_showingMarkers) {
            if (marker.timestampMs > currentPercent + 0.001) {
                if (newVal != 0.0) {
                    if (marker.timestampMs < newVal)
                        newVal = marker.timestampMs;
                } else {
                    newVal = marker.timestampMs;
                }
            }
        } else if (marker.markerType == "scene" && m_showingSceneMarkers) {
            if (marker.timestampMs > currentPercent + 0.001) {
                if (newVal != 0.0) {
                    if (marker.timestampMs < newVal)
                        newVal = marker.timestampMs;
                } else {
                    newVal = marker.timestampMs;
                }
            }
        } else if (marker.markerType == "cumshot" && m_showingCumshotMarkers) {
            if (marker.timestampMs > currentPercent + 0.001) {
                if (newVal != 0.0) {
                    if (marker.timestampMs < newVal)
                        newVal = marker.timestampMs;
                } else {
                    newVal = marker.timestampMs;
                }
            }
        } else if (marker.markerType == "strip" && m_showingStripMarkers) {
            if (marker.timestampMs > currentPercent + 0.001) {
                if (newVal != 0.0) {
                    if (marker.timestampMs < newVal)
                        newVal = marker.timestampMs;
                } else {
                    newVal = marker.timestampMs;
                }
            }
        } else if (marker.markerType == "dialog" && m_showingDialogMarkers) {
            if (marker.timestampMs > currentPercent + 0.001) {
                if (newVal != 0.0) {
                    if (marker.timestampMs < newVal)
                        newVal = marker.timestampMs;
                } else {
                    newVal = marker.timestampMs;
                }
            }
        } else if (marker.markerType == "cyan" && m_showingCyanMarkers) {
            if (marker.timestampMs > currentPercent + 0.001) {
                if (newVal != 0.0) {
                    if (marker.timestampMs < newVal)
                        newVal = marker.timestampMs;
                } else {
                    newVal = marker.timestampMs;
                }
            }
        } else if (marker.markerType == "magenta" && m_showingMagentaMarkers) {
            if (marker.timestampMs > currentPercent + 0.001) {
                if (newVal != 0.0) {
                    if (marker.timestampMs < newVal)
                        newVal = marker.timestampMs;
                } else {
                    newVal = marker.timestampMs;
                }
            }
        } else if (marker.markerType == "orange" && m_showingOrangeMarkers) {
            if (marker.timestampMs > currentPercent + 0.001) {
                if (newVal != 0.0) {
                    if (marker.timestampMs < newVal)
                        newVal = marker.timestampMs;
                } else {
                    newVal = marker.timestampMs;
                }
            }
        }
    }

    if (newVal != 0.0) {
        const int x = newVal * (m_maximum - m_minimum);
        emit valueChanged(x);
    }
}

void VideoSlider::goToPreviousMarker(const double currentPercent)
{
    double newVal = 0.0;

    for (const VideoMarkerRecord &marker : m_videoMarkers) {
        if (marker.markerType == "marker" && m_showingMarkers) {
            if (marker.timestampMs < currentPercent - 0.001) {
                if (newVal != 0.0) {
                    if (marker.timestampMs > newVal)
                        newVal = marker.timestampMs;
                } else {
                    newVal = marker.timestampMs;
                }
            }
        } else if (marker.markerType == "scene" && m_showingSceneMarkers) {
            if (marker.timestampMs < currentPercent - 0.001) {
                if (newVal != 0.0) {
                    if (marker.timestampMs > newVal)
                        newVal = marker.timestampMs;
                } else {
                    newVal = marker.timestampMs;
                }
            }
        } else if (marker.markerType == "cumshot" && m_showingCumshotMarkers) {
            if (marker.timestampMs < currentPercent - 0.001) {
                if (newVal != 0.0) {
                    if (marker.timestampMs > newVal)
                        newVal = marker.timestampMs;
                } else {
                    newVal = marker.timestampMs;
                }
            }
        } else if (marker.markerType == "strip" && m_showingStripMarkers) {
            if (marker.timestampMs < currentPercent - 0.001) {
                if (newVal != 0.0) {
                    if (marker.timestampMs > newVal)
                        newVal = marker.timestampMs;
                } else {
                    newVal = marker.timestampMs;
                }
            }
        } else if (marker.markerType == "dialog" && m_showingDialogMarkers) {
            if (marker.timestampMs < currentPercent - 0.001) {
                if (newVal != 0.0) {
                    if (marker.timestampMs > newVal)
                        newVal = marker.timestampMs;
                } else {
                    newVal = marker.timestampMs;
                }
            }
        } else if (marker.markerType == "cyan" && m_showingCyanMarkers) {
            if (marker.timestampMs < currentPercent - 0.001) {
                if (newVal != 0.0) {
                    if (marker.timestampMs > newVal)
                        newVal = marker.timestampMs;
                } else {
                    newVal = marker.timestampMs;
                }
            }
        } else if (marker.markerType == "magenta" && m_showingMagentaMarkers) {
            if (marker.timestampMs < currentPercent - 0.001) {
                if (newVal != 0.0) {
                    if (marker.timestampMs > newVal)
                        newVal = marker.timestampMs;
                } else {
                    newVal = marker.timestampMs;
                }
            }
        } else if (marker.markerType == "orange" && m_showingOrangeMarkers) {
            if (marker.timestampMs < currentPercent - 0.001) {
                if (newVal != 0.0) {
                    if (marker.timestampMs > newVal)
                        newVal = marker.timestampMs;
                } else {
                    newVal = marker.timestampMs;
                }
            }
        }
    }

    if (newVal != 0.0) {
        const int x = newVal * (m_maximum - m_minimum);
        emit valueChanged(x);
    }
}

void VideoSlider::loadVideoMarkers()
{
    m_videoMarkers.clear();
    m_videoMarkers = m_videoMarkerController->getVideoMarkers();
    update();
}
