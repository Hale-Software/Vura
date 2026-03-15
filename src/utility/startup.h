#ifndef HPLAYER_STARTUP_H
#define HPLAYER_STARTUP_H

#include <QString>
#include <QStringList>
#include <QSettings>
#include <QStandardPaths>
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>


class Startup {

public:
    Startup();

    bool Initialize(bool initDirectories = true, bool initApplicationFiles = true);

private:
    bool m_directoriesStatus = true;
    bool m_markerFileStatus = true;

    void InitDirectories();
    void InitApplicationFiles();

    bool createEmptyJsonFile(const QString& filePath);

};

#endif //HPLAYER_STARTUP_H
