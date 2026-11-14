#pragma once
#include <string>
#include <vector>
#include <imgui.h>
#include "PlayerData.hpp" // Для ItemStack

// Типы точек взаимодействия на карте
enum class MapNodeType {
    Shop,        // Торговец -> Экран торговли (ScreenState::Shop)
    Combat,      // Сражение -> Экран боя (ScreenState::Combat)
    Cooking,     // Полевой костер -> Экран готовки (ScreenState::Cooking)
    Crafting,    // Полевая мастерская -> Экран создания (ScreenState::Crafting)
    ResourceMine // Древняя рудная жила -> Интерактивная добыча руды (ScreenState::Mining)
};

// Данные о группе врагов для передачи на экран боя
struct EnemySpawnInfo {
    std::string enemyId;   // ID шаблона врага
    std::string enemyName; // Отображаемое имя
    int level = 1;         // Уровень врага
    int count = 1;         // Количество таких врагов в отряде противника
};

// Точка взаимодействия на карте
struct MapPointOfInterest {
    std::string id;            // Уникальный ID точки
    std::string title;         // Короткий текст под кругом
    std::string description;   // Краткая информация во всплывающем окне
    int floor = 1;             // Этаж, к которому привязана точка (1..10)
    ImVec2 position;           // Положение центра круга на экране (задается вручную)
    float radius = 28.0f;      // Радиус круга точки

    MapNodeType type;          // Тип перехода
    std::string iconKey;       // Ключ текстуры иконки ("shop", "combat", "resource" и т.д.)

    bool isTemporary = false;  // Временная ли точка (исчезает после нажатия "Перейти")
    bool isCompleted = false;  // Если true — точка скрыта с карты

    // --- Данные для передачи на экран Торговли (если type == MapNodeType::Shop) ---
    std::string merchantName;
    PlayerInventory merchantInventory{ 500 };

    // --- Данные для передачи на экран Боя (если type == MapNodeType::Combat) ---
    std::vector<EnemySpawnInfo> enemies;
};
