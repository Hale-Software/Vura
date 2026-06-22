#pragma once
#include <QString>

struct MediaItem {
    QString title;
    QString filePath;
    qint64 duration; // in milliseconds
    qint64 progress; // current watch position
    bool isWatched;
};
