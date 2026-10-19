#pragma once

#include <QColor>
#include <QList>
#include <QString>

/// Single source of truth for marker type -> colour. The VideoSlider should
/// use this too, so the chips, list rows and slider ticks always agree.
namespace MarkerColors {

struct TypeColor
{
    QString type;
    QString label;
    QColor color;
};

inline const QList<TypeColor> &all()
{
    static const QList<TypeColor> types = {
        {QStringLiteral("marker"),  QStringLiteral("Marker"),  QColor(0x71, 0x8b, 0x3a)},
        {QStringLiteral("strip"),   QStringLiteral("Strip"),   QColor(0xd8, 0x2d, 0x3a)},
        {QStringLiteral("magenta"), QStringLiteral("Magenta"), QColor(0xb4, 0x8c, 0xbd)},
        {QStringLiteral("orange"),  QStringLiteral("Orange"),  QColor(0xe8, 0x76, 0x2b)},
        {QStringLiteral("dialog"),  QStringLiteral("Dialog"),  QColor(0xd4, 0xa5, 0x2a)},
        {QStringLiteral("cumshot"), QStringLiteral("Cumshot"), QColor(0xf2, 0xf2, 0xf2)},
        {QStringLiteral("scene"),   QStringLiteral("Scene"),   QColor(0x4a, 0x8e, 0xf0)},
        {QStringLiteral("cyan"),    QStringLiteral("Cyan"),    QColor(0x1e, 0xe0, 0xc8)},
    };
    return types;
}

inline QColor forType(const QString &type)
{
    for (const TypeColor &t : all())
        if (t.type == type)
            return t.color;
    return QColor(0x80, 0x80, 0x80);
}

} // namespace MarkerColors
