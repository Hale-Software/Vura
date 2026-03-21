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

#include "startup.h"

#include <config.h>


Startup::Startup() {}

bool Startup::Initialize(bool initDirectories, bool initApplicationFiles)
{
    qDebug() << "Application startup initializing....";

    if (initDirectories) {
        InitDirectories();
    }

    if (initApplicationFiles) {
        InitApplicationFiles();
    }

    if (initDirectories && !m_directoriesStatus) {
        return false;
    }

    if (initApplicationFiles && !m_markerFileStatus) {
        return false;
    }

    return true;
}

void Startup::InitDirectories()
{
    // Application Directories
    qDebug() << "Running application directory initialization...";
    QStringList directoryList;
    QString appDataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (VURA_BUILD_TYPE == "Debug") {
        appDataDir = constants::ApplicationDebugFolder;
    }
    directoryList << appDataDir;
    directoryList << appDataDir + "/crashes";
    directoryList << appDataDir + "/logs";
    directoryList << appDataDir + "/updates";

    foreach (QString directory, directoryList)
    {
        if (!QDir(directory).exists()) {
            qDebug() << "Directory " << directory << " does not exist. Creating...";
            if (QDir().mkpath(directory)) {
                qDebug() << "Created directory: " << directory;
            } else {
                qWarning() << "Could not create directory: " << directory;
                m_directoriesStatus = false;
            }
        }
    }

    if (m_directoriesStatus) {
        qDebug() << "Application directories initialized!";
    }
}

void Startup::InitApplicationFiles()
{
    QSettings settings;

    // Marker File
    qDebug() << "Checking if marker file from settings exists...";
    QString defaultMarkerFile = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/markers.json";
    if (VURA_BUILD_TYPE == "Debug") {
        defaultMarkerFile = constants::ApplicationDebugFolder + "/markers.json";
    }
    QString markerFile = settings.value("markerFile", defaultMarkerFile).toString();

    if (QFile::exists(markerFile)) {
        qDebug() << "Marker file exists!";
    } else {
        qDebug() << "Marker file does not exists! Creating marker file...";
        if (createEmptyJsonFile(markerFile)) {
            qDebug() << "Marker file created!";
        } else {
            qWarning() << "Could not create marker file!";
            m_markerFileStatus = false;
        }
    }

    if (m_markerFileStatus) {
        qDebug() << "Using marker file: " << markerFile;
    }
}

bool Startup::createEmptyJsonFile(const QString &filePath)
{
    QJsonObject jsonObject;
    QJsonDocument doc(jsonObject);
    QByteArray jsonData = doc.toJson();

    QFile saveFile(filePath);
    if (!saveFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Failed to open save file:" << filePath;
        return false;
    }

    saveFile.write(jsonData);
    saveFile.close();

    qDebug() << "Empty JSON file created successfully at:" << filePath;
    return true;
}
