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
    const int m_height = 5;
    const int m_markerHeight = 10;
    const int m_markerWidth = 2;
    const int m_horizontalMarginOffset = 14;
    const int m_bottomMarginOffset = 10;
    const int m_caretRadius = 6;
}


VideoSlider::VideoSlider(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_Hover, true);
    this->setFixedHeight(30);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &VideoSlider::hideIndicator);
}

void VideoSlider::setMinimum(int mminimum)
{
    if (mminimum < m_maximum) {
        m_minimum = mminimum;
        update();
    }
}

void VideoSlider::setMaximum(int mmaximum)
{
    if (mmaximum > m_minimum) {
        m_maximum = mmaximum;
        update();
    }
}

void VideoSlider::setValue(int mvalue)
{
    m_value = mvalue;
    update();
}

void VideoSlider::setSliderPercent(double percent)
{
    m_sliderPercent = percent;
}

void VideoSlider::setVideoLoaded(bool isLoaded)
{
    m_videoLoaded = isLoaded;
}

void VideoSlider::setShowingMarkers(bool value)
{
    m_showingMarkers = value;
    update();
}

void VideoSlider::setShowingCumshotMarkers(bool value)
{
    m_showingCumshotMarkers = value;
    update();
}

void VideoSlider::setShowingCyanMarkers(bool value)
{
    m_showingCyanMarkers = value;
    update();
}

void VideoSlider::setShowingDialogMarkers(bool value)
{
    m_showingDialogMarkers = value;
    update();
}

void VideoSlider::setShowingMagentaMarkers(bool value)
{
    m_showingMagentaMarkers = value;
    update();
}

void VideoSlider::setShowingOrangeMarkers(bool value)
{
    m_showingOrangeMarkers = value;
    update();
}

void VideoSlider::setShowingSceneMarkers(bool value)
{
    m_showingSceneMarkers = value;
    update();
}

void VideoSlider::setShowingStripMarkers(bool value)
{
    m_showingStripMarkers = value;
    update();
}

void VideoSlider::setMarkers(QList<VuraVideoMarker> markers)
{
    m_markers = markers;
    update();
}

void VideoSlider::jumpToNextMarker(double currentPercent)
{
    double newVal = 0.0;

    for (const VuraVideoMarker &marker : m_markers) {
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
        int x = newVal * (this->maximum() - this->minimum());
        emit markerSelected(x);
    }
}

void VideoSlider::jumpToPreviousMarker(double currentPercent)
{
    double newVal = 0.0;

    for (const VuraVideoMarker &marker : m_markers) {
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
        int x = newVal * (this->maximum() - this->minimum());
        emit markerSelected(x);
    }
}

int VideoSlider::minimum() const { return m_minimum; }

int VideoSlider::maximum() const { return m_maximum; }

int VideoSlider::value() const { return m_value; }

double VideoSlider::sliderPercent() const { return m_sliderPercent; }

bool VideoSlider::isSliderDown() const { return m_isSliderDown; }

bool VideoSlider::isVideoLoaded() const { return m_videoLoaded; }

bool VideoSlider::showMarkers() const { return m_showingMarkers; }

bool VideoSlider::showCumshotMarkers() const { return m_showingCumshotMarkers; }

bool VideoSlider::showCyanMarkers() const { return m_showingCyanMarkers; }

bool VideoSlider::showDialogMarkers() const { return m_showingDialogMarkers; }

bool VideoSlider::showMagentaMarkers() const { return m_showingMagentaMarkers; }

bool VideoSlider::showOrangeMarkers() const { return m_showingOrangeMarkers; }

bool VideoSlider::showSceneMarkers() const { return m_showingSceneMarkers; }

bool VideoSlider::showStripMarkers() const { return m_showingStripMarkers; }

void VideoSlider::hideIndicator()
{
    m_showingIndicator = false;
    timer->stop();
}

void VideoSlider::enterEvent(QEnterEvent *event)
{
    if (m_videoLoaded) {
        m_showingIndicator = true;
    }
}

void VideoSlider::leaveEvent(QEvent *event)
{
    if (m_videoLoaded && m_showingIndicator) {
        timer->start(1000);
    }
}

