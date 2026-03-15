#include "filemanager.h"

FileManager::FileManager(QObject *parent) : QObject(parent) {}

QMap<QString, QString> FileManager::readHotkeyFile(const char *filename)
{
    qDebug() << "Reading hotkey file: " << filename;
    QMap<QString, QString> hotkeys;
    vector<HKEYDataEntry> entries;
    uint32_t _signature = 0xE8EBE5F9;

    ifstream stream(filename, ios::binary);
    if (stream.bad()) {
        qWarning() << "Could not open file for reading: " << filename;
        return hotkeys;
    }

    HKEYHeader header;
    stream.read(reinterpret_cast<char*>(&header), sizeof(HKEYHeader));

    HKEYDataEntry entry;
    while (stream.read(reinterpret_cast<char*>(&entry), sizeof(HKEYDataEntry))) {
        entries.push_back(entry);
    }
    stream.close();

    if (header.signature != _signature) {
        qWarning() << "HKey file signature mismatch";
        return hotkeys;
    }

    uint32_t computed = calculateHeaderChecksum(header);
    if (computed != header.checksum) {
        qWarning() << "HKey file checksum mismatch";
        return hotkeys;
    }

    for (int i = 0; i < entries.size(); ++i) {
        hotkeys[QString::fromStdString(entries[i].key)] = QString::fromStdString(entries[i].value);
    }
    return hotkeys;
}

bool FileManager::writeHotkeyFile(const char *filename, const QMap<QString, QString> &hotkeys)
{
    qDebug() << "Writing hotkey file: " << filename;
    ofstream stream(filename, ios::binary);
    if (stream.bad()) {
        qWarning() << "Could not open file for writing: " << filename;
        return false;
    }

    HKEYHeader header;
    header.data_size = sizeof(HKEYDataEntry);
    header.checksum = calculateHeaderChecksum(header);
    stream.write(reinterpret_cast<char*>(&header), sizeof(HKEYHeader));

    for (auto i = hotkeys.cbegin(); i != hotkeys.cend(); ++i) {
        HKEYDataEntry entry;
        entry.key = i.key().toStdString();
        entry.value = i.value().toStdString();
        stream.write(reinterpret_cast<char*>(&entry), sizeof(HKEYDataEntry));
    }
    stream.close();
    return true;
}

uint32_t FileManager::calculateHeaderChecksum(const HKEYHeader &header)
{
    const uint8_t* p = reinterpret_cast<const uint8_t*>(&header);
    uint32_t sum = 0;
    for (size_t i = 0; i < sizeof(HKEYHeader) - sizeof(uint32_t); ++i) {
        sum += p[i];
    }
    return sum;
}

uint32_t FileManager::calculateHeaderChecksum(const HMRKHeader &header)
{
    const uint8_t* p = reinterpret_cast<const uint8_t*>(&header);
    uint32_t sum = 0;
    for (size_t i = 0; i < sizeof(HMRKHeader) - sizeof(uint32_t); ++i) {
        sum += p[i];
    }
    return sum;
}

uint32_t FileManager::calculateHeaderChecksum(const HPLHeader &header)
{
    const uint8_t* p = reinterpret_cast<const uint8_t*>(&header);
    uint32_t sum = 0;
    for (size_t i = 0; i < sizeof(HPLHeader) - sizeof(uint32_t); ++i) {
        sum += p[i];
    }
    return sum;
}

uint32_t FileManager::calculateHeaderChecksum(const HPRJHeader &header)
{
    const uint8_t* p = reinterpret_cast<const uint8_t*>(&header);
    uint32_t sum = 0;
    for (size_t i = 0; i < sizeof(HPRJHeader) - sizeof(uint32_t); ++i) {
        sum += p[i];
    }
    return sum;
}
