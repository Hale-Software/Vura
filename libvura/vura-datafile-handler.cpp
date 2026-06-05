#include "vura-datafile-handler.h"


DataFileHandler::DataFileHandler(QObject *parent) : QObject(parent) {}

// Create blank files
bool DataFileHandler::createBlankApplicationDataFile(const QString &fileName)
{
    FileHeader fileHeader;
    fileHeader.signature = ApplicationDataSignature;

    std::ofstream file(fileName.toStdString(), std::ios::binary);

    if (!file) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Failed to create blank application data file. Application data will not be saved!");
        return false;
    }

    file.write(reinterpret_cast<const char*>(&fileHeader), sizeof(fileHeader));
    file.flush();
    file.close();

    return true;
}

bool DataFileHandler::createBlankHotkeysDataFile(const QString &fileName)
{
    FileHeader fileHeader;
    fileHeader.signature = HotkeysDataSignature;

    std::ofstream file(fileName.toStdString(), std::ios::binary);

    if (!file) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Failed to create blank hotkeys data file. Hotkeys data will not be saved!");
        return false;
    }

    file.write(reinterpret_cast<const char*>(&fileHeader), sizeof(fileHeader));
    file.flush();
    file.close();

    return true;
}

bool DataFileHandler::createBlankMarkersDataFile(const QString &fileName)
{
    FileHeader fileHeader;
    fileHeader.signature = MarkersDataSignature;

    std::ofstream file(fileName.toStdString(), std::ios::binary);

    if (!file) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Failed to create blank markers data file. Markers data will not be saved!");
        return false;
    }

    file.write(reinterpret_cast<const char*>(&fileHeader), sizeof(fileHeader));
    file.flush();
    file.close();

    return true;
}

bool DataFileHandler::createBlankPlaylistDataFile(const QString &fileName)
{
    FileHeader fileHeader;
    fileHeader.signature = PlaylistDataSignature;

    std::ofstream file(fileName.toStdString(), std::ios::binary);

    if (!file) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Failed to create blank playlist data file. Playlist data will not be saved!");
        return false;
    }

    file.write(reinterpret_cast<const char*>(&fileHeader), sizeof(fileHeader));
    file.flush();
    file.close();

    return true;
}

bool DataFileHandler::createBlankProjectDataFile(const QString &fileName)
{
    FileHeader fileHeader;
    fileHeader.signature = ProjectDataSignature;

    std::ofstream file(fileName.toStdString(), std::ios::binary);

    if (!file) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Failed to create blank project data file. Project data will not be saved!");
        return false;
    }

    file.write(reinterpret_cast<const char*>(&fileHeader), sizeof(fileHeader));
    file.flush();
    file.close();

    return true;
}


// Write to data files
bool DataFileHandler::write(const QString &fileName, const ApplicationData &data, const bool overwrite)
{
    ApplicationData existing = searchEntryInApplicationDataFile(fileName, data.fileName);

    if (existing.fileName[0] != '\0') {
        if (overwrite) {
            return appendToFile(fileName, data);

        } else {
            VMessageBox::warning(nullptr,
                "Vura Data Handler",
                "Entry exists in application data file and overwrite is set to false. Entry will not be saved to application data file!");
            return false;
        }
    }

    return writeToFile(fileName, data);
}

bool DataFileHandler::write(const QString &fileName, const HotkeysData &data, const bool overwrite)
{
    HotkeysData existing = searchEntryInHotkeysDataFile(fileName, data.action);

    if (existing.action[0] != '\0') {
        if (overwrite) {
            return appendToFile(fileName, data);

        } else {
            VMessageBox::warning(nullptr,
                "Vura Data Handler",
                "Entry exists in hotkeys data file and overwrite is set to false. Entry will not be saved to hotkeys data file!");
            return false;
        }
    }

    return writeToFile(fileName, data);
}

bool DataFileHandler::write(const QString &fileName, const MarkersData &data, const bool overwrite)
{
    bool existing = searchEntryInMarkersDataFile(fileName, data);

    if (existing) {
        if (overwrite) {
            return writeToFile(fileName, data);

        } else {
            VMessageBox::warning(nullptr,
                "Vura Data Handler",
                "Entry exists in markers data file and overwrite is set to false. Entry will not be saved to markers data file!");
            return false;
        }
    }

    return appendToFile(fileName, data);
}

