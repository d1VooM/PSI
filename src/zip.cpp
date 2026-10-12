#include "Zip.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <cstdint>

// Простая реализация CRC32 для валидности заголовков ZIP
static uint32_t calculate_crc32(const std::string& data) {
    uint32_t crc = 0xFFFFFFFF;
    for (char c : data) {
        crc ^= static_cast<uint8_t>(c);
        for (int j = 0; j < 8; j++) {
            crc = (crc >> 1) ^ (-(crc & 1) & 0xEDB88320);
        }
    }
    return ~crc;
}

void ZipArchive::addFile(const std::string& internalPath, const std::string& content) {
    files_[internalPath] = content;
}

bool ZipArchive::save(const std::string& filename) {
    std::ofstream out(filename, std::ios::binary);
    if (!out.is_open()) return false;

    struct FileEntryInfo {
        std::string name;
        uint32_t crc;
        uint32_t size;
        uint32_t offset;
    };

    std::vector<FileEntryInfo> entries;

    // 1. Запись локальных заголовков и содержимого файлов
    for (const auto& pair : files_) {
        const std::string& name = pair.first;
        const std::string& data = pair.second;

        uint32_t crc = calculate_crc32(data);
        uint32_t size = static_cast<uint32_t>(data.size());
        uint32_t offset = static_cast<uint32_t>(out.tellp());

        entries.push_back({name, crc, size, offset});

        uint32_t sig = 0x04034b50;
        uint16_t version = 20;
        uint16_t flags = 0;
        uint16_t method = 0; // Store (без сжатия)
        uint16_t modTime = 0x0000;
        uint16_t modDate = 0x0021;
        uint16_t nameLen = static_cast<uint16_t>(name.size());
        uint16_t extraLen = 0;

        out.write(reinterpret_cast<const char*>(&sig), 4);
        out.write(reinterpret_cast<const char*>(&version), 2);
        out.write(reinterpret_cast<const char*>(&flags), 2);
        out.write(reinterpret_cast<const char*>(&method), 2);
        out.write(reinterpret_cast<const char*>(&modTime), 2);
        out.write(reinterpret_cast<const char*>(&modDate), 2);
        out.write(reinterpret_cast<const char*>(&crc), 4);
        out.write(reinterpret_cast<const char*>(&size), 4);
        out.write(reinterpret_cast<const char*>(&size), 4);
        out.write(reinterpret_cast<const char*>(&nameLen), 2);
        out.write(reinterpret_cast<const char*>(&extraLen), 2);
        out.write(name.data(), nameLen);
        out.write(data.data(), size);
    }

    uint32_t centralDirOffset = static_cast<uint32_t>(out.tellp());
    uint32_t centralDirSize = 0;

    // 2. Запись Central Directory
    for (const auto& entry : entries) {
        uint32_t sig = 0x02014b50;
        uint16_t versionMadeBy = 20;
        uint16_t versionNeeded = 20;
        uint16_t flags = 0;
        uint16_t method = 0;
        uint16_t modTime = 0x0000;
        uint16_t modDate = 0x0021;
        uint16_t nameLen = static_cast<uint16_t>(entry.name.size());
        uint16_t extraLen = 0;
        uint16_t commentLen = 0;
        uint16_t diskStart = 0;
        uint16_t internalAttr = 0;
        uint32_t externalAttr = 0;
        uint32_t entryCrc = entry.crc;
        uint32_t entrySize = entry.size;
        uint32_t entryOffset = entry.offset;

        out.write(reinterpret_cast<const char*>(&sig), 4);
        out.write(reinterpret_cast<const char*>(&versionMadeBy), 2);
        out.write(reinterpret_cast<const char*>(&versionNeeded), 2);
        out.write(reinterpret_cast<const char*>(&flags), 2);
        out.write(reinterpret_cast<const char*>(&method), 2);
        out.write(reinterpret_cast<const char*>(&modTime), 2);
        out.write(reinterpret_cast<const char*>(&modDate), 2);
        out.write(reinterpret_cast<const char*>(&entryCrc), 4);
        out.write(reinterpret_cast<const char*>(&entrySize), 4);
        out.write(reinterpret_cast<const char*>(&entrySize), 4);
        out.write(reinterpret_cast<const char*>(&nameLen), 2);
        out.write(reinterpret_cast<const char*>(&extraLen), 2);
        out.write(reinterpret_cast<const char*>(&commentLen), 2);
        out.write(reinterpret_cast<const char*>(&diskStart), 2);
        out.write(reinterpret_cast<const char*>(&internalAttr), 2);
        out.write(reinterpret_cast<const char*>(&externalAttr), 4);
        out.write(reinterpret_cast<const char*>(&entryOffset), 4);
        out.write(entry.name.data(), nameLen);
    }

    uint32_t centralDirEnd = static_cast<uint32_t>(out.tellp());
    centralDirSize = centralDirEnd - centralDirOffset;

    // 3. Запись End of Central Directory Record
    uint32_t eocdSig = 0x06054b50;
    uint16_t diskNum = 0;
    uint16_t centralDirDisk = 0;
    uint16_t recordsOnDisk = static_cast<uint16_t>(entries.size());
    uint16_t totalRecords = static_cast<uint16_t>(entries.size());
    uint32_t cdSize = centralDirSize;
    uint32_t cdOffset = centralDirOffset;
    uint16_t commentLen = 0;

    out.write(reinterpret_cast<const char*>(&eocdSig), 4);
    out.write(reinterpret_cast<const char*>(&diskNum), 2);
    out.write(reinterpret_cast<const char*>(&centralDirDisk), 2);
    out.write(reinterpret_cast<const char*>(&recordsOnDisk), 2);
    out.write(reinterpret_cast<const char*>(&totalRecords), 2);
    out.write(reinterpret_cast<const char*>(&cdSize), 4);
    out.write(reinterpret_cast<const char*>(&cdOffset), 4);
    out.write(reinterpret_cast<const char*>(&commentLen), 2);

    out.close();
    return true;
}

bool ZipArchive::extractFile(const std::string&, const std::string&, std::string&) {
    return false;
}