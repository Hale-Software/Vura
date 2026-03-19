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

#include "applicationdata.h"
#include <config.h>


ApplicationData::ApplicationData(QObject *parent) : QObject{parent}
{
    QString defaultFile = "appdata.json";

    if (!loadFile(settings.value(defaultFile)) {
        qWarning() << "Failed to load application data file from settings.";

        if (createBlankFile(defaultFile)) {
            qDebug() << "Created blank application data file.";

            if (!loadFile(defaultFile)) {
                qCritical() << "Failed to load blank application data file.";

            }
        } else {
            qCritical() << "Failed to create blank application data file.";
        }
    }
}

QMap<QString, int> ApplicationData::getPlayHistory(QString filename)
{

}

QMap<QString,QList<double>> VideoMarkers::getMarkers(QString video)
{
    QMap<QString,QList<double>> markerMap;

    QList<double> markerList;
    QList<double> sceneList;
    QList<double> cumshotList;
    QList<double> stripList;
    QList<double> dialogList;
    QList<double> cyanList;
    QList<double> magentaList;
    QList<double> orangeList;

    QJsonObject videoObj = rootObj.value(video).toObject();

    QJsonValue markerValue = videoObj.value("marker");
    markerList = markersTypeList(markerValue);

    QJsonValue sceneValue = videoObj.value("scene");
    sceneList = markersTypeList(sceneValue);

    QJsonValue cumshotValue = videoObj.value("cumshot");
    cumshotList = markersTypeList(cumshotValue);

    QJsonValue stripValue = videoObj.value("strip");
    stripList = markersTypeList(stripValue);

    QJsonValue dialogValue = videoObj.value("dialog");
    dialogList = markersTypeList(dialogValue);

    QJsonValue cyanValue = videoObj.value("cyan");
    cyanList = markersTypeList(cyanValue);

    QJsonValue magentaValue = videoObj.value("magenta");
    magentaList = markersTypeList(magentaValue);

    QJsonValue orangeValue = videoObj.value("orange");
    orangeList = markersTypeList(orangeValue);

    markerMap.insert("marker", markerList);
    markerMap.insert("strip", stripList);
    markerMap.insert("scene", sceneList);
    markerMap.insert("cumshot", cumshotList);
    markerMap.insert("dialog", dialogList);
    markerMap.insert("cyan", cyanList);
    markerMap.insert("magenta", magentaList);
    markerMap.insert("orange", orangeList);

    return markerMap;
}

void VideoMarkers::saveMarkers(QString video, QMap<QString,QList<double>> markerMap)
{
    QSettings settings;
    QString defaultMarkerFile = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/markers.json";
    if (VURA_BUILD_TYPE == "Debug") {
        defaultMarkerFile = "C:/Users/halea/vura-debug/markers.json";
    }

    QList<double> markerList = markerMap.value("marker").toList();
    QList<double> sceneList = markerMap.value("scene").toList();
    QList<double> cumshotList = markerMap.value("cumshot").toList();
    QList<double> stripList = markerMap.value("strip").toList();
    QList<double> dialogList = markerMap.value("dialog").toList();
    QList<double> cyanList = markerMap.value("cyan").toList();
    QList<double> magentaList = markerMap.value("magenta").toList();
    QList<double> orangeList = markerMap.value("orange").toList();

    QJsonObject videoObj;

    QJsonArray markerArray;
    for (double marker : markerList) {
        markerArray.append(marker);
    }
    videoObj["marker"] = markerArray;

    QJsonArray sceneArray;
    for (double marker : sceneList) {
        sceneArray.append(marker);
    }
    videoObj["scene"] = sceneArray;

    QJsonArray cumshotArray;
    for (double marker : cumshotList) {
        cumshotArray.append(marker);
    }
    videoObj["cumshot"] = cumshotArray;

    QJsonArray stripArray;
    for (double marker : stripList) {
        stripArray.append(marker);
    }
    videoObj["strip"] = stripArray;

    QJsonArray dialogArray;
    for (double marker : dialogList) {
        dialogArray.append(marker);
    }
    videoObj["dialog"] = dialogArray;

    QJsonArray cyanArray;
    for (double marker : cyanList) {
        cyanArray.append(marker);
    }
    videoObj["cyan"] = cyanArray;

    QJsonArray magentaArray;
    for (double marker : magentaList) {
        magentaArray.append(marker);
    }
    videoObj["magenta"] = magentaArray;

    QJsonArray orangeArray;
    for (double marker : orangeList) {
        orangeArray.append(marker);
    }
    videoObj["orange"] = orangeArray;

    rootObj[video] = videoObj;

    if (!saveVideoMarkersFile(settings.value("markerFile", defaultMarkerFile).toString())) {
        qCritical() << "Failed to save video markers file";
    }
}


bool VideoMarkers::createBlankFile(QString filename)
{
    QJsonObject blankObj;
    QJsonArray blankArray;
    blankObj["blank"] = blankArray;
    QJsonDocument doc = QJsonDocument(blankObj);
    QByteArray data = QJsonDocument(doc).toJson();

    QFile file(filename);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        file.write(data);
        file.close();
        return true;
    } else {
        qCritical() << "Error saving application data file: " << filename << ". Error: " << file.errorString();
    }
    return false;
}

bool VideoMarkers::loadFile(QString filename)
{
    QFile dataFile(filename);
    if (!dataFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical() << "Could not read application data file: " << dataFile.errorString();
        return false;
    }

    QJsonDocument doc = QJsonDocument::fromJson(dataFile.readAll());
    if (doc.isNull()) {
        qCritical() << "Failed to parse JSON document in application data file";
        return false;
    }

    if (!doc.isObject()) {
        qCritical() << "JSON document is not an object in application data file";
        return false;
    }

    rootObj = doc.object();
    return true;
}

bool VideoMarkers::saveFile(QString filename)
{
    QByteArray data = QJsonDocument(rootObj).toJson();

    QFile file(filename);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        file.write(data);
        file.close();
        return true;
    } else {
        qCritical() << "Error saving application data file: " << filename << ". Error: " << file.errorString();
    }
    return false;
}
