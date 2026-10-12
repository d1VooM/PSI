#include "Journal.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <chrono>
#include <iomanip>

Journal::Journal(std::string filename) : filename_(std::move(filename)) {}

static std::string getCurrentDateTime() {
    auto now = std::chrono::system_clock::now();
    std::time_t in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// Вспомогательная функция для удаления кавычек по краям строки при чтении
static std::string trimQuotes(const std::string& str) {
    if (str.size() >= 2 && str.front() == '"' && str.back() == '"') {
        return str.substr(1, str.size() - 2);
    }
    return str;
}

void Journal::load() {
    std::ifstream file(filename_);
    if (!file.is_open()) return;

    std::string line;
    bool isFirstLine = true;
    while (std::getline(file, line)) {
        if (isFirstLine) { // Пропускаем шапку таблицы
            isFirstLine = false;
            continue;
        }
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string lastName, firstName, ticketStr, dateTime;

        if (std::getline(ss, lastName, ';') &&
            std::getline(ss, firstName, ';') &&
            std::getline(ss, ticketStr, ';') &&
            std::getline(ss, dateTime)) {
            
            StudentRecord rec;
            rec.lastName = trimQuotes(lastName);
            rec.firstName = trimQuotes(firstName);
            rec.ticketNumber = std::stoi(trimQuotes(ticketStr));
            rec.dateTime = trimQuotes(dateTime);
            records_.push_back(rec);
        }
    }
    file.close();
}

void Journal::addRecord(const std::string& lastName, const std::string& firstName, int ticketNum) {
    StudentRecord rec;
    rec.lastName = lastName;
    rec.firstName = firstName;
    rec.ticketNumber = ticketNum;
    rec.dateTime = getCurrentDateTime();

    records_.push_back(rec);
    saveToFile();
}

bool Journal::isFileLocked() const {
    std::ofstream test(filename_, std::ios::app);
    return !test.is_open();
}

void Journal::saveToFile() {
    std::ofstream file(filename_, std::ios::out | std::ios::binary);
    if (!file.is_open()) return;

    // Записываем BOM (Byte Order Mark) для UTF-8, чтобы Excel корректно отображал кириллицу
    unsigned char bom[] = {0xEF, 0xBB, 0xBF};
    file.write(reinterpret_cast<char*>(bom), 3);

    // Записываем шапку таблицы
    file << "\"Last name\";\"First name\";\"Номер билета\";\"Дата и время\"\n";

    // Записываем все накопленные записи в кавычках для защиты структуры колонок
    for (const auto& rec : records_) {
        file << "\"" << rec.lastName << "\";"
             << "\"" << rec.firstName << "\";"
             << rec.ticketNumber << ";"
             << "\"" << rec.dateTime << "\"\n";
    }
    file.close();
}