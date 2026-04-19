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

#pragma once

#include <QObject>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QDebug>
#include <QString>
#include <QList>
#include <QSettings>
#include <QStandardPaths>
#include <QRandomGenerator>


struct VuraVideoMarker
{
    QString id;
    QString fileName;
    QString markerName;
    QString markerType;
    double timestamp;

    void read(const QJsonObject &json)
    {
        if (json.contains("id") && json["id"].isString())
            id = json["id"].toString();

        if (json.contains("fileName") && json["fileName"].isString())
            fileName = json["fileName"].toString();

        if (json.contains("markerName") && json["markerName"].isString())
            markerName = json["markerName"].toString();

        if (json.contains("markerType") && json["markerType"].isString())
            markerType = json["markerType"].toString();

        if (json.contains("timestamp") && json["timestamp"].isDouble())
            timestamp = json["timestamp"].toDouble();
    }

    QJsonObject toJson() const
    {
        QJsonObject obj;
        obj["id"] = id;
        obj["fileName"] = fileName;
        obj["markerName"] = markerName;
        obj["markerType"] = markerType;
        obj["timestamp"] = timestamp;
        return obj;
    }
};


class VideoMarkers : public QObject
{
    Q_OBJECT
public:
    explicit VideoMarkers(QObject *parent = nullptr);

    static bool createBlankFile(const QString &fileName);
    static bool write(const QString &fileName, const QString &videoFileName, const QList<VuraVideoMarker> &markers);
    static QList<VuraVideoMarker> read(const QString &fileName, const QString &videoFileName);
    static QString generateMarkerID(const QString &fileName);

private:
    static QString generateRandomString(int length);

};
