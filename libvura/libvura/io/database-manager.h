#pragma once

#include <QObject>
#include <QSqlDatabase>

#include "../models/video-marker-record.h"


class DatabaseManager : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager() override;

    bool initializeDatabase();
    bool saveVideoMarker(const VideoMarkerRecord &record) const;
    int saveVideoMarkers(const QList<VideoMarkerRecord> &records) const;
    bool deleteVideoMarker(int id) const;
    bool deleteVideoMarkers(const QString &fileName) const;
    VideoMarkerRecord getVideoMarker(int id) const;
    QList<VideoMarkerRecord> getVideoMarkers(const QString &fileName) const;
    QList<VideoMarkerRecord> getAllVideoMarkers() const;

private:
    bool createTables() const;
    bool videoMarkerExists(int id) const;
    bool insertVideoMarker(const VideoMarkerRecord &record) const;
    bool updateVideoMarker(const VideoMarkerRecord &record) const;

    QSqlDatabase m_database;

};