bool DataFileHandler::write(const QString &fileName, const PlaylistData &data, const bool overwrite)
{
    PlaylistData existing = searchEntryInPlaylistDataFile(fileName, data.fileName);

    if (existing.fileName[0] != '\0') {
        if (overwrite) {
            return appendToFile(fileName, data);

        } else {
            VMessageBox::warning(nullptr,
                "Vura Data Handler",
                "Entry exists in playlist data file and overwrite is set to false. Entry will not be saved to playlist data file!");
            return false;
        }
    }

    return writeToFile(fileName, data);
}

bool DataFileHandler::write(const QString &fileName, const ProjectData &data, const bool overwrite)
{
    ProjectData existing = searchEntryInProjectDataFile(fileName, data.projectName);

    if (existing.projectName[0] != '\0') {
        if (overwrite) {
            return appendToFile(fileName, data);

        } else {
            VMessageBox::warning(nullptr,
                "Vura Data Handler",
                "Entry exists in project data file and overwrite is set to false. Entry will not be saved to project data file!");
            return false;
        }
    }

    return writeToFile(fileName, data);
}


// Read data files
QList<ApplicationData> DataFileHandler::readApplicationDataFile(const QString &fileName)
{
    FileHeader fileHeader;
    ApplicationData data;
    QList<ApplicationData> results;

    std::ifstream file(fileName.toStdString(), std::ios::binary);
    if (!file) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Failed to read application data file!");
        return results;
    }

    file.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    if (fileHeader.signature != ApplicationDataSignature) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Application data file header signature is incorrect! Please update settings");
        file.close();
        return results;
    }

    while (file.read(reinterpret_cast<char*>(&data), sizeof(data))) {
        results.append(data);
    }
    file.close();

    return results;
}

QList<HotkeysData> DataFileHandler::readHotkeysDataFile(const QString &fileName)
{
    FileHeader fileHeader;
    HotkeysData data;
    QList<HotkeysData> results;

    std::ifstream file(fileName.toStdString(), std::ios::binary);
    if (!file) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Failed to read hotkeys data file!");
        return results;
    }

    file.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    if (fileHeader.signature != HotkeysDataSignature) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Hotkeys data file header signature is incorrect! Please update settings");
        file.close();
        return results;
    }

    while (file.read(reinterpret_cast<char*>(&data), sizeof(data))) {
        results.append(data);
    }
    file.close();

    return results;
}

QList<MarkersData> DataFileHandler::readMarkersDataFile(const QString &fileName)
{
    FileHeader fileHeader;
    MarkersData data;
    QList<MarkersData> results;

    std::ifstream file(fileName.toStdString(), std::ios::binary);
    if (!file) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Failed to read markers data file!");
        return results;
    }

    file.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    if (fileHeader.signature != MarkersDataSignature) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Markers data file header signature is incorrect! Please update settings");
        file.close();
        return results;
    }

    while (file.read(reinterpret_cast<char*>(&data), sizeof(data))) {
        results.append(data);
    }
    file.close();

    return results;
}

QList<PlaylistData> DataFileHandler::readPlaylistDataFile(const QString &fileName)
{
    FileHeader fileHeader;
    PlaylistData data;
    QList<PlaylistData> results;

    std::ifstream file(fileName.toStdString(), std::ios::binary);
    if (!file) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Failed to read playlist data file!");
        return results;
    }

    file.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    if (fileHeader.signature != PlaylistDataSignature) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Playlist data file header signature is incorrect! Please update settings");
        file.close();
        return results;
    }

    while (file.read(reinterpret_cast<char*>(&data), sizeof(data))) {
        results.append(data);
    }
    file.close();

    return results;
}

QList<ProjectData> DataFileHandler::readProjectDataFile(const QString &fileName)
{
    FileHeader fileHeader;
    ProjectData data;
    QList<ProjectData> results;

    std::ifstream file(fileName.toStdString(), std::ios::binary);
    if (!file) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Failed to read project data file!");
        return results;
    }

    file.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    if (fileHeader.signature != ProjectDataSignature) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Project data file header signature is incorrect! Please update settings");
        file.close();
        return results;
    }

    while (file.read(reinterpret_cast<char*>(&data), sizeof(data))) {
        results.append(data);
    }
    file.close();

    return results;
}


