#pragma once
#include <QString>


struct VideoMarkerRecord
{
    int id = 0;
    QString fileName;
    QString markerName;
    QString markerType;
    QString comments;
    double timestampMs = 0.0;
};
