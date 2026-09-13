#pragma once

#include <QString>


struct TrackInfo
{
    QString id;
    QString title;
    QString language;
    QString codec;
    bool isDefault = false;
};