// Search data files
ApplicationData DataFileHandler::searchEntryInApplicationDataFile(const QString &fileName, const char searchFilename[260])
{
    FileHeader fileHeader;
    ApplicationData result;
    bool found = false;

    std::ifstream file(fileName.toStdString(), std::ios::binary);
    if (!file) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Failed to read application data file!");
        return ApplicationData();
    }

    file.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    if (fileHeader.signature != ApplicationDataSignature) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Application data file header signature is incorrect! Please update settings");
        file.close();
        return ApplicationData();
    }

    while (file.read(reinterpret_cast<char*>(&result), sizeof(result))) {
        if (strcmp(result.fileName, searchFilename) == 0) {
            found = true;
            break;
        }
    }
    file.close();

    if (!found)
        return ApplicationData();

    return result;
}

HotkeysData DataFileHandler::searchEntryInHotkeysDataFile(const QString &fileName, const char searchAction[75])
{
    FileHeader fileHeader;
    HotkeysData result;
    bool found = false;

    std::ifstream file(fileName.toStdString(), std::ios::binary);
    if (!file) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Failed to read hotkeys data file!");
        return HotkeysData();
    }

    file.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    if (fileHeader.signature != HotkeysDataSignature) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Hotkeys data file header signature is incorrect! Please update settings");
        file.close();
        return HotkeysData();
    }

    while (file.read(reinterpret_cast<char*>(&result), sizeof(result))) {
        if (strcmp(result.action, searchAction) == 0) {
            found = true;
            break;
        }
    }
    file.close();

    if (!found)
        return HotkeysData();

    return result;
}

HotkeysData DataFileHandler::searchEntryInHotkeysDataFile(const QString &fileName, const int searchModifier,
                                                          const int searchKeyCode)
{
    FileHeader fileHeader;
    HotkeysData result;
    bool found = false;

    std::ifstream file(fileName.toStdString(), std::ios::binary);
    if (!file) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Failed to read hotkeys data file!");
        return HotkeysData();
    }

    file.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    if (fileHeader.signature != HotkeysDataSignature) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Hotkeys data file header signature is incorrect! Please update settings");
        file.close();
        return HotkeysData();
    }

    while (file.read(reinterpret_cast<char*>(&result), sizeof(result))) {
        if (result.modifier == searchModifier && result.keyCode == searchKeyCode) {
            found = true;
            break;
        }
    }
    file.close();

    if (!found)
        return HotkeysData();

    return result;
}

bool DataFileHandler::searchEntryInMarkersDataFile(const QString &fileName, const MarkersData &data)
{
    FileHeader fileHeader;
    MarkersData result;
    bool found = false;

    std::ifstream file(fileName.toStdString(), std::ios::binary);
    if (!file) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Failed to read markers data file!");
        return false;
    }

    file.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    if (fileHeader.signature != MarkersDataSignature) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Markers data file header signature is incorrect! Please update settings");
        file.close();
        return false;
    }

    while (file.read(reinterpret_cast<char*>(&result), sizeof(result))) {
        if (strcmp(result.fileName.toStdString().c_str(), data.fileName.toStdString().c_str()) == 0) {
            if (strcmp(result.markerType.toStdString().c_str(), data.markerType.toStdString().c_str()) == 0) {
                if (result.markerTimestamp == data.markerTimestamp) {
                    found = true;
                    break;
                }
            }
        }
    }
    file.close();

    return found;
}

/*
MarkersData DataFileHandler::searchEntryInMarkersDataFile(const QString &fileName, const char searchMarkersType[75], const char searchFilename[260])
{
    FileHeader fileHeader;
    MarkersData result;
    bool found = false;

    std::ifstream file(fileName.toStdString(), std::ios::binary);
    if (!file) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Failed to read markers data file!");
        return MarkersData();
    }

    file.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    if (fileHeader.signature != MarkersDataSignature) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Markers data file header signature is incorrect! Please update settings");
        file.close();
        return MarkersData();
    }

    while (file.read(reinterpret_cast<char*>(&result), sizeof(result))) {
        if (strcmp(result.markerType, searchMarkersType) == 0) {
            if (searchFilename[0] != '\0') {
                if (strcmp(result.fileName, searchFilename) == 0) {
                    found = true;
                    break;
                }

            } else {
                found = true;
                break;
            }
        }
    }
    file.close();

    if (!found)
        return MarkersData();

    return result;
}
*/

