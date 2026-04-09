#pragma once

#include <QObject>
#include <QList>
#include <QString>
#include <QDataStream>
#include <QFile>
#include <QStandardPaths>
#include <QDebug>

#include "util/messagebox.h"


// File Headers
inline uint32_t ApplicationDataSignature = 0x80F6E4F4;
inline uint32_t HotkeysDataSignature = 0x80F6E8EB;
inline uint32_t MarkersDataSignature = 0x80F6F6ED;
inline uint32_t PlaylistDataSignature = 0x80F6F0EC;
inline uint32_t ProjectDataSignature = 0xF6F0F2EA;

struct FileHeader
{
    uint32_t signature;
    uint32_t version = 0x31;

    friend QDataStream &operator<<(QDataStream &out, const FileHeader &header)
    {
        out << header.signature << header.version;
        return out;
    }

    friend QDataStream &operator>>(QDataStream &in, FileHeader &header)
    {
        in >> header.signature >> header.version;
        return in;
    }
};


// File Data
/*
struct ApplicationData {
    char fileName[260];
    int position;
};

struct HotkeysData {
    char action[75];
    int modifier = 0;
    int keyCode = 0;
};
*/

struct MarkersData {
    QString fileName;
    QString markerName;
    QString markerType;
    double markerTimestamp;

    friend QDataStream &operator<<(QDataStream &out, const MarkersData &data)
    {
        out << data.fileName << data.markerName << data.markerType << data.markerTimestamp;
        return out;
    }

    friend QDataStream &operator>>(QDataStream &in, MarkersData &data)
    {
        in >> data.fileName >> data.markerName >> data.markerType >> data.markerTimestamp;
        return in;
    }
};

/*
struct PlaylistData {
    char fileName[260];
    char fileType[75];
};

struct ProjectData {
    char projectName[260];
};
*/

class DataFileHandler : public QObject
{
    Q_OBJECT
public:
    explicit DataFileHandler(QObject *parent = nullptr);

    static void writeMarkersDataToFile(const QString &fileName, QList<MarkersData> data);
    static QList<MarkersData> readMarkersDataFromFile(const QString &fileName, const QString &searchFilename);

    // Create blank files
    //static bool createBlankApplicationDataFile(const QString &fileName);
    //static bool createBlankHotkeysDataFile(const QString &fileName);
    //static bool createBlankMarkersDataFile(const QString &fileName);
    //static bool createBlankPlaylistDataFile(const QString &fileName);
    //static bool createBlankProjectDataFile(const QString &fileName);

    // Write to data files
    //static bool write(const QString &fileName, const ApplicationData &data, bool overwrite = false);
    //static bool write(const QString &fileName, const HotkeysData &data, bool overwrite = false);
    //static bool write(const QString &fileName, const MarkersData &data, bool overwrite = false);
    //static bool write(const QString &fileName, const PlaylistData &data, bool overwrite = false);
    //static bool write(const QString &fileName, const ProjectData &data, bool overwrite = false);

    // Read data files
    //static QList<ApplicationData> readApplicationDataFile(const QString &fileName);
    //static QList<HotkeysData> readHotkeysDataFile(const QString &fileName);
    //static QList<MarkersData> readMarkersDataFile(const QString &fileName);
    //static QList<PlaylistData> readPlaylistDataFile(const QString &fileName);
    //static QList<ProjectData> readProjectDataFile(const QString &fileName);

    // Search data files
    //static ApplicationData searchEntryInApplicationDataFile(const QString &fileName, const char searchFilename[260]);
    //static HotkeysData searchEntryInHotkeysDataFile(const QString &fileName, const char searchAction[75]);
    //static HotkeysData searchEntryInHotkeysDataFile(const QString &fileName, int searchModifier, int searchKeyCode);
    //static bool searchEntryInMarkersDataFile(const QString &fileName, const MarkersData &data);
    //static MarkersData searchEntryInMarkersDataFile(const QString &fileName, const char searchMarkersType[75], const char searchFilename[260] = "");
    //static PlaylistData searchEntryInPlaylistDataFile(const QString &fileName, const char searchFilename[260]);
    //static ProjectData searchEntryInProjectDataFile(const QString &fileName, const char searchProjectname[260]);

    //static QList<MarkersData> searchEntryInMarkersDataFile_ToList(const QString &fileName, const QString &searchFilename);

    // Modify entry in data file
    //static bool modify(const QString &fileName, const char searchFilename[260], const ApplicationData &data);
    //static bool modify(const QString &fileName, const char searchAction[75], const HotkeysData &data);
    //static bool modify(const QString &fileName, int searchModifier, int searchKeyCode, const HotkeysData &data);
    //static bool modify(const QString &fileName, const QString &searchFilename, const MarkersData &data);
    //static bool modify(const QString &fileName, const char searchFilename[260], const PlaylistData &data);
    //static bool modify(const QString &fileName, const char searchProjectname[260], const ProjectData &data);

    // Delete entry in data file
    //static bool deleteEntryInApplicationDataFile(const QString &fileName, const char searchFilename[260]);
    //static bool deleteEntryInHotkeysDataFile(const QString &fileName, const char searchAction[75]);
    //static bool deleteEntryInHotkeysDataFile(const QString &fileName, int searchModifier, int searchKeyCode);
    //static bool deleteEntryInMarkersDataFile(const QString &fileName, const QString &searchFilename);
    //static bool deleteEntryInMarkersDataFile(const QString &fileName, const char searchMarkersType[75], const char searchFilename[260] = "");
    //static bool deleteEntryInPlaylistDataFile(const QString &fileName, const char searchFilename[260]);
    //static bool deleteEntryInProjectDataFile(const QString &fileName, const char searchProjectname[260]);

private:
    //static bool writeToFile(const QString &fileName, ApplicationData data);
    //static bool writeToFile(const QString &fileName, HotkeysData data);
    //static bool writeToFile(const QString &fileName, MarkersData data);
    //static bool writeToFile(const QString &fileName, PlaylistData data);
    //static bool writeToFile(const QString &fileName, ProjectData data);

    //static bool appendToFile(const QString &fileName, ApplicationData data);
    //static bool appendToFile(const QString &fileName, HotkeysData data);
    //static bool appendToFile(const QString &fileName, MarkersData data);
    //static bool appendToFile(const QString &fileName, PlaylistData data);
    //static bool appendToFile(const QString &fileName, ProjectData data);
    
};
