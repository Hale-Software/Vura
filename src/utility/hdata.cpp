#include "hdata.h"

#include <config.h>

HData::HData(QObject *parent)
    : QObject{parent}
{
    QSettings settings;
    QString defaultMarkerFile = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/markers.json";
    if (VURA_BUILD_TYPE == "Debug") {
        defaultMarkerFile = "C:/Users/halea/vura-debug/markers.json";
    }

    if (!loadDataFile(settings.value("markerFile", defaultMarkerFile).toString())) {
        qDebug() << "Failed to load marker data file.";
        if (createBlankDataFile(settings.value("markerFile", defaultMarkerFile).toString())) {
            qDebug() << "Created blank marker data file.";
            if (!loadDataFile(settings.value("markerFile", defaultMarkerFile).toString())) {
                qDebug() << "Failed to load blank marker data file.";
            } else {
                qDebug() << "Loaded blank marker data file.";
            }
        } else {
            qDebug() << "Failed to create blank marker data file.";
        }
    }
}

QMap<QString,QList<double>> HData::getMarkers(QString video)
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
    if (!markerValue.isArray()) {
        qDebug() << "Video marker list is empty";
    }
    else
    {
        qDebug() << "Loading video marker list";
        QJsonArray jsonArray = markerValue.toArray();

        for (const QJsonValue &value : jsonArray) {
            if (value.isDouble()) {
                markerList.append(value.toDouble());
            }
        }
    }

    QJsonValue sceneValue = videoObj.value("scene");
    if (!sceneValue.isArray()) {
        qDebug() << "Video scene marker list is empty";
    }
    else
    {
        qDebug() << "Loading video scene marker list";
        QJsonArray jsonArray = sceneValue.toArray();

        for (const QJsonValue &value : jsonArray) {
            if (value.isDouble()) {
                sceneList.append(value.toDouble());
            }
        }
    }

    QJsonValue cumshotValue = videoObj.value("cumshot");
    if (!cumshotValue.isArray()) {
        qDebug() << "Video cumshot marker list is empty";
    }
    else
    {
        qDebug() << "Loading video cumshot marker list";
        QJsonArray jsonArray = cumshotValue.toArray();

        for (const QJsonValue &value : jsonArray) {
            if (value.isDouble()) {
                cumshotList.append(value.toDouble());
            }
        }
    }

    QJsonValue stripValue = videoObj.value("strip");
    if (!stripValue.isArray()) {
        qDebug() << "Video strip marker list is empty";
    }
    else
    {
        qDebug() << "Loading video strip marker list";
        QJsonArray jsonArray = stripValue.toArray();

        for (const QJsonValue &value : jsonArray) {
            if (value.isDouble()) {
                stripList.append(value.toDouble());
            }
        }
    }

    QJsonValue dialogValue = videoObj.value("dialog");
    if (!dialogValue.isArray()) {
        qDebug() << "Video dialog marker list is empty";
    }
    else
    {
        qDebug() << "Loading video dialog marker list";
        QJsonArray jsonArray = dialogValue.toArray();

        for (const QJsonValue &value : jsonArray) {
            if (value.isDouble()) {
                dialogList.append(value.toDouble());
            }
        }
    }

    QJsonValue cyanValue = videoObj.value("cyan");
    if (!cyanValue.isArray()) {
        qDebug() << "Video cyan marker list is empty";
    }
    else
    {
        qDebug() << "Loading video cyan marker list";
        QJsonArray jsonArray = cyanValue.toArray();

        for (const QJsonValue &value : jsonArray) {
            if (value.isDouble()) {
                cyanList.append(value.toDouble());
            }
        }
    }

    QJsonValue magentaValue = videoObj.value("magenta");
    if (!magentaValue.isArray()) {
        qDebug() << "Video magenta marker list is empty";
    }
    else
    {
        qDebug() << "Loading video magenta marker list";
        QJsonArray jsonArray = magentaValue.toArray();

        for (const QJsonValue &value : jsonArray) {
            if (value.isDouble()) {
                magentaList.append(value.toDouble());
            }
        }
    }

    QJsonValue orangeValue = videoObj.value("orange");
    if (!orangeValue.isArray()) {
        qDebug() << "Video orange marker list is empty";
    }
    else
    {
        qDebug() << "Loading video orange marker list";
        QJsonArray jsonArray = orangeValue.toArray();

        for (const QJsonValue &value : jsonArray) {
            if (value.isDouble()) {
                orangeList.append(value.toDouble());
            }
        }
    }

    qDebug() << "loaded markers for file: " << video;
    qDebug() << "number of markers found: " << markerList.length();
    qDebug() << "number of scene markers found: " << sceneList.length();
    qDebug() << "number of cumshot markers found: " << cumshotList.length();
    qDebug() << "number of strip markers found: " << stripList.length();
    qDebug() << "number of dialog markers found: " << dialogList.length();
    qDebug() << "number of cyan markers found: " << cyanList.length();
    qDebug() << "number of magenta markers found: " << magentaList.length();
    qDebug() << "number of orange markers found: " << orangeList.length();

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

void HData::saveMarkers(QString video, QMap<QString,QList<double>> markerMap)
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

    if (!saveDataFile(settings.value("markerFile", defaultMarkerFile).toString())) {
        qDebug() << "Failed to save marker data file";
    }
}

bool HData::loadDataFile(QString filename)
{
    QFile dataFile(filename);
    if (!dataFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Could not read marker data file: " << dataFile.errorString();
        return false;
    }

    QJsonDocument doc = QJsonDocument::fromJson(dataFile.readAll());
    if (doc.isNull()) {
        qDebug() << "Failed to parse JSON document in marker data file";
        return false;
    }

    if (!doc.isObject()) {
        qDebug() << "JSON document is not an object in marker data file";
        return false;
    }

    rootObj = doc.object();
    qDebug() << "Loaded marker data file";
    return true;
}

bool HData::saveDataFile(QString filename)
{
    QByteArray data = QJsonDocument(rootObj).toJson();

    QFile file(filename);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        file.write(data);
        file.close();
        qDebug() << "Saved marker data file";
        return true;
    } else {
        qDebug() << "Error saving marker data file: " << file.errorString();
    }
    return false;
}

bool HData::createBlankDataFile(QString filename)
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
        qDebug() << "Saved marker data file";
        return true;
    } else {
        qDebug() << "Error saving marker data file: " << file.errorString();
    }
    return false;
}
