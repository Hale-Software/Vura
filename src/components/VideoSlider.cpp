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

#include <QSettings>
#include <QPainter>
#include <QPaintEvent>
#include <QContextMenuEvent>
#include <QHelpEvent>
#include <QColor>
#include <QPen>
#include <QBrush>
#include <QString>
#include <QSignalBlocker>
#include <QRect>
#include <QMenu>
#include <QToolTip>
#include <QTime>
#include <QHash>
#include <QDebug>

#include <algorithm>
#include <cmath>


namespace {

/// Marker colours keyed by marker type, read from settings once per paint.
QHash<QString, QColor> markerColors(const QSettings &settings)
{
    return {
        {QStringLiteral("marker"),  QColor(settings.value("markerColor", "#03c200").toString())},
        {QStringLiteral("scene"),   QColor(settings.value("sceneMarkerColor", "#000eab").toString())},
        {QStringLiteral("cumshot"), QColor(settings.value("cumshotMarkerColor", "#ffffff").toString())},
        {QStringLiteral("strip"),   QColor(settings.value("stripMarkerColor", "#cf0202").toString())},
        {QStringLiteral("dialog"),  QColor(settings.value("dialogMarkerColor", "#e0f500").toString())},
        {QStringLiteral("cyan"),    QColor(settings.value("cyanMarkerColor", "#00edf5").toString())},
        {QStringLiteral("magenta"), QColor(settings.value("magentaMarkerColor", "#f5007e").toString())},
        {QStringLiteral("orange"),  QColor(settings.value("orangeMarkerColor", "#f56a00").toString())},
    };
}

QString formatTimestamp(const qint64 ms)
{
    const qint64 clamped = qMax<qint64>(0, ms);
    const QTime time = QTime::fromMSecsSinceStartOfDay(int(clamped % 86400000));
    return time.hour() > 0 ? time.toString(QStringLiteral("h:mm:ss"))
                           : time.toString(QStringLiteral("m:ss"));
}

} // namespace


VideoSlider::VideoSlider(QWidget *parent)
    : QWidget(parent),
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
    m_showingStripMarkers(true)
{
    setMouseTracking(true);
    this->setFixedHeight(20);
}

QSize VideoSlider::minimumSizeHint() const
{
    const QSettings settings;
    const int videoMarkerSideLength = settings.value("videoMarkerSideLength", 14).toInt();
    const int sliderLeftRightMargin = settings.value("sliderLeftRightMargin", 1).toInt();
    return {videoMarkerSideLength * 2 + sliderLeftRightMargin * 2, videoMarkerSideLength};
}

void VideoSlider::setPositionFromEngine(const qint64 ms)
{
    if (m_scrubbing)
        return;

    QSignalBlocker blocker(this);
    const qint64 lo = minimum();
    const qint64 hi = maximum();
    setValue(int(qBound(lo, ms, hi)));
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

    m_hoveredMarker = -1;
    unsetCursor();
    update();
}

void VideoSlider::loadVideoMarkers(QList<VideoMarkerRecord> markers)
{
    m_videoMarkers = std::move(markers);
    m_hoveredMarker = -1;
    update();
}

void VideoSlider::updateVideoSlider(QList<VideoMarkerRecord> markers)
{
    m_videoMarkers = std::move(markers);
    m_hoveredMarker = -1;
    update();
}

