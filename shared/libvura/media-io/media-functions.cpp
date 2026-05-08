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

#include "media-functions.h"


MediaFunctions::MediaFunctions(QObject *parent) : QObject(parent) {}

void MediaFunctions::extractSubclipFromVideo(const QString &filename, const int in, const int out)
{
    qDebug() << "Marker In Position: " << QString::number(in);
    qDebug() << "Marker Out Position: " << QString::number(out);


    const QString startTimestamp = createTimestampString(in);
    const QString endTimestamp = createTimestampString(out);

    // Get the user's documents location as a QString
    const QString documentsPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    const QString applicationFolder = "Vura";
    const QFileInfo fileInfo(filename);
    const QString projectFolder = fileInfo.baseName();
    const QString fileName = "subclip";
    const QString dirPath = QDir::cleanPath(documentsPath + QDir::separator() + applicationFolder + QDir::separator() + "Clips" + QDir::separator() + projectFolder);

    // Check if project folder exists.
    const QDir dir;

    if (dir.mkpath(dirPath)) {
        qDebug() << "Successfully created project directory (and parent directories if needed):" << dirPath;
    } else {
        qDebug() << "Failed to create project directory:" << dirPath;
    }

    const QString outputFile = generateSubclipFilenameWithIncrement(dirPath, fileName, "mp4");

    qDebug() << "Input file: " << filename;
    qDebug() << "Output file: " << outputFile;
    qDebug() << "Start timestamp: " << startTimestamp;
    qDebug() << "End timestamp: " << endTimestamp;

    QProcess *ffmpegProc = new QProcess(this);
    QStringList arguments;
    arguments << "-ss" << startTimestamp << "-to" << endTimestamp << "-i" << filename << "-c" << "copy" << outputFile;

    // Optional: Connect signals to handle output/errors
    connect(ffmpegProc, &QProcess::readyReadStandardError, [=]() {
        qWarning() << ffmpegProc->readAllStandardError();
    });

    ffmpegProc->start("ffmpeg", arguments);
    // Use ffmpegProc->waitForFinished(); if synchronous behavior is needed
}

QString MediaFunctions::createTimestampString(const qint64 pos)
{
    // Convert to seconds
    const qint64 totalSeconds = pos / 1000;

    // Format as mm:ss or hh:mm:ss
    const QTime time((totalSeconds / 3600) % 24, (totalSeconds / 60) % 60, totalSeconds % 60);
    const QString format = (totalSeconds >= 3600) ? "hh:mm:ss" : "mm:ss";
    QString timestamp = time.toString(format);

    // 'timestamp' is now a QString (e.g., "01:30" or "01:05:10")
    return timestamp;
}

QString MediaFunctions::generateSubclipFilenameWithIncrement(const QString &directoryPath, const QString &baseFileName, const QString &extension)
{
    QString fullPath;
    int index = 0;

    // Extract base name without extension for numbering
    const QString nameWithoutExt = QFileInfo(baseFileName).baseName();

    do {
        if (index == 0) {
            fullPath = QDir(directoryPath).absoluteFilePath(baseFileName + "." + extension);
        } else {
            fullPath = QDir(directoryPath).absoluteFilePath(nameWithoutExt + QString::number(index) + "." + extension);
        }
        index++;
    } while (QFile::exists(fullPath)); // Check if file exists

    return fullPath;
}

// TODO: Finish function
bool MediaFunctions::isPlaylist(const QUrl &url)
{
    const QFileInfo fileInfo(url.toString());
    const QString fileExtension = fileInfo.suffix();

    if (fileExtension == "hlist")
        return true;

    return false;
}

QString MediaFunctions::strippedFileName(const QString &fileName)
{
    return QFileInfo(fileName).fileName();
}

QString MediaFunctions::timestampString(const qint64 position, const qint64 duration)
{
    QString tStr;
    if (position || duration) {
        const QTime currentTime((position / 3600) % 60, (position / 60) % 60, position % 60, (position * 1000) % 1000);
        const QTime totalTime((duration / 3600) % 60, (duration / 60) % 60, duration % 60, (duration * 1000) % 1000);
        QString format = "mm:ss";
        if (duration > 3600)
            format = "hh:mm:ss";
        tStr = currentTime.toString(format) + " / " + totalTime.toString(format);
    }
    return tStr;
}

void MediaFunctions::takeSnapshot(const QString &filename, const int &position, const QVideoFrame &frame)
{
    QString fullPath;
    int index = 0;

    // TODO: Add setting for screenshot location.
    const QString documentsDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/Vura/Screenshots";
    const QString fileBaseName = strippedFileName(filename);
    // Extract base name without extension for numbering
    QString nameWithoutExt = QFileInfo(fileBaseName).baseName();

    QString format = "mm-ss";
    if (position > 3600)
        format = "hh-mm-ss";

    const QTime currentTime(
        (position / 3600) % 60,
        (position / 60) % 60,
        position % 60,
        (position * 1000) % 1000);

    QString positionString = currentTime.toString(format);

    do {
        if (index == 0) {
            fullPath = QDir(documentsDir).absoluteFilePath(QString("%1-%2.jpg").arg(nameWithoutExt, positionString));
        } else {
            fullPath = QDir(documentsDir).absoluteFilePath(QString("%1-%2 (%3).jpg").arg(nameWithoutExt, positionString, QString::number(index)));
        }
        index++;
    } while (QFile::exists(fullPath)); // Check if file exists

    const QImage image = frame.toImage();
    image.save(fullPath, "JPEG");
}
