#pragma once
#include <string>
#include <unordered_map>
#include <array>
#include <vector>
#include <memory>
#include <SFML/Graphics.hpp>
#include <imgui.h>

using StatArray = std::array<double, 8>;

namespace StatIndex {
    enum { HP = 0, Mana = 1, Attack = 2, Defense = 3, CritChance = 4, CritDamage = 5, Vampirism = 6, DamageBonus = 7 };
}

enum class ItemCategory { Consumable, Weapon, Artifact, Material, Quest };

// ==========================================
// ИЕРАРХИЯ ШАБЛОНОВ ОРУЖИЯ
// ==========================================
class WeaponTemplate {
protected:
    std::string id;
    std::string name;
    double baseDamage;
    int level;
    const int maxLevel = 20;

public:
    WeaponTemplate(std::string id, std::string name, double baseDamage)
        : id(std::move(id)), name(std::move(name)), baseDamage(baseDamage), level(1) {
    }

    virtual ~WeaponTemplate() = default;

    std::string getId() const { return id; }
    std::string getName() const { return name; }
    int getLevel() const { return level; }

    // Динамический расчет урона (+8% за каждый уровень заточки)
    virtual double getBaseDamage() const {
        return baseDamage * (1.0 + (level - 1) * 0.08);
    }

    bool upgrade() {
        if (level >= maxLevel) return false;
        level++;
        return true;
    }
};

// Конкретный предмет: Стальной Меч
class IronSwordTemplate : public WeaponTemplate {
public:
    IronSwordTemplate() : WeaponTemplate("iron_sword", "Стальной Меч", 25.0) {}
};

// Конкретный предмет: Посох Ученика
class MagicStaffTemplate : public WeaponTemplate {
public:
    MagicStaffTemplate() : WeaponTemplate("magic_staff", "Посох Ученика", 15.0) {}
};
// ==========================================
// ИЕРАРХИЯ ШАБЛОНОВ ПРЕДМЕТОВ
// ==========================================
class ItemTemplate {
protected:
    std::string id;
    std::string name;
    ItemCategory category;
    std::string iconPath;
    int maxStack;
    sf::Texture iconTexture;

public:
    ItemTemplate(std::string id, std::string name, ItemCategory category, std::string iconPath, int maxStack = 99)
        : id(std::move(id)), name(std::move(name)), category(category), iconPath(std::move(iconPath)), maxStack(maxStack) {
        if (!iconTexture.loadFromFile(this->iconPath)) {
            iconTexture.loadFromFile("assets/icons/missing.png");
        }
    }
    virtual ~ItemTemplate() = default;

    std::string getId() const { return id; }
    std::string getName() const { return name; }
    ItemCategory getCategory() const { return category; }

    ImTextureID getImGuiTextureID() const {
        return (ImTextureID)(uintptr_t)iconTexture.getNativeHandle();
    }
};

// Конкретный предмет: Яблоко
class AppleTemplate : public ItemTemplate {
public:
    AppleTemplate() : ItemTemplate("apple", "Яблоко", ItemCategory::Consumable, "assets/apple.png", 99) {}
};

// Конкретный предмет: Железная руда
class IronOreTemplate : public ItemTemplate {
public:
    IronOreTemplate() : ItemTemplate("iron_ore", "Железная руда", ItemCategory::Material, "assets/ore.png", 999) {}
};


// ==========================================
// ИЕРАРХИЯ ШАБЛОНОВ УМЕНИЙ
// ==========================================
class SkillTemplate {
protected:
    std::string id;
    std::string name;
    int cooldown;

public:
    SkillTemplate(std::string id, std::string name, int cooldown)
        : id(std::move(id)), name(std::move(name)), cooldown(cooldown) {
    }

    virtual ~SkillTemplate() = default;

    std::string getId() const { return id; }
    std::string getName() const { return name; }
    int getCooldown() const { return cooldown; }

