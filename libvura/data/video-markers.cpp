/*******************************************************************************
     Copyright (c) 2026. by Andrew Hale <halea2196@gmail.com>

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

#include "video-markers.h"


VideoMarkers::VideoMarkers(QObject *parent) : QObject(parent) {}

bool VideoMarkers::createBlankFile(const QString &fileName)
{
    const QJsonObject blankObj;
    const QJsonDocument doc(blankObj);
    const QByteArray data = doc.toJson();

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        file.write(data);
        file.close();
        return true;

    } else {
        qCritical() << "Error saving video markers file: " << fileName << ". Error: " << file.errorString();
    }
    return false;
}

bool VideoMarkers::write(const QString &fileName, const QString &videoFileName, const QList<VuraVideoMarker> &markers)
{
    QList<VuraVideoMarker> markerList;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Could not open file:" << fileName;
        return false;
    }

    const QByteArray rawData = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(rawData);
    if (doc.isNull()) {
        qWarning() << "Failed to create JSON doc.";
        return false;
    }

    if (doc.isArray()) {
        QJsonArray jsonArray = doc.array();

        for (const QJsonValue &value : jsonArray) {
            QJsonObject obj = value.toObject();
            VuraVideoMarker videoMarker;
            videoMarker.read(obj);

            if (videoMarker.fileName != videoFileName)
                markerList.append(videoMarker);
        }
    }

    for (const VuraVideoMarker &marker : markers) {
        markerList.append(marker);
    }

    QJsonArray markerArray;

    for (const auto &item : markerList) {
        markerArray.append(item.toJson());
    }

    QJsonDocument saveDoc(markerArray);

    QFile saveFile(fileName);
    if (saveFile.open(QIODevice::WriteOnly)) {
        saveFile.write(saveDoc.toJson());
        saveFile.close();
        return true;
    }
    return false;
}

QList<VuraVideoMarker> VideoMarkers::read(const QString &fileName, const QString &videoFileName)
{
    QList<VuraVideoMarker> markerList;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Could not open file:" << fileName;
        return markerList;
    }

    QByteArray rawData = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(rawData);
    if (doc.isNull()) {
        qWarning() << "Failed to create JSON doc.";
        return markerList;
    }

    if (doc.isArray()) {
        QJsonArray jsonArray = doc.array();

        for (const QJsonValue &value : jsonArray) {
            QJsonObject obj = value.toObject();
            VuraVideoMarker videoMarker;
            videoMarker.read(obj);

            if (videoMarker.fileName == videoFileName)
                markerList.append(videoMarker);
        }
    }

    return markerList;
}

QString VideoMarkers::generateMarkerID(const QString &fileName)
{
    QString markerID;
    QList<VuraVideoMarker> markerList;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Could not open file:" << fileName;
        return markerID;
    }

    QByteArray rawData = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(rawData);
    if (doc.isNull()) {
        qWarning() << "Failed to create JSON doc.";
        return markerID;
    }

    if (doc.isArray()) {
        QJsonArray jsonArray = doc.array();

        for (const QJsonValue &value : jsonArray) {
            QJsonObject obj = value.toObject();
            VuraVideoMarker videoMarker;
            videoMarker.read(obj);

            markerList.append(videoMarker);
        }
    }

    bool createdUniqueID = false;

    while (!createdUniqueID) {
        markerID = generateRandomString(64);
        bool existing = false;

        for (const VuraVideoMarker &marker : markerList) {
            if (marker.id == markerID) {
                existing = true;
            }
        }

        if (!existing) {
            createdUniqueID = true;
        }
    }

    return markerID;
}

QString VideoMarkers::generateRandomString(const int length)
{
    const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
    QString randomString;
    randomString.reserve(length);

    for(int i = 0; i < length; ++i) {
        const int index = QRandomGenerator::global()->bounded(possibleCharacters.length());
        randomString.append(possibleCharacters.at(index));
    }
    return randomString;
}