void VideoSlider::setValue(const int val)
{
    m_value = qBound(m_minimum, val, m_maximum);

    if (m_scrubbing)
        emit scrubbed(m_value);

    const double sliderRange = double(m_maximum) - double(m_minimum);
    m_sliderPercent = sliderRange > 0.0
        ? std::clamp(float((double(m_value) - double(m_minimum)) / sliderRange), 0.0f, 1.0f)
        : 0.0f;

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

void VideoSlider::setRange(const int minimum, const int maximum)
{
    if (maximum < minimum) {
        qWarning() << "Failed to set video slider range: " << QString::number(minimum) << " - " << QString::number(maximum);
        return;
    }

    m_minimum = minimum;
    m_maximum = maximum;
    m_value = qBound(m_minimum, m_value, m_maximum);
    update();
}

void VideoSlider::goToNextMarker(const double currentPercent)
{
    const VideoMarkerRecord *next = nullptr;

    for (const VideoMarkerRecord &marker : m_videoMarkers) {
        if (!getMarkerTypesVisible(marker.markerType)) continue;
        if (marker.timestampMs <= currentPercent + 0.001) continue;
        if (!next || marker.timestampMs < next->timestampMs)
            next = &marker;
    }

    if (next)
        seekToMarker(*next);
}

void VideoSlider::goToPreviousMarker(const double currentPercent)
{
    const VideoMarkerRecord *previous = nullptr;

    for (const VideoMarkerRecord &marker : m_videoMarkers) {
        if (!getMarkerTypesVisible(marker.markerType)) continue;
        if (marker.timestampMs >= currentPercent - 0.001) continue;
        if (!previous || marker.timestampMs > previous->timestampMs)
            previous = &marker;
    }

    if (previous)
        seekToMarker(*previous);
}

// Geometry ////////////////////////////////////////////////////////////////////

int VideoSlider::trackPadding() const
{
    const QSettings settings;
    return settings.value("sliderHandleRadius", 8).toInt()
         + settings.value("sliderPadding", 2).toInt();
}

int VideoSlider::trackWidth() const
{
    return qMax(0, width() - trackPadding() * 2);
}

int VideoSlider::xForPercent(const double percent) const
{
    return trackPadding() + qRound(std::clamp(percent, 0.0, 1.0) * trackWidth());
}

double VideoSlider::percentForX(const int x) const
{
    const int track = trackWidth();
    if (track <= 0)
        return 0.0;
    return std::clamp(double(x - trackPadding()) / double(track), 0.0, 1.0);
}

int VideoSlider::valueForPosition(const int x) const
{
    if (m_maximum <= m_minimum)
        return m_minimum;
    return m_minimum + qRound(percentForX(x) * (double(m_maximum) - double(m_minimum)));
}

qint64 VideoSlider::msForMarker(const VideoMarkerRecord &marker) const
{
    // timestampMs is stored as a 0..1 fraction of the duration, not milliseconds.
    return qint64(m_minimum)
         + qRound64(std::clamp(marker.timestampMs, 0.0, 1.0) * (double(m_maximum) - double(m_minimum)));
}

int VideoSlider::markerIndexAt(const QPoint &pos) const
{
    if (m_videoMarkers.isEmpty() || m_maximum <= m_minimum)
        return -1;

    const QSettings settings;
    const int grab = settings.value("markerGrabTolerance", 5).toInt();
    const int markerHeight = settings.value("videoMarkerHeight", 10).toInt();
    const int verticalMargin = settings.value("videoMarkerVerticalMargin", 0).toInt();

    if (pos.y() < verticalMargin - grab || pos.y() > markerHeight + grab)
        return -1;

    int bestIndex = -1;
    int bestDistance = grab + 1;

    for (int i = 0; i < m_videoMarkers.size(); ++i) {
        const VideoMarkerRecord &marker = m_videoMarkers.at(i);
        if (!getMarkerTypesVisible(marker.markerType))
            continue;

        const int distance = std::abs(xForPercent(marker.timestampMs) - pos.x());
        if (distance <= grab && distance < bestDistance) {
            bestDistance = distance;
            bestIndex = i;
        }
    }

    return bestIndex;
}

QString VideoSlider::markerToolTip(const VideoMarkerRecord &marker) const
{
    const QString name = marker.markerName.isEmpty() ? marker.markerType : marker.markerName;

    QString text = QStringLiteral("<b>%1</b> &nbsp; %2")
                       .arg(name.toHtmlEscaped(), formatTimestamp(msForMarker(marker)));

    if (!marker.comments.isEmpty())
        text += QStringLiteral("<br>%1").arg(marker.comments.toHtmlEscaped());

    return text;
}

void VideoSlider::seekToMarker(const VideoMarkerRecord &marker)
{
    const bool wasScrubbing = m_scrubbing;
    m_scrubbing = false;              // keep setValue from emitting scrubbed too
    setValue(int(msForMarker(marker)));
    m_scrubbing = wasScrubbing;
    emit scrubFinished(m_value);
}

// Painting ////////////////////////////////////////////////////////////////////

void VideoSlider::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    const QSettings settings;
    const int sliderHeight = settings.value("sliderHeight", 6).toInt();
    const int videoMarkerHeight = settings.value("videoMarkerHeight", 10).toInt();
    const int videoMarkerWidth = settings.value("videoMarkerWidth", 2).toInt();
    const int videoMarkerVerticalMargin = settings.value("videoMarkerVerticalMargin", 0).toInt();
    const int handleRadius = settings.value("sliderHandleRadius", 8).toInt();

    const QString backgroundTrackColor = settings.value("backgroundTrackColor", "#323232").toString();
    const QString progressFillColor = settings.value("progressFillColor", "#0076d7").toString();
    const QString handlePlayheadColor = settings.value("handlePlayheadColor", "#ffffff").toString();
    const QHash<QString, QColor> colors = markerColors(settings);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const int padding = trackPadding();
    const int track = trackWidth();
    const int yCenter = height() / 2;

    // Draw the Background Track
    const QRectF bgRect(padding, yCenter - (sliderHeight / 2.0), track, sliderHeight);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(backgroundTrackColor));
    painter.drawRoundedRect(bgRect, sliderHeight / 2.0, sliderHeight / 2.0);

    // Draw the Progress Fill
    const float fillWidth = track * m_sliderPercent;
    const QRectF fillRect(padding, yCenter - (sliderHeight / 2.0), fillWidth, sliderHeight);
    painter.setBrush(QColor(progressFillColor));
    painter.drawRoundedRect(fillRect, sliderHeight / 2.0, sliderHeight / 2.0);

    // Draw the Handle / Playhead
    const float handleX = padding + fillWidth;
    painter.setBrush(QColor(handlePlayheadColor));
    painter.drawEllipse(QPointF(handleX, yCenter), handleRadius, handleRadius);

    // Draw Markers
    for (int i = 0; i < m_videoMarkers.size(); ++i) {
        const VideoMarkerRecord &marker = m_videoMarkers.at(i);
        if (!getMarkerTypesVisible(marker.markerType))
            continue;
        if (marker.timestampMs < 0.0 || marker.timestampMs > 1.0)
            continue;

        const QColor color = colors.value(marker.markerType, QColor(Qt::white));
        const bool hovered = (i == m_hoveredMarker);

        painter.setPen(QPen(hovered ? color.lighter(150) : color,
                            hovered ? videoMarkerWidth + 2 : videoMarkerWidth));

        const int x = xForPercent(marker.timestampMs);
        painter.drawLine(x, videoMarkerVerticalMargin, x, videoMarkerHeight);
    }
}