PlaylistData DataFileHandler::searchEntryInPlaylistDataFile(const QString &fileName, const char searchFilename[260])
{
    FileHeader fileHeader;
    PlaylistData result;
    bool found = false;

    std::ifstream file(fileName.toStdString(), std::ios::binary);
    if (!file) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Failed to read playlist data file!");
        return PlaylistData();
    }

    file.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    if (fileHeader.signature != PlaylistDataSignature) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Playlist data file header signature is incorrect! Please update settings");
        file.close();
        return PlaylistData();
    }

    while (file.read(reinterpret_cast<char*>(&result), sizeof(result))) {
        if (strcmp(result.fileName, searchFilename) == 0) {
            found = true;
            break;
        }
    }
    file.close();

    if (!found)
        return PlaylistData();

    return result;
}

ProjectData DataFileHandler::searchEntryInProjectDataFile(const QString &fileName, const char searchProjectname[260])
{
    FileHeader fileHeader;
    ProjectData result;
    bool found = false;

    std::ifstream file(fileName.toStdString(), std::ios::binary);
    if (!file) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Failed to read project data file!");
        return ProjectData();
    }

    file.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    if (fileHeader.signature != ProjectDataSignature) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Project data file header signature is incorrect! Please update settings");
        file.close();
        return ProjectData();
    }

    while (file.read(reinterpret_cast<char*>(&result), sizeof(result))) {
        if (strcmp(result.projectName, searchProjectname) == 0) {
            found = true;
            break;
        }
    }
    file.close();

    if (!found)
        return ProjectData();

    return result;
}

QList<MarkersData> DataFileHandler::searchEntryInMarkersDataFile_ToList(const QString &fileName,
                                                                        const QString &searchFilename)
{
    FileHeader fileHeader;
    QList<MarkersData> results;
    MarkersData data;

    std::ifstream file(fileName.toStdString(), std::ios::binary);
    if (!file) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Failed to read markers data file!");
        return results;
    }

    file.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    if (fileHeader.signature != MarkersDataSignature) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Markers data file header signature is incorrect! Please update settings");
        file.close();
        return results;
    }

    while (file.read(reinterpret_cast<char*>(&data), sizeof(data))) {
        if (strcmp(data.fileName.toStdString().c_str(), searchFilename.toStdString().c_str()) == 0) {
            results.append(data);
        }
    }
    file.close();

    return results;
}

// Modify entry in data file
bool DataFileHandler::modify(const QString &fileName, const char searchFilename[260], const ApplicationData &data)
{
    FileHeader fileHeader;
    ApplicationData result;
    bool found = false;

    std::fstream file(fileName.toStdString(), std::ios::binary | std::ios::in | std::ios::out);
    if (!file) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Failed to modify application data file!");
        return false;
    }

    file.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    if (fileHeader.signature != ApplicationDataSignature) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Application data file header signature is incorrect! Please update settings");
        file.close();
        return false;
    }

    while (file.read(reinterpret_cast<char*>(&result), sizeof(result))) {
        if (strcmp(result.fileName, searchFilename) == 0) {
            found = true;
            result = data;

            file.seekp(-sizeof(result), std::ios::cur);
            file.write(reinterpret_cast<char*>(&result), sizeof(result));
            break;
        }
    }
    file.flush();
    file.close();

    return found;
}

bool DataFileHandler::modify(const QString &fileName, const char searchAction[75], const HotkeysData &data)
{
    FileHeader fileHeader;
    HotkeysData result;
    bool found = false;

    std::fstream file(fileName.toStdString(), std::ios::binary | std::ios::in | std::ios::out);
    if (!file) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Failed to read hotkeys data file!");
        return false;
    }

    file.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    if (fileHeader.signature != HotkeysDataSignature) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Hotkeys data file header signature is incorrect! Please update settings");
        file.close();
        return false;
    }

    while (file.read(reinterpret_cast<char*>(&result), sizeof(result))) {
        if (strcmp(result.action, searchAction) == 0) {
            found = true;
            result = data;

            file.seekp(-sizeof(result), std::ios::cur);
            file.write(reinterpret_cast<char*>(&result), sizeof(result));
            break;
        }
    }
    file.flush();
    file.close();

    return found;
}

