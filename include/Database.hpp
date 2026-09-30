#pragma once
#include <string>
#include <unordered_map>
#include <array>
#include <vector>
#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>

using StatArray = std::array<double, 8>;

namespace StatIndex {
    enum {
        HP = 0,
        Mana = 1,
        Attack = 2,
        Defense = 3,
        CritChance = 4,
        CritDamage = 5,
        Vampirism = 6,
        DamageBonus = 7
    };
}

enum class WeaponRarity { Common = 1, Uncommon = 2, Rare = 3, Epic = 4, Legendary = 5, Unique = 6 };

enum class ItemCategory {
    Consumable, // Яблоки, зелья здоровья, еда
    Weapon,     // Оружие
    Artifact,   // Артефакты / Браслеты
    Material,   // Ресурсы, руда, свитки
    Quest       // Квестовые предметы 
};

struct ItemTemplate {
    std::string id;          // "apple", "hp_potion"
    std::string name;        // "Яблоко"
    ItemCategory category;   // ItemCategory::Consumable
    std::string iconPath;    // "assets/icons/apple.png"
    int maxStack = 99;       // Максимальное количество в слоте
    sf::Texture iconTexture;
    ImTextureID getImGuiTextureID() const {
        return (ImTextureID)(uintptr_t)iconTexture.getNativeHandle();
    }
};

struct WeaponTemplate {
    std::string id;
    std::string name;
    WeaponRarity rarity;
    double baseDamage;        // Базовый урон
    StatArray bonusStats;     // Массив доп. характеристик
    std::string effectDesc;   // Описание спец-эффекта (для 4*+)
};

// --- Структура для формулы умения ---
struct SkillTemplate {
    std::string id;          // "fireball"
    std::string name;        // "Огненная Стрела"
    int scalingStatIndex;    // Какой стат берем за основу (например, StatIndex::Attack или StatIndex::Mana)
    double scalingRatio;     // Множитель (0.60 = 60%, 0.50 = 50%)
    double flatBonus;        // Плоская прибавка к урону (например, +20 единиц)
    int cooldown;            // Кулдаун в ходах

    // Метод расчета итогового значения урона/лечения
    double calculateValue(const StatArray& stats) const {
        if (scalingStatIndex < 0 || scalingStatIndex >= 8) return flatBonus;

        // Значение стата * Множитель + Базовый урон
        double baseValue = (stats[scalingStatIndex] * scalingRatio) + flatBonus;

        // Если урон скалируется от Атаки — учитываем общий Бонус Урона персонажа
        if (scalingStatIndex == StatIndex::Attack) {
            baseValue *= (1.0 + stats[StatIndex::DamageBonus]);
        }

        return baseValue;
    }
};

// --- Шаблон Персонажа ---
struct CharacterTemplate {
    std::string id;
    std::string name;
    StatArray baseStats;
    StatArray growthPerLevel;
    std::vector<SkillTemplate> skills; // Список умений персонажа
};

class GameDatabase {
public:
    static GameDatabase& getInstance() {
        static GameDatabase instance;
        return instance;
    }

    std::unordered_map<std::string, CharacterTemplate> characters;

    void init() {
        // Задаем персонажа Адеир с его навыками
        characters["adeir"] = {
            "adeir",
            "Адеир",
            { 110.0, 90.0, 18.0, 14.0, 0.05, 1.50, 0.0, 0.0 }, // Base Stats
            { 12.0,  10.0,  3.0,  2.0, 0.00, 0.00, 0.0, 0.0 }, // Growth
            {
                // Навык 0: Обычная атака (60% от Атаки)
                { "basic_atk", "Обычная атака", StatIndex::Attack, 0.60, 0.0, 0 },

                // Навык 1: Взрыв Магии (50% от Маны)
                { "magic_blast", "Взрыв Магии", StatIndex::Mana, 0.50, 10.0, 2 },

                // Навык 2: Щит от Защиты (80% от Защиты)
                { "shield", "Каменный Щит", StatIndex::Defense, 0.80, 5.0, 3 }
            }
        };
        weapons["iron_sword"] = {
            "iron_sword",
            "Стальной Меч",
            WeaponRarity::Common,
            25.0, // baseDamage -> идет в Атаку
            //   HP,   MP,  ATK,  DEF, Crit%, CritDmg, Vamp, DmgBonus
            {   0.0,  0.0,  0.0,  0.0,  0.10,    0.0,   0.0,    0.0 }, // +10% Крита
            ""
        };

        // 2. Волшебный посох: даёт Ману и Атаку
        weapons["magic_staff"] = {
            "magic_staff",
            "Посох Ученика",
            WeaponRarity::Rare,
            15.0, // baseDamage
            //   HP,   MP,  ATK,  DEF, Crit%, CritDmg, Vamp, DmgBonus
            {   0.0, 50.0,  0.0,  0.0,   0.0,    0.0,   0.0,    0.0 }, // +50 Маны
            ""
        };
    }
    std::unordered_map<std::string, WeaponTemplate> weapons;
    
    std::unordered_map<std::string, ItemTemplate> items;

    void registerItem(const std::string& id, const std::string& name, ItemCategory category, const std::string& iconPath) {
        ItemTemplate item;
        item.id = id;
        item.name = name;
        item.category = category;
        item.iconPath = iconPath;

        // Автоматически загружаем иконку с диска при старте игры
        if (!item.iconTexture.loadFromFile(iconPath)) {
            // Если иконка не найдена, загружаем дефолтную заглушку или выводим ошибку
            item.iconTexture.loadFromFile("assets/icons/missing.png");
        }

        items[id] = std::move(item);
    }

    void initItems() {
        items["apple"] = { "apple", "Яблоко", ItemCategory::Consumable, "assets/apple.png", 99 };
        items["hp_potion"] = { "hp_potion", "Зелье HP", ItemCategory::Consumable, "assets/potion.png", 99 };
        items["iron_ore"] = { "iron_ore", "Железная руда", ItemCategory::Material, "assets/ore.png", 999 };
    }
};