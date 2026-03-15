#ifndef HDATA_H
#define HDATA_H

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

class HData : public QObject
{
    Q_OBJECT

public:
    explicit HData(QObject *parent = nullptr);

    QMap<QString,QList<double>> getMarkers(QString video);
    void saveMarkers(QString video, QMap<QString,QList<double>> markerMap);

private:
    QJsonObject rootObj;
    QString video;

    bool loadDataFile(QString filename);
    bool saveDataFile(QString filename);
    bool createBlankDataFile(QString filename);

};

#endif // HDATA_H
