#include "Icons.h"

#include <QLinearGradient>
#include <QPainter>
#include <QPainterPath>
#include <QRandomGenerator>
#include <QTransform>

namespace {

// Every icon is authored inside a 24x24 box and scaled from there.
constexpr qreal kBox = 24.0;

QPixmap renderPath(const QPainterPath &path, const QColor &color, int px)
{
    QPixmap pm(px, px);
    pm.setDevicePixelRatio(1.0);
    pm.fill(Qt::transparent);

    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.scale(px / kBox, px / kBox);
    p.fillPath(path, color);
    return pm;
}

QIcon toIcon(const QPainterPath &path, const QColor &color)
{
    QIcon icon;
    for (int px : {16, 24, 32, 48, 64})
        icon.addPixmap(renderPath(path, color, px));
    return icon;
}

// Turns a polyline into a filled outline so stroked icons scale like the
// solid ones and never get hairline-thin on high-DPI screens.
QPainterPath strokeToPath(const QPainterPath &line, qreal width)
{
    QPainterPathStroker stroker;
    stroker.setWidth(width);
    stroker.setCapStyle(Qt::RoundCap);
    stroker.setJoinStyle(Qt::RoundJoin);
    return stroker.createStroke(line);
}

} // namespace

namespace Icons {

QIcon play(const QColor &color)
{
    QPainterPath p;
    p.moveTo(8.0, 5.2);
    p.lineTo(19.0, 12.0);
    p.lineTo(8.0, 18.8);
    p.closeSubpath();
    return toIcon(p, color);
}

QIcon pause(const QColor &color)
{
    QPainterPath p;
    p.addRoundedRect(QRectF(7.0, 5.5, 3.6, 13.0), 1.4, 1.4);
    p.addRoundedRect(QRectF(13.4, 5.5, 3.6, 13.0), 1.4, 1.4);
    return toIcon(p, color);
}

QIcon previousTrack(const QColor &color)
{
    QPainterPath p;
    p.addRoundedRect(QRectF(6.0, 6.0, 2.6, 12.0), 1.2, 1.2);
    p.moveTo(18.6, 6.0);
    p.lineTo(18.6, 18.0);
    p.lineTo(10.0, 12.0);
    p.closeSubpath();
    return toIcon(p, color);
}

QIcon nextTrack(const QColor &color)
{
    QPainterPath p;
    p.moveTo(5.4, 6.0);
    p.lineTo(14.0, 12.0);
    p.lineTo(5.4, 18.0);
    p.closeSubpath();
    p.addRoundedRect(QRectF(15.4, 6.0, 2.6, 12.0), 1.2, 1.2);
    return toIcon(p, color);
}

QIcon stepBack(const QColor &color)
{
    QPainterPath line;
    line.moveTo(15.0, 5.5);
    line.lineTo(8.5, 12.0);
    line.lineTo(15.0, 18.5);
    return toIcon(strokeToPath(line, 2.1), color);
}

QIcon settings(const QColor &color)
{
    QPainterPath gear;
    for (int i = 0; i < 6; ++i) {
        QPainterPath tooth;
        tooth.addRoundedRect(QRectF(-2.6, -11.0, 5.2, 6.0), 1.5, 1.5);
        QTransform t;
        t.translate(12.0, 12.0);
        t.rotate(i * 60.0);
        gear.addPath(t.map(tooth));
    }
    gear.addEllipse(QPointF(12.0, 12.0), 7.0, 7.0);
    gear.setFillRule(Qt::WindingFill);

    QPainterPath hub;
    hub.addEllipse(QPointF(12.0, 12.0), 2.9, 2.9);
    return toIcon(gear.simplified().subtracted(hub), color);
}

QIcon expand(const QColor &color)
{
    QPainterPath line;
    line.moveTo(14.0, 4.5);
    line.lineTo(19.5, 4.5);
    line.lineTo(19.5, 10.0);
    line.moveTo(19.5, 4.5);
    line.lineTo(13.2, 10.8);
    line.moveTo(10.0, 19.5);
    line.lineTo(4.5, 19.5);
    line.lineTo(4.5, 14.0);
    line.moveTo(4.5, 19.5);
    line.lineTo(10.8, 13.2);
    return toIcon(strokeToPath(line, 2.0), color);
}

QIcon more(const QColor &color)
{
    QPainterPath p;
    for (qreal x : {5.5, 12.0, 18.5})
        p.addEllipse(QPointF(x, 12.0), 1.7, 1.7);
    return toIcon(p, color);
}

QIcon appMark()
{
    QPixmap pm(64, 64);
    pm.fill(Qt::transparent);

    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing, true);

    QLinearGradient g(0, 0, 64, 64);
    g.setColorAt(0.0, QColor("#a78bfa"));
    g.setColorAt(1.0, QColor("#6d28d9"));

    QPainterPath badge;
    badge.addRoundedRect(QRectF(4, 4, 56, 56), 16, 16);
    p.fillPath(badge, g);

    QPainterPath mark;
    mark.moveTo(25.0, 19.0);
    mark.lineTo(46.0, 32.0);
    mark.lineTo(25.0, 45.0);
    mark.closeSubpath();
    p.fillPath(mark, QColor("#f5f3ff"));

    return QIcon(pm);
}

QPixmap placeholderThumbnail(const QColor &top, const QColor &bottom, int seed)
{
    constexpr int w = 200;
    constexpr int h = 112;

    QPixmap pm(w, h);
    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing, true);

    QLinearGradient sky(0, 0, 0, h);
    sky.setColorAt(0.0, top);
    sky.setColorAt(1.0, bottom);
    p.fillRect(QRect(0, 0, w, h), sky);

    // A few silhouettes so the rows read as distinct frames rather than
    // flat swatches. Seeded, so a given track always looks the same.
    QRandomGenerator rng(static_cast<quint32>(seed));
    p.setPen(Qt::NoPen);
    for (int i = 0; i < 9; ++i) {
        const int bw = 14 + rng.bounded(20);
        const int bh = 26 + rng.bounded(58);
        const int bx = rng.bounded(w);
        p.setBrush(QColor(8, 6, 18, 120 + rng.bounded(90)));
        p.drawRect(bx, h - bh, bw, bh);
    }

    p.setBrush(QColor(255, 255, 255, 40));
    p.drawEllipse(QPointF(w * 0.72, h * 0.3), 16, 16);

    return pm;
}

}
