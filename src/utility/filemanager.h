#pragma once

#include <QObject>
#include <QMap>
#include <QDebug>

#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>


using namespace std;


struct HKEYHeader {
    uint32_t signature = 0xE8EBE5F9;
    uint32_t version = 0x31;
    uint32_t data_size;
    uint32_t checksum;
};

struct HMRKHeader {
    uint32_t signature = 0xE8EDF2EB;
    uint32_t version = 0x31;
    uint32_t data_size;
    uint32_t checksum;
};

struct HPLHeader {
    uint32_t signature = 0x80E8F0EC;
    uint32_t version = 0x31;
    uint32_t data_size;
    uint32_t checksum;
};

struct HPRJHeader {
    uint32_t signature = 0xE8F0F2EA;
    uint32_t version = 0x31;
    uint32_t data_size;
    uint32_t checksum;
};

struct HKEYDataEntry {
    string key;
    string value;
};

struct HMRKDataEntry {
    uint8_t key;
    uint32_t value;
};

struct HPLDataEntry {
    uint8_t key;
    uint32_t value;
};

struct HPRJDataEntry {
    uint8_t key;
    uint32_t value;
};

class FileManager : public QObject
{
    Q_OBJECT

public:
    explicit FileManager(QObject *parent = nullptr);

    QMap<QString, QString> readHotkeyFile(const char *filename);
    bool writeHotkeyFile(const char *filename, const QMap<QString, QString> &hotkeys);

private:
    uint32_t calculateHeaderChecksum(const HKEYHeader &header);
    uint32_t calculateHeaderChecksum(const HMRKHeader &header);
    uint32_t calculateHeaderChecksum(const HPLHeader &header);
    uint32_t calculateHeaderChecksum(const HPRJHeader &header);

};
