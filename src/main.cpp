#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <conio.h>
#include "Journal.h"

// Функция для обрезки пробелов по краям (требование ТЗ)
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

int main() {
    // Установка кодировки UTF-8 для корректного отображения кириллицы в консоли Windows
    std::system("chcp 65001 > nul");
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // Указываем путь "../journal.xlsx", чтобы файл сохранялся в папку ticket_generator (рядом с папкой build)
    Journal journal("../journal.csv");
    journal.load();

    std::cout << "=== Генератор экзаменационных билетов ===\n";
    std::cout << "Для выхода нажмите ESC.\n\n";

    while (true) {
        std::string lastName, firstName;

        // Ввод Last name с перехватом ESC и проверкой на пустую строку
        while (true) {
            std::cout << "Last name: ";
            int ch = _getch();
            if (ch == 27) { // Нажатие ESC
                std::cout << "\nВыход из программы.\n";
                return 0;
            }
            if (ch == '\r' || ch == '\n') {
                std::cout << "\nОшибка: пустая строка не принимается.\n";
                continue;
            }

            std::cout << static_cast<char>(ch);
            std::string rest;
            std::getline(std::cin, rest);
            lastName = trim(static_cast<char>(ch) + rest);

            if (!lastName.empty()) break;
            std::cout << "Ошибка: пустая строка не принимается.\n";
        }

        // Ввод First name с проверкой на пустую строку
        while (true) {
            std::cout << "First name: ";
            std::getline(std::cin, firstName);
            firstName = trim(firstName);
            if (!firstName.empty()) break;
            std::cout << "Ошибка: пустая строка не принимается.\n";
        }

        // Генерация номера билета от 1 до 20
        int ticketNum = (std::rand() % 20) + 1;
        std::cout << "Билет № " << ticketNum << ".\n";

        // Проверка блокировки файла (если открыт в Excel)[cite: 1]
        while (journal.isFileLocked()) {
            std::cout << "Ошибка: файл journal.xlsx открыт в Excel. Закройте его и нажмите Enter...";
            std::cin.get();
        }

        // Добавление записи и моментальное сохранение в файл[cite: 1]
        journal.addRecord(lastName, firstName, ticketNum);
    }

    return 0;
}