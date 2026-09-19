# MyRPGGame

## Описание проекта
Данный проект представляет собой пошаговую систему боя и управления отрядом персонажей, реализованную на C++ (с возможностью расширения/интеграции).

## Технологический стек
* **Языки программирования:** C++
* **Система сборки:** CMake
* **Контроль версий:** Git / GitHub 

## Структура проекта
```text
├── build/                # Директория сборки (в .gitignore)
├── docs/                 # Документация и материалы курсового проекта
├── include/              # Заголовочные файлы (.h / .hpp)
│   ├── Characters.hpp
│   ├── Weapons.hpp
│   ├── Artifacts.hpp
│   └── SquadManager.hpp
├── src/                  # Исходный код (.cpp)
│   ├── Characters.cpp
│   ├── Weapons.cpp
│   ├── Artifacts.cpp
│   ├── SquadManager.cpp
│   └── main.cpp
├── .gitignore            # Исключения Git
├── CMakeLists.txt        # Конфигурация CMake
└── README.md             # Описание проекта