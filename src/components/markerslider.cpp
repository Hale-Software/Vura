#include "markerslider.h"


MarkerSlider::MarkerSlider(QWidget *parent) : QSlider(Qt::Horizontal, parent) {}

void MarkerSlider::setMarkers(QMap<QString,QList<double>> markers)
{
    m_markers = markers;
    update();
}

void MarkerSlider::jumpToNextMarker(double currentPercent)
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
        //setValue(newVal);
        int x = newVal * (this->maximum() - this->minimum());
        emit markerSelected(x);
    }
}

void MarkerSlider::jumpToPreviousMarker(double currentPercent)
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
        //setValue(newVal);
        int x = newVal * (this->maximum() - this->minimum());
        emit markerSelected(x);
    }
}

void MarkerSlider::paintEvent(QPaintEvent *event)
{
    // 1. Draw the standard slider first
    QSlider::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(Qt::green, 2)); // Style your marker (e.g., yellow, 2px wide)

    // 2. Calculate marker positions based on the current style
    QStyleOptionSlider opt;
    initStyleOption(&opt);

    // Use the available space where the handle actually moves
    int availableLength = style()->pixelMetric(QStyle::PM_SliderSpaceAvailable, &opt, this);
    int offset = (width() - availableLength) / 2;

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
            if (marker > this->minimum() && marker < this->maximum()) {
                painter.setPen(QPen(Qt::green, 2));
                int x = offset + (marker * availableLength);
                painter.drawLine(x, 0, x, height());
            }
        }
    }

    if (showSceneMarkers) {
        foreach(const double marker, sceneMarkers)
        {
            if (marker > this->minimum() && marker < this->maximum()) {
                painter.setPen(QPen(Qt::blue, 2));
                int x = offset + (marker * availableLength);
                painter.drawLine(x, 0, x, height());
            }
        }
    }

    if (showCumshotMarkers) {
        foreach(const double marker, cumshotMarkers)
        {
            if (marker > this->minimum() && marker < this->maximum()) {
                painter.setPen(QPen(Qt::white, 2));
                int x = offset + (marker * availableLength);
                painter.drawLine(x, 0, x, height());
            }
        }
    }

    if (showStripMarkers) {
        foreach(const double marker, stripMarkers)
        {
            if (marker > this->minimum() && marker < this->maximum()) {
                painter.setPen(QPen(Qt::red, 2));
                int x = offset + (marker * availableLength);
                painter.drawLine(x, 0, x, height());
            }
        }
    }

    if (showDialogMarkers) {
        foreach(const double marker, dialogMarkers)
        {
            if (marker > this->minimum() && marker < this->maximum()) {
                painter.setPen(QPen(Qt::yellow, 2));
                int x = offset + (marker * availableLength);
                painter.drawLine(x, 0, x, height());
            }
        }
    }

    if (showCyanMarkers) {
        foreach(const double marker, cyanMarkers)
        {
            if (marker > this->minimum() && marker < this->maximum()) {
                painter.setPen(QPen(Qt::cyan, 2));
                int x = offset + (marker * availableLength);
                painter.drawLine(x, 0, x, height());
            }
        }
    }

    if (showMagentaMarkers) {
        foreach(const double marker, magentaMarkers)
        {
            if (marker > this->minimum() && marker < this->maximum()) {
                painter.setPen(QPen(Qt::magenta, 2));
                int x = offset + (marker * availableLength);
                painter.drawLine(x, 0, x, height());
            }
        }
    }

    if (showOrangeMarkers) {
        foreach(const double marker, orangeMarkers)
        {
            if (marker > this->minimum() && marker < this->maximum()) {
                painter.setPen(QPen(Qt::darkYellow, 2));
                int x = offset + (marker * availableLength);
                painter.drawLine(x, 0, x, height());
            }
        }
    }
}

void MarkerSlider::mousePressEvent(QMouseEvent *event)
{
    QSlider::mousePressEvent(event);

    if (event->button() == Qt::LeftButton) {
        QStyleOptionSlider opt;
        initStyleOption(&opt);
        QRect groove = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderGroove, this);

        int value = minimum();
        if (orientation() == Qt::Horizontal) {
            int sliderLength = size().width();
            // Ensure calculation uses the full width of the widget
            double posRatio = (double)event->x() / sliderLength;
            value = minimum() + posRatio * (maximum() - minimum());
        } else { // Qt::Vertical
            int sliderLength = size().height();
            // For vertical sliders, position is typically inverted (top is max, bottom is min by default in some styles,
            // but Qt usually assumes bottom is min, top is max).
            // Need to adjust calculation if it's inverted. Here's a standard approach:
            double posRatio = (double)event->y() / sliderLength;
            // For vertical, the value increases from bottom to top, so adjust the ratio
            value = maximum() - posRatio * (maximum() - minimum());
        }

        // Set the new value, which will automatically emit valueChanged()
        //setValue(std::round(value));
        emit sliderClicked(std::round(value));
        event->accept(); // Mark the event as handled
    }
}
