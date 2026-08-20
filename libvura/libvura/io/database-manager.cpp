/*******************************************************************************
     Copyright (c) 2026 by Andrew Hale <halea2196@gmail.com>

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

#include <QSqlQuery>
#include <QSqlError>
#include <QStandardPaths>
#include <QDir>
#include <QVariant>
#include <QSet>
#include <QDebug>

#include "database-manager.h"
#include <libvura/config.h>


DatabaseManager::DatabaseManager(QObject *parent) : QObject(parent) {}

DatabaseManager::~DatabaseManager()
{
    if (m_database.isOpen())
        m_database.close();
}

bool DatabaseManager::initializeDatabase()
{
    qDebug() << "Initializing database...";

    const QString databaseDir = (QString(VURA_BUILD_TYPE) == "Debug") ? "debug"
                                 : QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    if (!QDir().mkpath(databaseDir)) {
        qCritical() << "Failed to create database directory: " << databaseDir;
        return false;
    }

    const QString databasePath = databaseDir + "/global.sqlite";

    if (QSqlDatabase::contains(QSqlDatabase::defaultConnection)) {
        m_database = QSqlDatabase::database(QSqlDatabase::defaultConnection);
    } else {
        m_database = QSqlDatabase::addDatabase("QSQLITE");
        m_database.setDatabaseName(databasePath);
    }

    if (!m_database.open()) {
        qCritical() << "Failed to open database: " << m_database.lastError().text();
        return false;
    }

    if (!createTables()) {
        qCritical() << "Failed to create database tables: " << m_database.lastError().text();
        return false;
    }

    qDebug() << "Database initialized successfully.";
    return true;
}

bool DatabaseManager::saveVideoMarker(const VideoMarkerRecord &record) const
{
    if (record.id <= 0) {
        return insertVideoMarker(record);
    }
    if (videoMarkerExists(record.id)) {
        return updateVideoMarker(record);
    }
    return insertVideoMarker(record);
}

int DatabaseManager::saveVideoMarkers(const QList<VideoMarkerRecord> &records) const
{
    QSqlQuery query(m_database);
    int numInserted = 0;

    for (const auto &record : records) {
        if (videoMarkerExists(record.id)) {
            if (updateVideoMarker(record)) {
                numInserted++;
                continue;
            }
        } else {
            if (insertVideoMarker(record)) {
                numInserted++;
                continue;
            }
        }
        qCritical() << "Failed to save video marker: " << record.fileName;
    }
    return numInserted;
}

bool DatabaseManager::deleteVideoMarker(const int id) const
{
    QSqlQuery query(m_database);
    query.prepare("DELETE FROM VideoMarkers WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qCritical() << "Failed to delete video marker: " << query.lastError().text();
        return false;
    }

    if (query.numRowsAffected() > 0) {
        qDebug() << "Video marker deleted successfully.";
        return true;
    }
    return false;
}

bool DatabaseManager::deleteVideoMarkers(const QString &fileName) const
{
    QSqlQuery query(m_database);
    query.prepare("DELETE FROM VideoMarkers WHERE fileName = :fileName");
    query.bindValue(":fileName", fileName);

    if (!query.exec()) {
        qCritical() << "Failed to delete video markers: " << query.lastError().text();
        return false;
    }

    if (query.numRowsAffected() > 0) {
        qDebug() << "Video markers deleted successfully.";
        return true;
    }
    return false;
}

VideoMarkerRecord DatabaseManager::getVideoMarker(const int id) const
{
    QSqlQuery query(m_database);
    query.prepare("SELECT * FROM VideoMarkers WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qCritical() << "Failed to retrieve video marker: " << query.lastError().text();
        return VideoMarkerRecord();
    }

    if (query.next()) {
        VideoMarkerRecord record;

        record.id           = query.value(0).toInt();
        record.fileName     = query.value(1).toString();
        record.markerType   = query.value(2).toString();
        record.comments     = query.value(3).toString();
        record.timestampMs  = query.value(4).toDouble();

        return record;
    }

    return VideoMarkerRecord();
}

QList<VideoMarkerRecord> DatabaseManager::getVideoMarkers(const QString &fileName) const
{
    QSqlQuery query(m_database);
    query.prepare("SELECT * FROM VideoMarkers WHERE fileName = :fileName");
    query.bindValue(":fileName", fileName);

    if (!query.exec()) {
        qCritical() << "Failed to retrieve video markers: " << query.lastError().text();
        return QList<VideoMarkerRecord>();
    }

    QList<VideoMarkerRecord> records;
    while (query.next()) {
        VideoMarkerRecord record;
        record.id           = query.value(0).toInt();
        record.fileName     = query.value(1).toString();
        record.markerType   = query.value(2).toString();
        record.comments     = query.value(3).toString();
        record.timestampMs  = query.value(4).toDouble();
        records.append(record);
    }
    return records;
}

QList<VideoMarkerRecord> DatabaseManager::getAllVideoMarkers() const
{
    QSqlQuery query(m_database);
    query.prepare("SELECT * FROM VideoMarkers");

    if (!query.exec()) {
        qCritical() << "Failed to retrieve all video markers: " << query.lastError().text();
        return QList<VideoMarkerRecord>();
    }

    QList<VideoMarkerRecord> records;
    while (query.next()) {
        VideoMarkerRecord record;
        record.id           = query.value(0).toInt();
        record.fileName     = query.value(1).toString();
        record.markerType   = query.value(2).toString();
        record.comments     = query.value(3).toString();
        record.timestampMs  = query.value(4).toDouble();
        records.append(record);
    }
    return records;
}

bool DatabaseManager::createTables() const
{
    qDebug() << "Creating database tables...";

    QSqlQuery query(m_database);

    const QString createVideoMarkersTable = "CREATE TABLE IF NOT EXISTS VideoMarkers ("
                                            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                            "fileName TEXT NOT NULL,"
                                            "markerType TEXT,"
                                            "comments TEXT,"
                                            "timestamp REAL"
                                            ")";

    if (!query.exec(createVideoMarkersTable)) {
        qCritical() << "Failed to create VideoMarkers table: " << query.lastError().text();
        return false;
    }
    qDebug() << "Database VideoMarkers table created successfully.";

    return true;
}

bool DatabaseManager::videoMarkerExists(const int id) const
{
    QSqlQuery query(m_database);
    query.prepare("SELECT COUNT(*) FROM VideoMarkers WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qCritical() << "Failed to retrieve video marker: " << query.lastError().text();
        return false;
    }

    if (query.next()) {
        if (query.value(0).toInt() > 0)
            return true;
    }

    return false;
}

bool DatabaseManager::insertVideoMarker(const VideoMarkerRecord &record) const
{
    QSqlQuery query(m_database);
    query.prepare(R"(
        INSERT OR IGNORE INTO VideoMarkers (fileName, markerType, comments, timestamp)
        VALUES (:fileName, :markerType, :comments, :timestamp)
    )");

    query.bindValue(":fileName", record.fileName);
    query.bindValue(":markerType", record.markerType);
    query.bindValue(":comments", record.comments);
    query.bindValue(":timestamp", record.timestampMs);

    if (!query.exec()) {
        qCritical() << "Failed to insert video marker: " << query.lastError().text();
        return false;
    }

    if (query.numRowsAffected() > 0) {
        qDebug() << "Video marker inserted successfully.";
        return true;
    }

    qDebug() << "Video marker already exists.";
    return false;
}

bool DatabaseManager::updateVideoMarker(const VideoMarkerRecord &record) const
{
    QSqlQuery query(m_database);
    query.prepare(R"(
        UPDATE VideoMarkers
        SET fileName = :fileName, markerType = :markerType, comments = :comments, timestamp = :timestamp
        WHERE id = :id
    )");
    query.bindValue(":id", record.id);
    query.bindValue(":fileName", record.fileName);
    query.bindValue(":markerType", record.markerType);
    query.bindValue(":comments", record.comments);
    query.bindValue(":timestamp", record.timestampMs);

    if (!query.exec()) {
        qCritical() << "Failed to update video marker: " << query.lastError().text();
        return false;
    }

    if (query.numRowsAffected() > 0) {
        qDebug() << "Video marker updated successfully.";
        return true;
    }

    qDebug() << "Video marker not found.";
    return false;
}
