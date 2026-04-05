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

#include "vura-serializer.h"


VuraSerializer::VuraSerializer(QObject *parent) : QObject(parent) {}

void VuraSerializer::Save(const QString &fileName, QList<ApplicationData> applicationData)
{
    QFile file(fileName);

    if (!file.open(QIODevice::WriteOnly))
    {
        qDebug() << "Could not open file";
        return;
    }

    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_6_10);

    FileHeader fileHeader;
    fileHeader.signature = ApplicationDataSignature;
    out << fileHeader;

    foreach (ApplicationData data, applicationData)
    {
        out << data.fileName << data.position;
    }

    file.flush();
    file.close();

}

void VuraSerializer::Save(const QString& fileName, QList<HotkeysData> hotkeysData)
{
    QFile file(fileName);

    if (!file.open(QIODevice::WriteOnly))
    {
        qDebug() << "Could not open file";
        return;
    }

    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_6_10);

    foreach (HotkeysData data, hotkeysData)
    {
        out << data.action << data.modifier << data.keyCode;
    }

    file.flush();
    file.close();
}

void VuraSerializer::Save(const QString& fileName, QList<MarkersData> markersData)
{
    QFile file(fileName);

    if (!file.open(QIODevice::WriteOnly))
    {
        qDebug() << "Could not open file";
        return;
    }

    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_6_10);

    foreach (MarkersData data, markersData)
    {
        out << data.fileName << data.markerName << data.markerType << data.markerTimestamp;
    }

    file.flush();
    file.close();
}

void VuraSerializer::Save(const QString& fileName, QList<PlaylistData> playlistData)
{
    QFile file(fileName);

    if (!file.open(QIODevice::WriteOnly))
    {
        qDebug() << "Could not open file";
        return;
    }

    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_6_10);

    foreach (PlaylistData data, playlistData)
    {
        out << data.fileName << data.fileType;
    }

    file.flush();
    file.close();
}

void VuraSerializer::Save(const QString& fileName, QList<ProjectData> projectData)
{
    QFile file(fileName);

    if (!file.open(QIODevice::WriteOnly))
    {
        qDebug() << "Could not open file";
        return;
    }

    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_6_10);

    foreach (ProjectData data, projectData)
    {
        out << data.projectName;
    }

    file.flush();
    file.close();
}


QList<ApplicationData> VuraSerializer::LoadApplicationData(const QString& fileName)
{
    FileHeader fileHeader;
    QList<ApplicationData> applicationData;

    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Could not open file";
        return applicationData;
    }

    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_6_10);

    in >> fileHeader;

    if (fileHeader.signature == ApplicationDataSignature) {
        qDebug() << "Application data file signature matches.";
        in >> applicationData;

    } else {
        qDebug() << "Application data file signature does not match.";
    }

    file.close();

    return  applicationData;
}

QList<HotkeysData> VuraSerializer::LoadHotkeysData(const QString& fileName)
{
    QList<HotkeysData> hotkeysData;

    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Could not open file";
        return hotkeysData;
    }

    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_6_10);

    in >> hotkeysData;

    file.close();

    return  hotkeysData;
}

QList<MarkersData> VuraSerializer::LoadMarkersData(const QString& fileName)
{
    QList<MarkersData> markersData;

    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Could not open file";
        return markersData;
    }

    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_6_10);

    in >> markersData;

    file.close();

    return  markersData;
}

QList<PlaylistData> VuraSerializer::LoadPlaylistData(const QString& fileName)
{
    QList<PlaylistData> playlistData;

    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Could not open file";
        return playlistData;
    }

    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_6_10);

    in >> playlistData;

    file.close();

    return  playlistData;
}

QList<ProjectData> VuraSerializer::LoadProjectData(const QString& fileName)
{
    QList<ProjectData> projectData;

    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Could not open file";
        return projectData;
    }

    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_6_10);

    in >> projectData;

    file.close();

    return  projectData;
}