    // Виртуальный метод расчета урона, теперь учитывающий skillStacks
    virtual double calculateValue(const StatArray& stats, int skillStacks) const = 0;
};

// Конкретное умение: Обычная атака Адеира
class AdeirBasicAtkTemplate : public SkillTemplate {
public:
    AdeirBasicAtkTemplate() : SkillTemplate("Adeir_basic_atk", "Обычная атака", 0) {}

    double calculateValue(const StatArray& stats, int skillStacks) const override {
        // Урон: 60% от Атаки + бонусный урон за каждый накопленный стак
        double baseValue = (stats[StatIndex::Attack] * 0.60) + (skillStacks * 15.0);
        return baseValue * (1.0 + stats[StatIndex::DamageBonus]);
    }
};

// Конкретное умение: Взрыв магии Адеира
class AdeirMagicBlastTemplate : public SkillTemplate {
public:
    AdeirMagicBlastTemplate() : SkillTemplate("Adeir_magic_blast", "Взрыв Магии", 2) {}

    double calculateValue(const StatArray& stats, int skillStacks) const override {
        // Урон: 50% от Маны + плоская прибавка, стаки увеличивают процент скейла
        double scaleMult = 0.50 + (skillStacks * 0.05);
        return (stats[StatIndex::Mana] * scaleMult) + 10.0;
    }
};


// ==========================================
// ОБНОВЛЕННЫЙ ШАБЛОН ПЕРСОНАЖА
// ==========================================
class CharacterTemplate {
protected:
    std::string id;
    std::string name;
    StatArray baseStats;
    StatArray growthPerLevel;

    // Хранение умений через указатели для поддержки полиморфизма
    std::vector<std::shared_ptr<SkillTemplate>> skills;

public:
    CharacterTemplate(std::string id, std::string name, StatArray base, StatArray growth, std::vector<std::shared_ptr<SkillTemplate>> skills)
        : id(std::move(id)), name(std::move(name)), baseStats(base), growthPerLevel(growth), skills(std::move(skills)) {
    }

    virtual ~CharacterTemplate() = default;

    std::string getId() const { return id; }
    std::string getName() const { return name; }
    const std::vector<std::shared_ptr<SkillTemplate>>& getSkills() const { return skills; }
    const StatArray& getBaseStats() const { return baseStats; }
    const StatArray& getGrowthPerLevel() const { return growthPerLevel; }
};

// Конкретный шаблон: Адеир
class AdeirTemplate : public CharacterTemplate {
public:
    AdeirTemplate() : CharacterTemplate(
        "adeir",
        "Адеир",
        { 110.0, 90.0, 18.0, 14.0, 0.05, 1.50, 0.0, 0.0 },
        { 12.0,  10.0,  3.0,  2.0, 0.00, 0.00, 0.0, 0.0 },
        {
            std::make_shared<AdeirBasicAtkTemplate>(),
            std::make_shared<AdeirMagicBlastTemplate>()
        }
    ) {
    }
};

// Класс Базы Данных (фрагмент)
class GameDatabase {
public:
    static GameDatabase& getInstance() {
        static GameDatabase instance;
        return instance;
    }

    // Хранилища умных указателей
    std::unordered_map<std::string, std::shared_ptr<CharacterTemplate>> characters;
    std::unordered_map<std::string, std::shared_ptr<WeaponTemplate>> weapons; // <- Возвращаем словарь оружия
    std::unordered_map<std::string, std::shared_ptr<ItemTemplate>> items;

    void init() {
        // Инициализация персонажей
        characters["adeir"] = std::make_shared<AdeirTemplate>();

        // Инициализация шаблонов оружия
        weapons["iron_sword"] = std::make_shared<IronSwordTemplate>();
        weapons["magic_staff"] = std::make_shared<MagicStaffTemplate>();

        // Инициализация предметов
        items["apple"] = std::make_shared<AppleTemplate>();
        items["iron_ore"] = std::make_shared<IronOreTemplate>();
    }
};
