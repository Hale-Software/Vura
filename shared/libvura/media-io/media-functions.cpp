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
    QDir dir;

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

QString MediaFunctions::generateSubclipFilenameWithIncrement(const QString &directoryPath, const QString &baseFileName,
                                                      const QString &extension)
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
    QFileInfo fileInfo(url.toString());
    QString fileExtension = fileInfo.suffix();

    if (fileExtension == "hlist")
        return true;

    return false;
}

QString MediaFunctions::strippedFileName(const QString &fileName)
{
    return QFileInfo(fileName).fileName();
}

QString MediaFunctions::timestampString(qint64 position, qint64 duration)
{
    QString tStr;
    if (position || duration) {
        QTime currentTime((position / 3600) % 60, (position / 60) % 60, position % 60,
                          (position * 1000) % 1000);
        QTime totalTime((duration / 3600) % 60, (duration / 60) % 60, duration % 60,
                        (duration * 1000) % 1000);
        QString format = "mm:ss";
        if (duration > 3600)
            format = "hh:mm:ss";
        tStr = currentTime.toString(format) + " / " + totalTime.toString(format);
    }
    return tStr;
}