bool DataFileHandler::modify(const QString &fileName, const int searchModifier, const int searchKeyCode, const HotkeysData &data)
{
    FileHeader fileHeader;
    HotkeysData result;
    bool found = false;

    std::fstream file(fileName.toStdString(), std::ios::binary | std::ios::in | std::ios::out);
    if (!file) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Failed to read hotkeys data file!");
        return false;
    }

    file.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    if (fileHeader.signature != HotkeysDataSignature) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Hotkeys data file header signature is incorrect! Please update settings");
        file.close();
        return false;
    }

    while (file.read(reinterpret_cast<char*>(&result), sizeof(result))) {
        if (result.modifier == searchModifier && result.keyCode == searchKeyCode) {
            found = true;
            result = data;

            file.seekp(-sizeof(result), std::ios::cur);
            file.write(reinterpret_cast<char*>(&result), sizeof(result));
            break;
        }
    }
    file.flush();
    file.close();

    return found;
}

bool DataFileHandler::modify(const QString &fileName, const QString &searchFilename, const MarkersData &data)
{
    FileHeader fileHeader;
    MarkersData result;
    bool found = false;

    std::fstream file(fileName.toStdString(), std::ios::binary | std::ios::in | std::ios::out);
    if (!file) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Failed to read markers data file!");
        return false;
    }

    file.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    if (fileHeader.signature != MarkersDataSignature) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Markers data file header signature is incorrect! Please update settings");
        file.close();
        return false;
    }

    while (file.read(reinterpret_cast<char*>(&result), sizeof(result))) {
        if (strcmp(result.fileName.toStdString().c_str(), searchFilename.toStdString().c_str()) == 0) {
            found = true;
            result = data;

            file.seekp(-sizeof(result), std::ios::cur);
            file.write(reinterpret_cast<char*>(&result), sizeof(result));
            break;
        }
    }
    file.flush();
    file.close();

    return found;
}

bool DataFileHandler::modify(const QString &fileName, const char searchFilename[260], const PlaylistData &data)
{
    FileHeader fileHeader;
    PlaylistData result;
    bool found = false;

    std::fstream file(fileName.toStdString(), std::ios::binary | std::ios::in | std::ios::out);
    if (!file) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Failed to read playlist data file!");
        return false;
    }

    file.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    if (fileHeader.signature != PlaylistDataSignature) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Playlist data file header signature is incorrect! Please update settings");
        file.close();
        return false;
    }

    while (file.read(reinterpret_cast<char*>(&result), sizeof(result))) {
        if (strcmp(result.fileName, searchFilename) == 0) {
            found = true;
            result = data;

            file.seekp(-sizeof(result), std::ios::cur);
            file.write(reinterpret_cast<char*>(&result), sizeof(result));
            break;
        }
    }
    file.flush();
    file.close();

    return found;
}

bool DataFileHandler::modify(const QString &fileName, const char searchProjectname[260], const ProjectData &data)
{
    FileHeader fileHeader;
    ProjectData result;
    bool found = false;

    std::fstream file(fileName.toStdString(), std::ios::binary | std::ios::in | std::ios::out);
    if (!file) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Failed to read project data file!");
        return false;
    }

    file.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    if (fileHeader.signature != ProjectDataSignature) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Project data file header signature is incorrect! Please update settings");
        file.close();
        return false;
    }

    while (file.read(reinterpret_cast<char*>(&result), sizeof(result))) {
        if (strcmp(result.projectName, searchProjectname) == 0) {
            found = true;
            result = data;

            file.seekp(-sizeof(result), std::ios::cur);
            file.write(reinterpret_cast<char*>(&result), sizeof(result));
            break;
        }
    }
    file.flush();
    file.close();

    return found;
}


// Delete entry in data file
bool DataFileHandler::deleteEntryInApplicationDataFile(const QString &fileName, const char searchFilename[260])
{
    FileHeader fileHeader;
    ApplicationData result;
    bool found = false;
    QString tempFileLocation = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/temp.vdt";

    std::ifstream file(fileName.toStdString(), std::ios::binary);
    std::ofstream tempFile(tempFileLocation.toStdString(), std::ios::binary);
    if (!file) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Failed to modify application data file!");
        return false;
    }
    if (!tempFile) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Failed to create temporary application data file!");
        return false;
    }

    file.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    if (fileHeader.signature != ApplicationDataSignature) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Application data file header signature is incorrect! Please update settings");
        file.close();
        return false;
    }

    tempFile.write(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));

    while (file.read(reinterpret_cast<char*>(&result), sizeof(result))) {
        if (strcmp(result.fileName, searchFilename) != 0) {
            tempFile.write(reinterpret_cast<char*>(&result), sizeof(result));

        } else {
            found = true;
        }
    }
    tempFile.flush();
    tempFile.close();
    file.close();

    remove(fileName.toStdString().c_str());
    rename("temp.vdt", fileName.toStdString().c_str());

    return found;
}

