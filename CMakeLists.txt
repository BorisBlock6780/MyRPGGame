cmake_minimum_required(VERSION 3.16)
project(MyRPGGame CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Загружаем стабильный SFML 2.6.1
include(FetchContent)
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG        2.6.1
)
FetchContent_MakeAvailable(SFML)

# 2. Создаем ALIAS-цели для SFML 2.x
if(NOT TARGET SFML::Graphics)
    add_library(SFML::Graphics ALIAS sfml-graphics)
endif()
if(NOT TARGET SFML::Window)
    add_library(SFML::Window ALIAS sfml-window)
endif()
if(NOT TARGET SFML::System)
    add_library(SFML::System ALIAS sfml-system)
endif()

if (MSVC)
    add_compile_options(/utf-8)
endif()

# 3. Настройки для ImGui
set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/imgui-sfml/imgui")
set(IMGUI_SFML_FIND_SFML OFF CACHE BOOL "" FORCE)

# 4. Подключаем imgui-sfml
add_subdirectory(external/imgui-sfml)

# 5. Исходный код проекта
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.hpp")

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME} PRIVATE 
    include 
    ${IMGUI_DIR}
)

# 6. Линковка
target_link_libraries(${PROJECT_NAME} PRIVATE 
    sfml-graphics 
    sfml-window 
    sfml-system 
    ImGui-SFML::ImGui-SFML
)