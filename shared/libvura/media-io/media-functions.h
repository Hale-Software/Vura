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
    bool isPlaylist(const QUrl &url);
    QString strippedFileName(const QString &fileName);
    QString timestampString(qint64 position, qint64 duration);
    void takeSnapshot(const QString &filename, const int &position, const QVideoFrame &frame);

};

