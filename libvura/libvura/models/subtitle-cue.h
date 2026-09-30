#pragma once

#include <QStringList>


struct SubtitleCue {
    qint64 start = 0;   // ms
    qint64 end   = 0;   // ms
    QStringList lines;
};
