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
    QFile file(fileName);

    if (!file.open(QIODevice::WriteOnly))
    {
        qDebug() << "Could not open file";
        return;
    }

    //QDataStream out(&file);
    //out.setVersion(QDataStream::Qt_6_10);

    //out << markersData1;
    //out << markersData2;
    //out << markersData3;
    //out << markersData4;

    //file.flush();
    //file.close();

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

    //QDataStream in(&file);
    //in.setVersion(QDataStream::Qt_6_10);

    //in >> markersData1;
    //in >> markersData2;
    //in >> markersData3;
    //in >> markersData4;

    //file.close();

}