bool DataFileHandler::deleteEntryInHotkeysDataFile(const QString &fileName, const char searchAction[75])
{
    FileHeader fileHeader;
    HotkeysData result;
    bool found = false;
    QString tempFileLocation = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/temp.vhk";

    std::ifstream file(fileName.toStdString(), std::ios::binary);
    std::ofstream tempFile(tempFileLocation.toStdString(), std::ios::binary);
    if (!file) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Failed to modify hotkeys data file!");
        return false;
    }
    if (!tempFile) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Failed to create temporary hotkeys data file!");
        return false;
    }

    file.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    if (fileHeader.signature != HotkeysDataSignature) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Hotkeys data file header signature is incorrect! Please update settings");
        file.close();
        return false;
    }

    tempFile.write(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));

    while (file.read(reinterpret_cast<char*>(&result), sizeof(result))) {
        if (strcmp(result.action, searchAction) != 0) {
            tempFile.write(reinterpret_cast<char*>(&result), sizeof(result));

        } else {
            found = true;
        }
    }
    tempFile.flush();
    tempFile.close();
    file.close();

    remove(fileName.toStdString().c_str());
    rename("temp.vhk", fileName.toStdString().c_str());

    return found;
}

bool DataFileHandler::deleteEntryInHotkeysDataFile(const QString &fileName, int searchModifier, int searchKeyCode)
{
    FileHeader fileHeader;
    HotkeysData result;
    bool found = false;
    QString tempFileLocation = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/temp.vhk";

    std::ifstream file(fileName.toStdString(), std::ios::binary);
    std::ofstream tempFile(tempFileLocation.toStdString(), std::ios::binary);
    if (!file) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Failed to modify hotkeys data file!");
        return false;
    }
    if (!tempFile) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Failed to create temporary hotkeys data file!");
        return false;
    }

    file.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    if (fileHeader.signature != HotkeysDataSignature) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Hotkeys data file header signature is incorrect! Please update settings");
        file.close();
        return false;
    }

    tempFile.write(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));

    while (file.read(reinterpret_cast<char*>(&result), sizeof(result))) {
        if (result.modifier != searchModifier || result.keyCode != searchKeyCode) {
            tempFile.write(reinterpret_cast<char*>(&result), sizeof(result));

        } else {
            found = true;
        }
    }
    tempFile.flush();
    tempFile.close();
    file.close();

    remove(fileName.toStdString().c_str());
    rename("temp.vhk", fileName.toStdString().c_str());

    return found;
}

bool DataFileHandler::deleteEntryInMarkersDataFile(const QString &fileName, const QString &searchFilename)
{
    FileHeader fileHeader;
    MarkersData result;
    bool found = false;
    QString tempFileLocation = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/temp.vvm";

    std::ifstream file(fileName.toStdString(), std::ios::binary);
    std::ofstream tempFile(tempFileLocation.toStdString(), std::ios::binary);
    if (!file) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Failed to modify markers data file!");
        return false;
    }
    if (!tempFile) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Failed to create temporary markers data file!");
        return false;
    }

    file.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    if (fileHeader.signature != MarkersDataSignature) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Markers data file header signature is incorrect! Please update settings");
        file.close();
        return false;
    }

    tempFile.write(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));

    while (file.read(reinterpret_cast<char*>(&result), sizeof(result))) {
        if (strcmp(result.fileName.toStdString().c_str(), searchFilename.toStdString().c_str()) != 0) {
            tempFile.write(reinterpret_cast<char*>(&result), sizeof(result));

        } else {
            found = true;
        }
    }
    tempFile.flush();
    tempFile.close();
    file.close();

    remove(fileName.toStdString().c_str());
    rename("temp.vvm", fileName.toStdString().c_str());

    return found;
}

