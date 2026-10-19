#pragma once

#include <QColor>
#include <QIcon>
#include <QPixmap>

// All chrome is drawn with QPainter instead of shipping PNG/SVG assets, so the
// icons stay crisp at any DPI and can be recoloured from the palette.
namespace Icons {

QIcon play(const QColor &color);
QIcon pause(const QColor &color);
QIcon previousTrack(const QColor &color);
QIcon nextTrack(const QColor &color);
QIcon stepBack(const QColor &color);
QIcon settings(const QColor &color);
QIcon expand(const QColor &color);
QIcon more(const QColor &color);
QIcon appMark();

// Stand-in artwork for playlist rows until real frame grabs are available.
QPixmap placeholderThumbnail(const QColor &top, const QColor &bottom, int seed);

} // namespace Icons
