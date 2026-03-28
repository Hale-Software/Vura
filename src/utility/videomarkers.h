/*******************************************************************************
     Copyright (c) 2026.  by Andrew Hale <halea2196@gmail.com>

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
#include <QMap>
#include <QDataStream>
#include <QSettings>
#include <QStandardPaths>

#include <constants.h>


class VideoMarkers : public QObject
{
    Q_OBJECT

public:
    explicit VideoMarkers(QObject *parent = nullptr);

    QMap<QString,QList<double>> getMarkers(QString video);
    void saveMarkers(QString video, QMap<QString,QList<double>> markerMap);

private:
    QJsonObject rootObj;
    QString video;

    QList<double> markersTypeList(const QJsonValue &markerValue);
    bool createBlankVideoMarkersFile(QString filename);
    bool loadVideoMarkersFile(QString filename);
    bool saveVideoMarkersFile(QString filename);

};
