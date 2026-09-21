#pragma once

#include <QString>
#include <qtypes.h>


struct FileEntry
{
    QString name;
    QString hash;
    QString downloadedFilePath = QString();
    qint64 size = 0;
};