/*
bool DataFileHandler::deleteEntryInMarkersDataFile(const QString &fileName, const char searchMarkersType[75], const char searchFilename[260])
{
    FileHeader fileHeader;
    MarkersData result;
    bool found = false;
    QString tempFileLocation = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/temp.vvm";

    std::ifstream file(fileName.toStdString(), std::ios::binary);
    std::ofstream tempFile(tempFileLocation.toStdString(), std::ios::binary);
    if (!file) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Failed to modify markers data file!");
        return false;
    }
    if (!tempFile) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Failed to create temporary markers data file!");
        return false;
    }

    file.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    if (fileHeader.signature != ApplicationDataSignature) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Markers data file header signature is incorrect! Please update settings");
        file.close();
        return false;
    }

    tempFile.write(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));

    while (file.read(reinterpret_cast<char*>(&result), sizeof(result))) {
        if (strcmp(result.markerType, searchMarkersType) != 0) {
            if (searchFilename[0] != '\0') {
                if (strcmp(result.fileName, searchFilename) != 0) {
                    tempFile.write(reinterpret_cast<char*>(&result), sizeof(result));
                } else {
                    tempFile.write(reinterpret_cast<char*>(&result), sizeof(result));
                }
            } else {
                found = true;
            }
        }
    }
    tempFile.flush();
    tempFile.close();
    file.close();

    remove(fileName.toStdString().c_str());
    rename("temp.vvm", fileName.toStdString().c_str());

    return found;
}
*/

bool DataFileHandler::deleteEntryInPlaylistDataFile(const QString &fileName, const char searchFilename[260])
{
    FileHeader fileHeader;
    PlaylistData result;
    bool found = false;
    QString tempFileLocation = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/temp.vpl";

    std::ifstream file(fileName.toStdString(), std::ios::binary);
    std::ofstream tempFile(tempFileLocation.toStdString(), std::ios::binary);
    if (!file) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Failed to modify playlist data file!");
        return false;
    }
    if (!tempFile) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Failed to create temporary playlist data file!");
        return false;
    }

    file.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    if (fileHeader.signature != PlaylistDataSignature) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Playlist data file header signature is incorrect! Please update settings");
        file.close();
        return false;
    }

    tempFile.write(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));

    while (file.read(reinterpret_cast<char*>(&result), sizeof(result))) {
        if (strcmp(result.fileName, searchFilename) != 0) {
            tempFile.write(reinterpret_cast<char*>(&result), sizeof(result));

        } else {
            found = true;
        }
    }
    tempFile.flush();
    tempFile.close();
    file.close();

    remove(fileName.toStdString().c_str());
    rename("temp.vpl", fileName.toStdString().c_str());

    return found;
}

bool DataFileHandler::deleteEntryInProjectDataFile(const QString &fileName, const char searchProjectname[260])
{
    FileHeader fileHeader;
    ProjectData result;
    bool found = false;
    QString tempFileLocation = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/temp.vprj";

    std::ifstream file(fileName.toStdString(), std::ios::binary);
    std::ofstream tempFile(tempFileLocation.toStdString(), std::ios::binary);
    if (!file) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Failed to modify project data file!");
        return false;
    }
    if (!tempFile) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Failed to create temporary project data file!");
        return false;
    }

    file.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    if (fileHeader.signature != ProjectDataSignature) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Project data file header signature is incorrect! Please update settings");
        file.close();
        return false;
    }

    tempFile.write(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));

    while (file.read(reinterpret_cast<char*>(&result), sizeof(result))) {
        if (strcmp(result.projectName, searchProjectname) != 0) {
            tempFile.write(reinterpret_cast<char*>(&result), sizeof(result));

        } else {
            found = true;
        }
    }
    tempFile.flush();
    tempFile.close();
    file.close();

    remove(fileName.toStdString().c_str());
    rename("temp.vprj", fileName.toStdString().c_str());

    return found;
}


// Private
bool DataFileHandler::writeToFile(const QString &fileName, ApplicationData data)
{
    FileHeader fileHeader;
    fileHeader.signature = ApplicationDataSignature;

    std::ofstream file(fileName.toStdString(), std::ios::binary);
    if (!file) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Failed to write to application data file. Application data will not be saved!");
        return false;
    }

    file.write(reinterpret_cast<char *>(&fileHeader), sizeof(fileHeader));
    file.write(reinterpret_cast<char *>(&data), sizeof(data));
    file.flush();
    file.close();

    return true;
}

