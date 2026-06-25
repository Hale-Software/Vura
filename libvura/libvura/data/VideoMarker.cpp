#include "VideoMarker.h"

bool VideoMarkers::saveMarkersToFile(const QString &fileName, const QList<VideoMarker> &markers)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Could not open file:" << fileName;
        return false;
    }

    QDataStream stream(&file);
    stream << (quint32)0x564D524B;
    stream << (qint32)1;
    stream << markers;
    file.close();
    return true;
}

QList<VideoMarker> VideoMarkers::readMarkersFromFile(const QString &fileName)
{
    QList<VideoMarker> markers;
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open file:" << fileName;
        return markers;
    }

    QDataStream stream(&file);
    quint32 signature;
    stream >> signature;
    if (signature != 0x564D524B) {
        qWarning() << "Invalid signature:" << signature;
        return markers;
    }

    qint32 version;
    stream >> version;
    if (version == 1) {
        stream >> markers;
    } else {
        qWarning() << "Unsupported version:" << version;
    }
    file.close();
    return markers;
}
