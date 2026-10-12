#pragma once
#include <string>
#include <map>
#include <vector>
#include <cstdint>

class ZipArchive {
public:
    // Добавить файл в виртуальный архив (путь внутри архива -> содержимое)
    void addFile(const std::string& internalPath, const std::string& content);
    
    // Собрать архив и записать его на диск под именем filename
    bool save(const std::string& filename);

    // Статический метод для распаковки/чтения файлов из существующего xlsx архива
    static bool extractFile(const std::string& zipFilename, const std::string& internalPath, std::string& outContent);

private:
    std::map<std::string, std::string> files_;
    
    // Вспомогательные структуры для формирования заголовков ZIP
    struct LocalHeader;
    struct CentralDirectoryHeader;
};