// Interaction /////////////////////////////////////////////////////////////////

void VideoSlider::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton || m_maximum <= m_minimum) {
        QWidget::mousePressEvent(event);
        return;
    }

    const int index = markerIndexAt(event->position().toPoint());
    if (index >= 0) {
        const VideoMarkerRecord marker = m_videoMarkers.at(index);
        seekToMarker(marker);
        emit markerActivated(marker);
        event->accept();
        return;
    }

    m_scrubbing = true;
    setValue(valueForPosition(int(event->position().x())));   // emits scrubbed
    event->accept();
}

void VideoSlider::mouseMoveEvent(QMouseEvent *event)
{
    if (m_scrubbing) {
        setValue(valueForPosition(int(event->position().x())));   // emits scrubbed
        event->accept();
        return;
    }

    const int index = markerIndexAt(event->position().toPoint());
    if (index != m_hoveredMarker) {
        m_hoveredMarker = index;
        if (index >= 0)
            setCursor(Qt::PointingHandCursor);
        else
            unsetCursor();
        update();
    }

    QWidget::mouseMoveEvent(event);
}

void VideoSlider::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton || !m_scrubbing) {
        QWidget::mouseReleaseEvent(event);
        return;
    }

    const int target = valueForPosition(int(event->position().x()));
    m_scrubbing = false;      // clear first so setValue doesn't emit scrubbed as well
    setValue(target);
    emit scrubFinished(m_value);
    event->accept();
}

void VideoSlider::contextMenuEvent(QContextMenuEvent *event)
{
    const int index = markerIndexAt(event->pos());
    if (index < 0) {
        QWidget::contextMenuEvent(event);
        return;
    }

    const VideoMarkerRecord marker = m_videoMarkers.at(index);

    // Keep it highlighted while the menu is open.
    m_hoveredMarker = index;
    update();

    QMenu menu(this);
    const QAction *goTo = menu.addAction(tr("Go to Marker"));
    const QAction *edit = menu.addAction(tr("Edit Marker..."));
    menu.addSeparator();
    const QAction *remove = menu.addAction(tr("Delete Marker"));
    menu.addSeparator();
    const QAction *hideType = menu.addAction(tr("Hide \"%1\" Markers").arg(marker.markerType));

    const QAction *chosen = menu.exec(event->globalPos());
    event->accept();

    if (!chosen)
        return;

    if (chosen == goTo) {
        seekToMarker(marker);
        emit markerActivated(marker);
    } else if (chosen == edit) {
        emit markerEditRequested(marker);
    } else if (chosen == remove) {
        emit markerDeleteRequested(marker);
    } else if (chosen == hideType) {
        setMarkerTypeVisible(marker.markerType, false);
        emit markerTypeHidden(marker.markerType);
    }
}

void VideoSlider::leaveEvent(QEvent *event)
{
    if (m_hoveredMarker != -1) {
        m_hoveredMarker = -1;
        unsetCursor();
        update();
    }
    QWidget::leaveEvent(event);
}

bool VideoSlider::event(QEvent *event)
{
    if (event->type() == QEvent::ToolTip) {
        const auto *helpEvent = static_cast<QHelpEvent *>(event);
        const int index = markerIndexAt(helpEvent->pos());

        if (index >= 0) {
            QToolTip::showText(helpEvent->globalPos(), markerToolTip(m_videoMarkers.at(index)), this);
        } else {
            QToolTip::hideText();
            event->ignore();
        }
        return true;
    }

    return QWidget::event(event);
}