bool DataFileHandler::writeToFile(const QString &fileName, HotkeysData data)
{
    FileHeader fileHeader;
    fileHeader.signature = HotkeysDataSignature;

    std::ofstream file(fileName.toStdString(), std::ios::binary);

    if (!file) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Failed to write to hotkeys data file. Hotkeys data will not be saved!");
        return false;
    }

    file.write(reinterpret_cast<char *>(&fileHeader), sizeof(fileHeader));
    file.write(reinterpret_cast<char *>(&data), sizeof(data));
    file.flush();
    file.close();

    return true;
}

bool DataFileHandler::writeToFile(const QString &fileName, MarkersData data)
{
    FileHeader fileHeader;
    fileHeader.signature = MarkersDataSignature;

    std::ofstream file(fileName.toStdString(), std::ios::binary);

    if (!file) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Failed to write to markers data file. Markers data will not be saved!");
        return false;
    }

    file.write(reinterpret_cast<char *>(&fileHeader), sizeof(fileHeader));
    file.write(reinterpret_cast<char *>(&data), sizeof(data));
    file.flush();
    file.close();

    return true;
}

bool DataFileHandler::writeToFile(const QString &fileName, PlaylistData data)
{
    FileHeader fileHeader;
    fileHeader.signature = PlaylistDataSignature;

    std::ofstream file(fileName.toStdString(), std::ios::binary);

    if (!file) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Failed to write to Playlist file. Playlist will not be saved!");
        return false;
    }

    file.write(reinterpret_cast<char *>(&fileHeader), sizeof(fileHeader));
    file.write(reinterpret_cast<char *>(&data), sizeof(data));
    file.flush();
    file.close();

    return true;
}

bool DataFileHandler::writeToFile(const QString &fileName, ProjectData data)
{
    FileHeader fileHeader;
    fileHeader.signature = ProjectDataSignature;

    std::ofstream file(fileName.toStdString(), std::ios::binary);

    if (!file) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Failed to write to project data file. Project data will not be saved!");
        return false;
    }

    file.write(reinterpret_cast<char *>(&fileHeader), sizeof(fileHeader));
    file.write(reinterpret_cast<char *>(&data), sizeof(data));
    file.flush();
    file.close();

    return true;
}

bool DataFileHandler::appendToFile(const QString &fileName, ApplicationData data)
{
    std::ofstream file(fileName.toStdString(), std::ios::binary | std::ios::app);

    if (!file) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Failed to write to application data file. Application data will not be saved!");
        return false;
    }

    file.write(reinterpret_cast<char *>(&data), sizeof(data));
    file.flush();
    file.close();

    return true;
}

bool DataFileHandler::appendToFile(const QString &fileName, HotkeysData data)
{
    std::ofstream file(fileName.toStdString(), std::ios::binary | std::ios::app);

    if (!file) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Failed to write to hotkeys data file. Hotkeys data will not be saved!");
        return false;
    }

    file.write(reinterpret_cast<char *>(&data), sizeof(data));
    file.flush();
    file.close();

    return true;
}

bool DataFileHandler::appendToFile(const QString &fileName, MarkersData data)
{
    std::ofstream file(fileName.toStdString(), std::ios::binary | std::ios::app);

    if (!file) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Failed to write to markers data file. Markers data will not be saved!");
        return false;
    }

    file.write(reinterpret_cast<char *>(&data), sizeof(data));
    file.flush();
    file.close();

    return true;
}

bool DataFileHandler::appendToFile(const QString &fileName, PlaylistData data)
{
    std::ofstream file(fileName.toStdString(), std::ios::binary | std::ios::app);

    if (!file) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Failed to write to Playlist file. Playlist will not be saved!");
        return false;
    }

    file.write(reinterpret_cast<char *>(&data), sizeof(data));
    file.flush();
    file.close();

    return true;
}

bool DataFileHandler::appendToFile(const QString &fileName, ProjectData data)
{
    std::ofstream file(fileName.toStdString(), std::ios::binary | std::ios::app);

    if (!file) {
        VMessageBox::warning(nullptr, "Vura Data Handler", "Failed to write to project data file. Project data will not be saved!");
        return false;
    }

    file.write(reinterpret_cast<char *>(&data), sizeof(data));
    file.flush();
    file.close();

    return true;
}
