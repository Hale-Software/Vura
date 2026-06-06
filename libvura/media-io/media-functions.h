/*******************************************************************************
     Copyright (c) 2026. by Andrew Hale <halea2196@gmail.com>

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

#include <QObject>
#include <QDebug>
#include <QString>
#include <QStringList>
#include <QDir>
#include <QFile>
#include <QUrl>
#include <QProcess>
#include <QStandardPaths>
#include <QFileInfo>
#include <QImage>
#include <QtMultimedia/QVideoFrame>
#include <QTime>


class MediaFunctions : public QObject
{
    Q_OBJECT
public:
    explicit MediaFunctions(QObject *parent = nullptr);

    void extractSubclipFromVideo(const QString &filename, int in, int out);
    static QString createTimestampString(qint64 pos);
    static QString generateSubclipFilenameWithIncrement(const QString &directoryPath, const QString &baseFileName, const QString &extension);
    static bool isPlaylist(const QUrl &url);
    static QString strippedFileName(const QString &fileName);
    static QString timestampString(qint64 position, qint64 duration);
    static void takeSnapshot(const QString &filename, const int &position, const QVideoFrame &frame);

};

