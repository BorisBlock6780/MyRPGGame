cmake_minimum_required(VERSION 3.14)
project(CourseProject CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Подключаем папку с заголовочными файлами
include_directories(include)

# Собираем все .cpp файлы из папки src
file(GLOB SOURCES "src/*.cpp")

# Создаем исполняемый файл проекта
add_executable(MyProject ${SOURCES})