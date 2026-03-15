#include "videoslider.h"


VideoSlider::VideoSlider(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_Hover, true);
    this->setFixedHeight(25);

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
        painter.setPen(QPen(Qt::lightGray, 1));
        painter.setBrush(QBrush(Qt::lightGray, Qt::SolidPattern));
        painter.drawRect(0, (this->height() / 2) - 1, currentWidth, 5);
    } else {
        double distanceFromMin = (this->value() - this->minimum());
        double sliderRange = (this->maximum() - this->minimum());
        double sliderPercent = (distanceFromMin / sliderRange);

        int x = sliderPercent * currentWidth;

        painter.setPen(QPen(Qt::blue, 1));
        painter.setBrush(QBrush(Qt::blue, Qt::SolidPattern));
        painter.drawRect(0, (this->height() / 2) - 1, x, 5);

        painter.setPen(QPen(Qt::lightGray, 1));
        painter.setBrush(QBrush(Qt::lightGray, Qt::SolidPattern));
        painter.drawRect(x, (this->height() / 2) - 1, currentWidth, 5);

        if (m_showingIndicator) {
            painter.setPen(QPen(Qt::lightGray, 1));
            painter.setBrush(QBrush(Qt::lightGray, Qt::SolidPattern));
            QPoint center(x, (this->height() / 2) + 2);
            int radius = 6;
            painter.drawEllipse(center, radius, radius);
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

    if (showMarkers) {
        foreach(const double marker, markers)
        {
            if (marker > this->minimum() && marker < this->maximum()) {
                painter.setPen(QPen(Qt::green, 2));
                int x = marker * this->width();
                painter.drawLine(x, 0, x, height());
            }
        }
    }

    if (showSceneMarkers) {
        foreach(const double marker, sceneMarkers)
        {
            if (marker > this->minimum() && marker < this->maximum()) {
                painter.setPen(QPen(Qt::blue, 2));
                int x = marker * this->width();
                painter.drawLine(x, 0, x, height());
            }
        }
    }

    if (showCumshotMarkers) {
        foreach(const double marker, cumshotMarkers)
        {
            if (marker > this->minimum() && marker < this->maximum()) {
                painter.setPen(QPen(Qt::white, 2));
                int x = marker * this->width();
                painter.drawLine(x, 0, x, height());
            }
        }
    }

    if (showStripMarkers) {
        foreach(const double marker, stripMarkers)
        {
            if (marker > this->minimum() && marker < this->maximum()) {
                painter.setPen(QPen(Qt::red, 2));
                int x = marker * this->width();
                painter.drawLine(x, 0, x, height());
            }
        }
    }

    if (showDialogMarkers) {
        foreach(const double marker, dialogMarkers)
        {
            if (marker > this->minimum() && marker < this->maximum()) {
                painter.setPen(QPen(Qt::yellow, 2));
                int x = marker * this->width();
                painter.drawLine(x, 0, x, height());
            }
        }
    }

    if (showCyanMarkers) {
        foreach(const double marker, cyanMarkers)
        {
            if (marker > this->minimum() && marker < this->maximum()) {
                painter.setPen(QPen(Qt::cyan, 2));
                int x = marker * this->width();
                painter.drawLine(x, 0, x, height());
            }
        }
    }

    if (showMagentaMarkers) {
        foreach(const double marker, magentaMarkers)
        {
            if (marker > this->minimum() && marker < this->maximum()) {
                painter.setPen(QPen(Qt::magenta, 2));
                int x = marker * this->width();
                painter.drawLine(x, 0, x, height());
            }
        }
    }

    if (showOrangeMarkers) {
        foreach(const double marker, orangeMarkers)
        {
            if (marker > this->minimum() && marker < this->maximum()) {
                painter.setPen(QPen(Qt::darkYellow, 2));
                int x = marker * this->width();
                painter.drawLine(x, 0, x, height());
            }
        }
    }
}

void VideoSlider::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_isSliderDown = true;

        double posRatio = (double)event->x() / this->width();
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
