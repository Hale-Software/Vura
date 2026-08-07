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

#include "helpers.h"


void Helpers::simulateApplicationCrash()
{
    int *ptr = nullptr;
    *ptr = 42; // Accessing address 0 causes a SIGSEGV/Access Violation
}

bool Helpers::fileExists(const QString &fileName)
{
    QFileInfo file(fileName);
    if (file.exists() && file.isFile()) {
        return true;
    }

    return false;
}

const char *Helpers::QStringToChar(const QString &text)
{
    QByteArray byteArray = text.toUtf8();
    return byteArray.constData();
}

int Helpers::qint64ToInt(const qint64 &number)
{
    return static_cast<int>(number);
}

QString Helpers::createTimestampString(const qint64 pos)
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

QString Helpers::generateSubclipFilenameWithIncrement(const QString &directoryPath,
    const QString &baseFileName, const QString &extension)
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
bool Helpers::isPlaylist(const QUrl &url)
{
    const QFileInfo fileInfo(url.toString());
    const QString fileExtension = fileInfo.suffix();

    if (fileExtension == "hlist")
        return true;

    return false;
}

QString Helpers::strippedFileName(const QString &fileName)
{
    return QFileInfo(fileName).fileName();
}

QString Helpers::timestampString(const qint64 position, const qint64 duration)
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

QString Helpers::networkUrlFormatter(QString networkUrl)
{
    if (networkUrl.startsWith("vura://", Qt::CaseInsensitive)) {
        networkUrl.remove(0, 7);
    }

    const QRegularExpression httpRegex(R"(^(https?)(?::*)(?://)?)", QRegularExpression::CaseInsensitiveOption);

    if (httpRegex.match(networkUrl).hasMatch()) {
        const QRegularExpressionMatch match = httpRegex.match(networkUrl);
        const QString protocol = match.captured(1).toLower();

        networkUrl.remove(httpRegex);
        networkUrl = protocol + "://" + networkUrl;
    }

    if (!networkUrl.endsWith("/stream", Qt::CaseInsensitive)) {
        if (networkUrl.endsWith('/')) {
            networkUrl.chop(1);
        }
        networkUrl.append("/stream");
    }
    return networkUrl;
}
