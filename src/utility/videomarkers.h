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
