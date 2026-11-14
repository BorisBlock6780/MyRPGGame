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


# 3. Настройки для ImGui
set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/imgui-sfml/imgui")
set(IMGUI_SFML_FIND_SFML OFF CACHE BOOL "" FORCE)

# 4. Подключаем imgui-sfml
add_subdirectory(external/imgui-sfml)

# 5. Основная игра с GUI (SFML + ImGui)
add_executable(MyRPGGame 
    src/main.cpp 
    src/ScreenManager.cpp
    src/Lab3Demo.cpp
)

target_compile_definitions(${PROJECT_NAME} PRIVATE COMBINED_BUILD)

target_include_directories(${PROJECT_NAME} PRIVATE 
    include 
    ${IMGUI_DIR}
)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /utf-8)
endif()

# 6. Линковка основной игры
target_link_libraries(${PROJECT_NAME} PRIVATE 
    sfml-graphics 
    sfml-window 
    sfml-system 
    ImGui-SFML::ImGui-SFML
)

# 7. Консольная программа демонстрации Лабораторной работы 3 (Часть 2: Умные указатели)
add_executable(Lab3Demo 
    src/Lab3Demo.cpp
)

target_include_directories(Lab3Demo PRIVATE 
    include
)

if(MSVC)
    target_compile_options(Lab3Demo PRIVATE /utf-8)
endif()
