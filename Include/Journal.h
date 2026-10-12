#pragma once
#include <string>
#include <vector>

struct StudentRecord {
    std::string lastName;
    std::string firstName;
    int ticketNumber;
    std::string dateTime;
};

class Journal {
public:
    explicit Journal(std::string filename);

    // Загрузить существующие записи из файла (если он есть)
    void load();

    // Добавить нового студента в память и сразу сохранить в файл
    void addRecord(const std::string& lastName, const std::string& firstName, int ticketNum);

    // Проверить, заблокирован ли файл (например, открыт в Excel)
    bool isFileLocked() const;

private:
    std::string filename_;
    std::vector<StudentRecord> records_;

    // Генерация структуры OpenXML для Excel
    void saveToFile();
    std::string generateContentTypesXML() const;
    std::string generateRelsXML() const;
    std::string generateWorkbookXML() const;
    std::string generateStylesXML() const;
    std::string generateSheetXML() const;
};