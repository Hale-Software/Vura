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

#include <QtCore/QtCore>

#include "vura-data.h"


QDataStream &operator<<(QDataStream &out, const FileHeader &fileHeader);
QDataStream &operator>>(QDataStream &in, FileHeader &fileHeader);

inline QDataStream &operator<<(QDataStream &out, const FileHeader &fileHeader)
{
    out << fileHeader.signature << fileHeader.version;
    return out;
}

inline QDataStream &operator>>(QDataStream &in, FileHeader &fileHeader)
{
    fileHeader = FileHeader();
    in >> fileHeader.signature >> fileHeader.version;
    return in;
}


QDataStream &operator<<(QDataStream &out, const ApplicationData &applicationData);
QDataStream &operator>>(QDataStream &in, ApplicationData &applicationData);

inline QDataStream &operator<<(QDataStream &out, const ApplicationData &applicationData)
{
    out << applicationData.fileName << applicationData.position;
    return out;
}

inline QDataStream &operator>>(QDataStream &in, ApplicationData &applicationData)
{
    applicationData = ApplicationData();
    in >> applicationData.fileName >> applicationData.position;
    return in;
}


QDataStream &operator<<(QDataStream &out, const HotkeysData &hotkeysData);
QDataStream &operator>>(QDataStream &in, HotkeysData &hotkeysData);

inline QDataStream &operator<<(QDataStream &out, const HotkeysData &hotkeysData)
{
    out << hotkeysData.action << hotkeysData.modifier << hotkeysData.keyCode;
    return out;
}

inline QDataStream &operator>>(QDataStream &in, HotkeysData &hotkeysData)
{
    hotkeysData = HotkeysData();
    in >> hotkeysData.action >> hotkeysData.modifier >> hotkeysData.keyCode;
    return in;
}


QDataStream &operator<<(QDataStream &out, const MarkersData &markers);
QDataStream &operator>>(QDataStream &in, MarkersData &markers);

inline QDataStream &operator<<(QDataStream &out, const MarkersData &markers)
{
    // Write a MarkersData class to the stream
    out << markers.fileName << markers.markerName << markers.markerType << markers.markerTimestamp;
    return out;
}

inline QDataStream &operator>>(QDataStream &in, MarkersData &markers)
{
    // Read in a MarkersData class
    markers = MarkersData();
    in >> markers.fileName >> markers.markerName >> markers.markerType >> markers.markerTimestamp;
    return in;
}

QDataStream &operator<<(QDataStream &out, const PlaylistData &playlistData);
QDataStream &operator>>(QDataStream &in, PlaylistData &playlistData);

inline QDataStream &operator<<(QDataStream &out, const PlaylistData &playlistData)
{
    out << playlistData.fileName << playlistData.fileType;
    return out;
}

inline QDataStream &operator>>(QDataStream &in, PlaylistData &playlistData)
{
    playlistData = PlaylistData();
    in >> playlistData.fileName >> playlistData.fileType;
    return in;
}


QDataStream &operator<<(QDataStream &out, const ProjectData &projectData);
QDataStream &operator>>(QDataStream &in, ProjectData &projectData);

inline QDataStream &operator<<(QDataStream &out, const ProjectData &projectData)
{
    out << projectData.projectName;
    return out;
}

inline QDataStream &operator>>(QDataStream &in, ProjectData &projectData)
{
    projectData = ProjectData();
    in >> projectData.projectName;
    return in;
}
