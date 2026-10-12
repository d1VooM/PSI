cmake_minimum_required(VERSION 3.10)
project(ticket_generator)

set(CMAKE_CXX_STANDARD 17)

# Подключаем папки с заголовочными файлами
include_directories(include)

# Создаем исполняемый файл из всех модулей
add_executable(ticket_generator
    src/main.cpp
    src/Journal.cpp
    src/Zip.cpp
)