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
        qDebug() << "Minimum set to: " << m_minimum;
        update();
    }
}

void VideoSlider::setMaximum(int mmaximum)
{
    if (mmaximum > m_minimum) {
        m_maximum = mmaximum;
        qDebug() << "Maximum set to: " << m_maximum;
        update();
    }
}

void VideoSlider::setValue(int mvalue)
{
    m_value = mvalue;
    qDebug() << "Value set to: " << m_value;
    update();
}

void VideoSlider::setVideoLoaded(bool isLoaded)
{
    m_videoLoaded = isLoaded;
}

void VideoSlider::setMarkers(QMap<QString,QList<double>> markers)
{
    m_markers = markers;
    update();
}

void VideoSlider::jumpToNextMarker(double currentPercent)
{
    double newVal = 0.0;

    QList<double> markerList;
    QList<double> markers = m_markers.value("marker").toList();
    QList<double> sceneMarkers = m_markers.value("scene").toList();
    QList<double> cumshotMarkers = m_markers.value("cumshot").toList();
    QList<double> stripMarkers = m_markers.value("strip").toList();
    QList<double> dialogMarkers = m_markers.value("dialog").toList();
    QList<double> cyanMarkers = m_markers.value("cyan").toList();
    QList<double> magentaMarkers = m_markers.value("magenta").toList();
    QList<double> orangeMarkers = m_markers.value("orange").toList();

    if (showMarkers) {
        foreach(const double marker, markers)
        {
            markerList.append(marker);
        }
    }

    if (showSceneMarkers) {
        foreach(const double marker, sceneMarkers)
        {
            markerList.append(marker);
        }
    }

    if (showCumshotMarkers) {
        foreach(const double marker, cumshotMarkers)
        {
            markerList.append(marker);
        }
    }

    if (showStripMarkers) {
        foreach(const double marker, stripMarkers)
        {
            markerList.append(marker);
        }
    }

    if (showDialogMarkers) {
        foreach(const double marker, dialogMarkers)
        {
            markerList.append(marker);
        }
    }

    if (showCyanMarkers) {
        foreach(const double marker, cyanMarkers)
        {
            markerList.append(marker);
        }
    }

    if (showMagentaMarkers) {
        foreach(const double marker, magentaMarkers)
        {
            markerList.append(marker);
        }
    }

    if (showOrangeMarkers) {
        foreach(const double marker, orangeMarkers)
        {
            markerList.append(marker);
        }
    }

    foreach(const double marker, markerList) {
        if (marker > currentPercent + 0.001) {
            if (newVal != 0.0) {
                if (marker < newVal) {
                    newVal = marker;
                }
            } else {
                newVal = marker;
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

    QList<double> markerList;
    QList<double> markers = m_markers.value("marker").toList();
    QList<double> sceneMarkers = m_markers.value("scene").toList();
    QList<double> cumshotMarkers = m_markers.value("cumshot").toList();
    QList<double> stripMarkers = m_markers.value("strip").toList();
    QList<double> dialogMarkers = m_markers.value("dialog").toList();
    QList<double> cyanMarkers = m_markers.value("cyan").toList();
    QList<double> magentaMarkers = m_markers.value("magenta").toList();
    QList<double> orangeMarkers = m_markers.value("orange").toList();

    if (showMarkers) {
        foreach(const double marker, markers)
        {
            markerList.append(marker);
        }
    }

    if (showSceneMarkers) {
        foreach(const double marker, sceneMarkers)
        {
            markerList.append(marker);
        }
    }

    if (showCumshotMarkers) {
        foreach(const double marker, cumshotMarkers)
        {
            markerList.append(marker);
        }
    }

    if (showStripMarkers) {
        foreach(const double marker, stripMarkers)
        {
            markerList.append(marker);
        }
    }

    if (showDialogMarkers) {
        foreach(const double marker, dialogMarkers)
        {
            markerList.append(marker);
        }
    }

    if (showCyanMarkers) {
        foreach(const double marker, cyanMarkers)
        {
            markerList.append(marker);
        }
    }

    if (showMagentaMarkers) {
        foreach(const double marker, magentaMarkers)
        {
            markerList.append(marker);
        }
    }

    if (showOrangeMarkers) {
        foreach(const double marker, orangeMarkers)
        {
            markerList.append(marker);
        }
    }

    foreach(const double marker, markerList) {
        if (marker < currentPercent - 0.001) {
            if (newVal != 0.0) {
                if (marker > newVal) {
                    newVal = marker;
                }
            } else {
                newVal = marker;
            }
        }
    }

    if (newVal != 0.0) {
        int x = newVal * (this->maximum() - this->minimum());
        emit markerSelected(x);
    }
}

int VideoSlider::minimum() const
{
    return m_minimum;
}

int VideoSlider::maximum() const
{
    return m_maximum;
}

int VideoSlider::value() const
{
    return m_value;
}

bool VideoSlider::isSliderDown() const
{
    return m_isSliderDown;
}

bool VideoSlider::isVideoLoaded() const
{
    return m_videoLoaded;
}

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

    QList<double> markerList;
    QList<double> markers = m_markers.value("marker").toList();
    QList<double> sceneMarkers = m_markers.value("scene").toList();
    QList<double> cumshotMarkers = m_markers.value("cumshot").toList();
    QList<double> stripMarkers = m_markers.value("strip").toList();
    QList<double> dialogMarkers = m_markers.value("dialog").toList();
    QList<double> cyanMarkers = m_markers.value("cyan").toList();
    QList<double> magentaMarkers = m_markers.value("magenta").toList();
    QList<double> orangeMarkers = m_markers.value("orange").toList();

    // Draw markers.
    if (showMarkers) {
        foreach(const double marker, markers)
        {
            if (marker > this->minimum() && marker < this->maximum()) {
                painter.setPen(QPen(Qt::green, m_markerWidth));
                int x = marker * this->width();
                painter.drawLine(x, 0, x, m_markerHeight);
            }
        }
    }

    // Draw scene markers.
    if (showSceneMarkers) {
        foreach(const double marker, sceneMarkers)
        {
            if (marker > this->minimum() && marker < this->maximum()) {
                painter.setPen(QPen(Qt::blue, m_markerWidth));
                int x = marker * this->width();
                painter.drawLine(x, 0, x, m_markerHeight);
            }
        }
    }

    // Draw cumshot markers.
    if (showCumshotMarkers) {
        foreach(const double marker, cumshotMarkers)
        {
            if (marker > this->minimum() && marker < this->maximum()) {
                painter.setPen(QPen(Qt::white, m_markerWidth));
                int x = marker * this->width();
                painter.drawLine(x, 0, x, m_markerHeight);
            }
        }
    }

    // Draw strip markers.
    if (showStripMarkers) {
        foreach(const double marker, stripMarkers)
        {
            if (marker > this->minimum() && marker < this->maximum()) {
                painter.setPen(QPen(Qt::red, m_markerWidth));
                int x = marker * this->width();
                painter.drawLine(x, 0, x, m_markerHeight);
            }
        }
    }

    // Draw dialog markers.
    if (showDialogMarkers) {
        foreach(const double marker, dialogMarkers)
        {
            if (marker > this->minimum() && marker < this->maximum()) {
                painter.setPen(QPen(Qt::yellow, m_markerWidth));
                int x = marker * this->width();
                painter.drawLine(x, 0, x, m_markerHeight);
            }
        }
    }

    // Draw cyan markers.
    if (showCyanMarkers) {
        foreach(const double marker, cyanMarkers)
        {
            if (marker > this->minimum() && marker < this->maximum()) {
                painter.setPen(QPen(Qt::cyan, m_markerWidth));
                int x = marker * this->width();
                painter.drawLine(x, 0, x, m_markerHeight);
            }
        }
    }

    // Draw magenta markers.
    if (showMagentaMarkers) {
        foreach(const double marker, magentaMarkers)
        {
            if (marker > this->minimum() && marker < this->maximum()) {
                painter.setPen(QPen(Qt::magenta, m_markerWidth));
                int x = marker * this->width();
                painter.drawLine(x, 0, x, m_markerHeight);
            }
        }
    }

    // Draw orange markers.
    if (showOrangeMarkers) {
        foreach(const double marker, orangeMarkers)
        {
            if (marker > this->minimum() && marker < this->maximum()) {
                painter.setPen(QPen(Qt::darkYellow, m_markerWidth));
                int x = marker * this->width();
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
