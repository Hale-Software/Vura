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

void VuraSerializer::Save(const QString &fileName)
{
    MarkersData markersData1;
    markersData1.fileName = "c:/Users/halea/Videos/Extra/SDMU-724.mp4";
    markersData1.markerType = "scene";
    markersData1.markerPos = 0.3013312623925498;

    MarkersData markersData2;
    markersData2.fileName = "c:/Users/halea/Videos/Extra/SDMU-724.mp4";
    markersData2.markerType = "scene";
    markersData2.markerPos = 0.4328884249391577;

    MarkersData markersData3;
    markersData3.fileName = "file:///C:/Users/halea/Videos/Extra/Found Some hot ass looking for Tony.mp4";
    markersData3.markerType = "marker";
    markersData3.markerPos = 0.5493785095199009;

    MarkersData markersData4;
    markersData4.fileName = "file:///C:/Users/halea/Videos/Extra/GirlsDoPorn - E170 Melissa King (HQ)%5B720p%5D.mp4";
    markersData4.markerType = "scene";
    markersData4.markerPos = 0.1451034949743104;

    QFile file(fileName);

    if (!file.open(QIODevice::WriteOnly))
    {
        qDebug() << "Could not open file";
        return;
    }

    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_6_10);

    out << markersData1;
    out << markersData2;
    out << markersData3;
    out << markersData4;

    file.flush();
    file.close();

}

void VuraSerializer::Load(const QString &fileName)
{
    MarkersData markersData1;
    MarkersData markersData2;
    MarkersData markersData3;
    MarkersData markersData4;

    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Could not open file";
        return;
    }

    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_6_10);

    in >> markersData1;
    in >> markersData2;
    in >> markersData3;
    in >> markersData4;

    file.close();

    qDebug() << "Marker 1 - File: " << markersData1.fileName << " Type: " << markersData1.markerType << " Pos: " << markersData1.markerPos;
    qDebug() << "Marker 2 - File: " << markersData2.fileName << " Type: " << markersData2.markerType << " Pos: " << markersData2.markerPos;
    qDebug() << "Marker 3 - File: " << markersData3.fileName << " Type: " << markersData3.markerType << " Pos: " << markersData3.markerPos;
    qDebug() << "Marker 4 - File: " << markersData4.fileName << " Type: " << markersData4.markerType << " Pos: " << markersData4.markerPos;

}