void VideoSlider::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int currentWidth = this->width();

    if (!m_videoLoaded) {
        painter.setPen(QPen(m_emptySliderColor, 1));
        painter.setBrush(QBrush(m_emptySliderColor, Qt::SolidPattern));
        painter.drawRect((m_horizontalMarginOffset / 2),
            this->height() - m_bottomMarginOffset,
            currentWidth,
            m_height);

    } else {
        double distanceFromMin = (this->value() - this->minimum());
        double sliderRange = (this->maximum() - this->minimum());
        double sliderPercent = (distanceFromMin / sliderRange);

        int x = sliderPercent * currentWidth;

        // Draw progress portion of slider.
        painter.setPen(QPen(m_fullSliderColor, 1));
        painter.setBrush(QBrush(m_fullSliderColor, Qt::SolidPattern));
        painter.drawRect((m_horizontalMarginOffset / 2),
            this->height() - m_bottomMarginOffset,
            x - 7,
            m_height);

        // Draw remaining portion of slider.
        painter.setPen(QPen(m_emptySliderColor, 1));
        painter.setBrush(QBrush(m_emptySliderColor, Qt::SolidPattern));
        painter.drawRect(x,
            this->height() - m_bottomMarginOffset,
            currentWidth - (x - 28),
            m_height);

        // Draw caret.
        if (m_showingIndicator) {
            painter.setPen(QPen(Qt::lightGray, 1));
            painter.setBrush(QBrush(Qt::lightGray, Qt::SolidPattern));
            QPoint center(x,
                (this->height() - m_bottomMarginOffset) + (m_caretRadius / 2));

            painter.drawEllipse(center, m_caretRadius, m_caretRadius);
        }
    }

    for (const VuraVideoMarker &marker : m_markers) {
        if (marker.markerType == "marker" && m_showingMarkers) {
            if (marker.timestamp > this->minimum() && marker.timestamp < this->maximum()) {
                painter.setPen(QPen(Qt::green, m_markerWidth));
                int x = marker.timestamp * this->width();
                painter.drawLine(x, 0, x, m_markerHeight);
            }
        } else if (marker.markerType == "scene" && m_showingSceneMarkers) {
            if (marker.timestamp > this->minimum() && marker.timestamp < this->maximum()) {
                painter.setPen(QPen(Qt::blue, m_markerWidth));
                int x = marker.timestamp * this->width();
                painter.drawLine(x, 0, x, m_markerHeight);
            }
        } else if (marker.markerType == "cumshot" && m_showingCumshotMarkers) {
            if (marker.timestamp > this->minimum() && marker.timestamp < this->maximum()) {
                painter.setPen(QPen(Qt::white, m_markerWidth));
                int x = marker.timestamp * this->width();
                painter.drawLine(x, 0, x, m_markerHeight);
            }
        } else if (marker.markerType == "strip" && m_showingStripMarkers) {
            if (marker.timestamp > this->minimum() && marker.timestamp < this->maximum()) {
                painter.setPen(QPen(Qt::red, m_markerWidth));
                int x = marker.timestamp * this->width();
                painter.drawLine(x, 0, x, m_markerHeight);
            }
        } else if (marker.markerType == "dialog" && m_showingDialogMarkers) {
            if (marker.timestamp > this->minimum() && marker.timestamp < this->maximum()) {
                painter.setPen(QPen(Qt::yellow, m_markerWidth));
                int x = marker.timestamp * this->width();
                painter.drawLine(x, 0, x, m_markerHeight);
            }
        } else if (marker.markerType == "cyan" && m_showingCyanMarkers) {
            if (marker.timestamp > this->minimum() && marker.timestamp < this->maximum()) {
                painter.setPen(QPen(Qt::cyan, m_markerWidth));
                int x = marker.timestamp * this->width();
                painter.drawLine(x, 0, x, m_markerHeight);
            }
        } else if (marker.markerType == "magenta" && m_showingMagentaMarkers) {
            if (marker.timestamp > this->minimum() && marker.timestamp < this->maximum()) {
                painter.setPen(QPen(Qt::magenta, m_markerWidth));
                int x = marker.timestamp * this->width();
                painter.drawLine(x, 0, x, m_markerHeight);
            }
        } else if (marker.markerType == "orange" && m_showingOrangeMarkers) {
            if (marker.timestamp > this->minimum() && marker.timestamp < this->maximum()) {
                painter.setPen(QPen(Qt::darkYellow, m_markerWidth));
                int x = marker.timestamp * this->width();
                painter.drawLine(x, 0, x, m_markerHeight);
            }
        }
    }
}

void VideoSlider::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_isSliderDown = true;

        double posRatio = (double)event->pos().x() / this->width();
        m_value = m_minimum + posRatio * (m_maximum - m_minimum);
        update();
        //emit sliderClicked(std::round(m_value));
        event->accept();
    }
}

void VideoSlider::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_isSliderDown = false;
        emit sliderClicked(std::round(m_value));
    }
}
