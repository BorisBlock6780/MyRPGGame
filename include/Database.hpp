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
enum class WeaponRarity { Common = 1, Uncommon = 2, Rare = 3, Epic = 4, Legendary = 5, Unique = 6 };
enum class ItemCategory { Food, Weapon, Artifact, Material, Quest, Jewelry};
enum class WeaponType { OneHanded, TwoHanded, Ranged, Magic };

//  ШАБЛОН ОРУЖИЯ
class WeaponTemplate {
protected:
    std::string id;
    std::string name;
    WeaponType type;
    WeaponRarity rarity;
    double baseDamage;
    double damageGrowth;      // Фиксированный прирост урона за каждый уровень
    StatArray bonusStats;     // Базовые бонусы на 1-м уровне
    StatArray statGrowth;     // Фиксированный прирост доп. характеристик за уровень
    std::string effectDesc;

public:
    WeaponTemplate(std::string id, std::string name, WeaponType type, WeaponRarity rarity,
        double baseDamage, double damageGrowth,
        StatArray bonusStats, StatArray statGrowth, std::string effectDesc = "")
        : id(std::move(id)), name(std::move(name)), type(type), rarity(rarity),
        baseDamage(baseDamage), damageGrowth(damageGrowth),
        bonusStats(bonusStats), statGrowth(statGrowth), effectDesc(std::move(effectDesc)) {
    }

    virtual ~WeaponTemplate() = default;
    std::string getId() const { return id; }
    std::string getName() const { return name; }
    WeaponType getType() const { return type; }
    WeaponRarity getRarity() const { return rarity; }
    std::string getEffectDesc() const { return effectDesc; }
    virtual double getDamageAtLevel(int level) const {             // Расчет урона на заданном уровне (по умолчанию 1-20)
        return baseDamage + (damageGrowth * (level - 1));
    }
    virtual StatArray getBonusStatsAtLevel(int level) const {      // Расчет доп. статов на заданном уровне
        StatArray currentStats = bonusStats;
        for (size_t i = 0; i < 8; ++i) {
            currentStats[i] += (statGrowth[i] * (level - 1));
        }
        return currentStats;
    }
    // Виртуальные методы для уникальных механик 6* оружия
    virtual void onHit(StatArray& attackerStats, StatArray& targetStats) const {
    }
};


// 1* ОДНОРУЧНЫЕ МЕЧИ

// Ржавый клинок
class RustyBladeTemplate : public WeaponTemplate {
public:
    RustyBladeTemplate() : WeaponTemplate(
        "rusty_blade", "Ржавый клинок", WeaponType::OneHanded, WeaponRarity::Common,
        15.0, 1.5, 
        { 0, 0, 0, 3.0, 0, 0, 0, 0 }, 
        { 0, 0, 0, 0.5, 0, 0, 0, 0 }  
    ) {
    }
};
// Старый тесак
class OldCleaverTemplate : public WeaponTemplate {
public:
    OldCleaverTemplate() : WeaponTemplate(
        "old_cleaver", "Старый тесак", WeaponType::OneHanded, WeaponRarity::Common,
        18.0, 2.0,
        { 10.0, 0, 0, 0, 0, 0, 0, 0 }, 
        { 2.0,  0, 0, 0, 0, 0, 0, 0 } 
    ) {
    }
};


// 4* ЭПИЧЕСКОЕ ОРУЖИЕ
// Топор Разъяренного
class BerserkerAxeTemplate : public WeaponTemplate {
public:
    BerserkerAxeTemplate() : WeaponTemplate(
        "berserker_axe", "Топор Разъяренного", WeaponType::TwoHanded, WeaponRarity::Epic,
        60.0, 5.0,
        { 0, 0, 15.0, 0, 0, 0, 0, 0 }, 
        { 0, 0, 2.0,  0, 0, 0, 0, 0 }, 
        "Дает до +30 к атаке в зависимости от потерянного здоровья."
    ) {
    }
};


// 6* УНИКАЛЬНОЕ ОРУЖИЕ 

// Кровавый Пожиратель
class VampireBladeTemplate : public WeaponTemplate {
public:
    VampireBladeTemplate() : WeaponTemplate(
        "vampire_blade", "Кровавый Пожиратель", WeaponType::OneHanded, WeaponRarity::Unique,
        45.0, 4.0,
        { 50.0, 0, 0, 0, 0.10, 0.20, 0, 0 },
        { 10.0, 0, 0, 0, 0.0,  0.0,  0, 0 }, 
        "Восстанавливает здоровье в размере 15% от нанесенного урона при атаке." 
    ) {
    }
    void onHit(StatArray& attackerStats, StatArray& targetStats) const override {
        double damageDealt = std::max(1.0, attackerStats[StatIndex::Attack] - targetStats[StatIndex::Defense]);
        double healAmount = damageDealt * 0.15; 
        attackerStats[StatIndex::HP] = std::min(
            attackerStats[StatIndex::HP] + healAmount,
            attackerStats[StatIndex::HP] 
        );
    }
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
    int buyPrice;
    int sellPrice;
    int maxStack;
    sf::Texture iconTexture;

public:
    ItemTemplate(std::string id, std::string name, ItemCategory category, std::string iconPath, int buyPrice, int sellPrice, int maxStack = 99)
        : id(std::move(id)), name(std::move(name)), category(category), iconPath(std::move(iconPath)), buyPrice(buyPrice), sellPrice(sellPrice), maxStack(maxStack) {
        if (!iconTexture.loadFromFile(this->iconPath)) {
            iconTexture.loadFromFile("assets/icons/missing.png");
        }
    }
    virtual ~ItemTemplate() = default;

    std::string getId() const { return id; }
    std::string getName() const { return name; }
    int getBuyPrice() const { return buyPrice; }
    int getSellPrice() const { return sellPrice; }
    ItemCategory getCategory() const { return category; }

    ImTextureID getImGuiTextureID() const {
        return (ImTextureID)(uintptr_t)iconTexture.getNativeHandle();
    }
};

// ШАБЛОН УМЕНИЙ
class SkillTemplate {
protected:
    std::string id;
    std::string name;
    int cooldown;
    int stacksGranted; // Сколько стаков дает навык
    bool consumesAllStacks; // Расходует ли все стаки при использовании

public:
    SkillTemplate(std::string id, std::string name, int cooldown, int stacksGranted = 0, bool consumesAllStacks = false)
        : id(std::move(id)), name(std::move(name)), cooldown(cooldown), stacksGranted(stacksGranted), consumesAllStacks(consumesAllStacks) {
    }

    virtual ~SkillTemplate() = default;

    std::string getId() const { return id; }
    std::string getName() const { return name; }
    int getCooldown() const { return cooldown; }
    int getStacksGranted() const { return stacksGranted; }
    bool getConsumesAllStacks() const { return consumesAllStacks; }

    virtual double calculateValue(const StatArray& stats, int skillStacks) const = 0;
};

//  ШАБЛОН ПЕРСОНАЖА
class CharacterTemplate {
protected:
    std::string id;
    std::string name;
    WeaponType preferredWeapon;
    StatArray baseStats;
    StatArray growthPerLevel;
    std::vector<std::shared_ptr<SkillTemplate>> skills;

public:
    CharacterTemplate(std::string id, std::string name, WeaponType preferredWeapon, StatArray base, StatArray growth, std::vector<std::shared_ptr<SkillTemplate>> skills)
        : id(std::move(id)), name(std::move(name)), preferredWeapon(preferredWeapon), baseStats(base), growthPerLevel(growth), skills(std::move(skills)) {
    }

    virtual ~CharacterTemplate() = default;

    std::string getId() const { return id; }
    std::string getName() const { return name; }
    WeaponType getPreferredWeapon() const { return preferredWeapon; }
    const std::vector<std::shared_ptr<SkillTemplate>>& getSkills() const { return skills; }
    const StatArray& getBaseStats() const { return baseStats; }
    const StatArray& getGrowthPerLevel() const { return growthPerLevel; }
};
//
// АДЕИР
//
class AdeirBasicAtkTemplate : public SkillTemplate {
public:
    AdeirBasicAtkTemplate() : SkillTemplate("Adeir_basic_atk", "Огненный Шар", 0, 1, false) {}
    double calculateValue(const StatArray& stats, int skillStacks) const override {
        // Урон от Маны + бонус за стак
        return (stats[StatIndex::Mana] * 0.8) + (skillStacks * 10.0);
    }
};
class AdeirSkillTemplate : public SkillTemplate {
public:
    AdeirSkillTemplate() : SkillTemplate("Adeir_skill", "Пламенный щит", 3, 1, false) {}
    double calculateValue(const StatArray& stats, int skillStacks) const override {
        // Прочность щита 150% от Защиты + бонус от стаков
        return (stats[StatIndex::Defense] * 1.5) + (skillStacks * 20.0);
    }
};
class AdeirUltimateTemplate : public SkillTemplate {
public:
    AdeirUltimateTemplate() : SkillTemplate("Adeir_ultimate", "Пламенный Щит", 3, 0, true) {}
    double calculateValue(const StatArray& stats, int skillStacks) const override {
        // Прочность щита зависит от Маны и Защиты
        return (stats[StatIndex::Mana] * 1.5) + (stats[StatIndex::Defense] * 2.0);
    }
};
class AdeirTemplate : public CharacterTemplate {
public:
    AdeirTemplate() : CharacterTemplate(
        "adeir",
        "Адеир",
        WeaponType::Magic, // Магическое оружие
        { 110.0, 90.0, 18.0, 14.0, 0.05, 1.50, 0.0, 0.0 },
        { 12.0,  10.0,  3.0,  2.0, 0.00, 0.00, 0.0, 0.0 },
        {
            std::make_shared<AdeirBasicAtkTemplate>(),
            std::make_shared<AdeirSkillTemplate>(),
            std::make_shared<AdeirUltimateTemplate>()
        }
    ) {
    }
};
//
// РЕНВИК
//
class FighterBasicAtkTemplate : public SkillTemplate {
public:
    FighterBasicAtkTemplate() : SkillTemplate("Fighter_basic_atk", "Быстрый удар", 0, 1, false) {}
    double calculateValue(const StatArray& stats, int skillStacks) const override {
        return (stats[StatIndex::Attack] * 1.0);
    }
};
class FighterSkillTemplate : public SkillTemplate {
public:
    FighterSkillTemplate() : SkillTemplate("Fighter_skill", "Шквал ударов", 2, 3, false) {}
    double calculateValue(const StatArray& stats, int skillStacks) const override {
        // Скейл урона от стаков
        return (stats[StatIndex::Attack] * 1.2) + (skillStacks * 25.0);
    }
};
class FighterUltimateTemplate : public SkillTemplate {
public:
    FighterUltimateTemplate() : SkillTemplate("Fighter_ultimate", "Казнь", 4, 0, true) {}
    double calculateValue(const StatArray& stats, int skillStacks) const override {
        // Расходует все стаки, нанося огромный урон
        return (stats[StatIndex::Attack] * 2.5) + (skillStacks * 60.0);
    }
};
class FighterTemplate : public CharacterTemplate {
public:
    FighterTemplate() : CharacterTemplate(
        "renwick",
        "Ренвик",
        WeaponType::OneHanded, // Одноручное
        { 130.0, 50.0, 25.0, 10.0, 0.10, 1.50, 0.0, 0.0 },
        { 15.0,  5.0,  4.5,  1.5, 0.00, 0.00, 0.0, 0.0 },
        {
            std::make_shared<FighterBasicAtkTemplate>(),
            std::make_shared<FighterSkillTemplate>(),
            std::make_shared<FighterUltimateTemplate>()
        }
    ) {
    }
};
//
// ГАРОШ
//
class TankBasicAtkTemplate : public SkillTemplate {
public:
    TankBasicAtkTemplate() : SkillTemplate("Tank_basic_atk", "Тяжелый удар", 0, 0, false) {}
    double calculateValue(const StatArray& stats, int skillStacks) const override {
        return (stats[StatIndex::Attack] * 1.0);
    }
};
class TankSkillTemplate : public SkillTemplate {
public:
    TankSkillTemplate() : SkillTemplate("Tank_skill", "Каменная кожа", 3, 3, false) {}
    double calculateValue(const StatArray& stats, int skillStacks) const override {
        // Щит зависит от защиты
        return (stats[StatIndex::Defense] * 2.0);
    }
};
class TankUltimateTemplate : public SkillTemplate {
public:
    TankUltimateTemplate() : SkillTemplate("Tank_ultimate", "Второе дыхание", 5, 0, false) {}
    double calculateValue(const StatArray& stats, int skillStacks) const override {
        // Восстановление здоровья зависит от максимального ХП и стаков
        return (stats[StatIndex::HP] * 0.3) + (skillStacks * 50.0);
    }
};
class TankTemplate : public CharacterTemplate {
public:
    TankTemplate() : CharacterTemplate(
        "garosh",
        "Гарош",
        WeaponType::TwoHanded, // Двуручное
        { 180.0, 60.0, 15.0, 25.0, 0.05, 1.50, 0.0, 0.0 },
        { 20.0,  6.0,  2.0,  4.0, 0.00, 0.00, 0.0, 0.0 },
        {
            std::make_shared<TankBasicAtkTemplate>(),
            std::make_shared<TankSkillTemplate>(),
            std::make_shared<TankUltimateTemplate>()
        }
    ) {
    }
};
// 
// ВЕКС
// 
class VexBasicAtkTemplate : public SkillTemplate {
public:
    VexBasicAtkTemplate() : SkillTemplate("vex_basic", "Быстрый выстрел", 0, 2, false) {}
    double calculateValue(const StatArray& stats, int skillStacks) const override {
        return stats[StatIndex::Attack] * 0.5; 
    }
};
class VexSkillTemplate : public SkillTemplate {
public:
    VexSkillTemplate() : SkillTemplate("vex_skill", "Прицеливание", 2, 3, false) {}
    double calculateValue(const StatArray& stats, int skillStacks) const override {
        return 5.0;
    }
};
class VexUltimateTemplate : public SkillTemplate {
public:
    VexUltimateTemplate() : SkillTemplate("vex_ult", "Выстрел в голову", 4, 0, true) {}
    double calculateValue(const StatArray& stats, int skillStacks) const override {
        double multiplier = (skillStacks >= 4) ? 3.0 : 0.5;
        return (stats[StatIndex::Attack] * stats[StatIndex::CritDamage]) * multiplier;
    }
};
class VexTemplate : public CharacterTemplate {
public:
    VexTemplate() : CharacterTemplate(
        "vex",
        "Векс",
        WeaponType::Ranged,
        { 90.0, 40.0, 30.0, 8.0, 0.20, 2.00, 0.0, 0.0 }, 
        { 10.0,  4.0,  5.0,  1.0, 0.00, 0.00, 0.0, 0.0 },
        {
            std::make_shared<VexBasicAtkTemplate>(),
            std::make_shared<VexSkillTemplate>(),
            std::make_shared<VexUltimateTemplate>()
        }
    ) {
    }
};
// 
// КАЭЛИМ 
// 
class KaelimBasicAtkTemplate : public SkillTemplate {
public:
    KaelimBasicAtkTemplate() : SkillTemplate("kaelim_basic", "Кровавый шип", 0, 1, false) {}

    double calculateValue(const StatArray& stats, int skillStacks) const override {
        return stats[StatIndex::HP] * 0.10;
    }
};
class KaelimSkillTemplate : public SkillTemplate {
public:
    KaelimSkillTemplate() : SkillTemplate("kaelim_skill", "Жертвенное исцеление", 3, 2, false) {}
    double calculateValue(const StatArray& stats, int skillStacks) const override {
        return stats[StatIndex::HP] * 0.20;
    }
};
class KaelimUltimateTemplate : public SkillTemplate {
public:
    KaelimUltimateTemplate() : SkillTemplate("kaelim_ult", "Багровый взрыв", 5, 0, true) {}
    double calculateValue(const StatArray& stats, int skillStacks) const override {
        double baseDamage = stats[StatIndex::HP] * 0.15;
        double stackMultiplier = 1.0 + (skillStacks * 0.5);
        return baseDamage * stackMultiplier;
    }
};
class KaelimTemplate : public CharacterTemplate {
public:
    KaelimTemplate() : CharacterTemplate(
        "kaelim",
        "Каэлим",
        WeaponType::Magic,
        { 250.0, 100.0, 10.0, 15.0, 0.05, 1.50, 0.0, 0.0 }, 
        { 25.0,  5.0,   2.0,  2.0,  0.00, 0.00, 0.0, 0.0 }, 
        {
            std::make_shared<KaelimBasicAtkTemplate>(),
            std::make_shared<KaelimSkillTemplate>(),
            std::make_shared<KaelimUltimateTemplate>()
        }
    ) {
    }
};
// 
// ВЕЛАРИ
// 
class VelariBasicAtkTemplate : public SkillTemplate {
public:
    VelariBasicAtkTemplate() : SkillTemplate("velari_basic", "Иллюзорный клинок", 0, 1, false) {}
    double calculateValue(const StatArray& stats, int skillStacks) const override {
        return stats[StatIndex::Mana] * 0.50;
    }
};
class VelariSkillTemplate : public SkillTemplate {
public:
    VelariSkillTemplate() : SkillTemplate("velari_skill", "Зеркальный барьер", 3, 2, false) {}
    double calculateValue(const StatArray& stats, int skillStacks) const override {
        return stats[StatIndex::Mana] * 0.80;
    }
};
class VelariUltimateTemplate : public SkillTemplate {
public:
    VelariUltimateTemplate() : SkillTemplate("velari_ult", "Осколки разума", 4, 0, false) {}
    double calculateValue(const StatArray& stats, int skillStacks) const override {
        double baseDamage = stats[StatIndex::Mana] * 1.0;
        double stackMultiplier = 1.0 + (skillStacks * 0.20);
        return baseDamage * stackMultiplier;
    }
};
class VelariTemplate : public CharacterTemplate {
public:
    VelariTemplate() : CharacterTemplate(
        "velari",
        "Велари",
        WeaponType::Magic,
        { 90.0, 180.0, 12.0, 10.0, 0.10, 1.50, 0.0, 0.0 }, 
        { 8.0,  20.0,  2.0,  1.0,  0.00, 0.00, 0.0, 0.0 }, 
        {
            std::make_shared<VelariBasicAtkTemplate>(),
            std::make_shared<VelariSkillTemplate>(),
            std::make_shared<VelariUltimateTemplate>()
        }
    ) {
    }
};
// 
// ТАУРОС 
// 
class TaurosBasicAtkTemplate : public SkillTemplate {
public:
    TaurosBasicAtkTemplate() : SkillTemplate("tauros_basic", "Разряд", 0, 1, false) {}
    double calculateValue(const StatArray& stats, int skillStacks) const override {
        return stats[StatIndex::Attack] * 0.8;
    }
};
class TaurosSkillTemplate : public SkillTemplate {
public:
    TaurosSkillTemplate() : SkillTemplate("tauros_skill", "Цепная молния", 3, 3, false) {}
    double calculateValue(const StatArray& stats, int skillStacks) const override {
        return (stats[StatIndex::Attack] * 0.5) + (stats[StatIndex::Mana] * 0.5);
    }
};
class TaurosUltimateTemplate : public SkillTemplate {
public:
    TaurosUltimateTemplate() : SkillTemplate("tauros_ult", "Гнев Бури", 4, 0, true) {}
    double calculateValue(const StatArray& stats, int skillStacks) const override {
        double damagePerStack = 50.0 + (stats[StatIndex::Mana] * 0.8);
        return damagePerStack * skillStacks;
    }
};
class TaurosTemplate : public CharacterTemplate {
public:
    TaurosTemplate() : CharacterTemplate(
        "tauros",
        "Таурос",
        WeaponType::Magic,
        { 120.0, 120.0, 20.0, 15.0, 0.05, 1.50, 0.0, 0.0 },
        { 10.0,  12.0,  4.0,  2.0,  0.00, 0.00, 0.0, 0.0 },
        {
            std::make_shared<TaurosBasicAtkTemplate>(),
            std::make_shared<TaurosSkillTemplate>(),
            std::make_shared<TaurosUltimateTemplate>()
        }
    ) {
    }
};
// 
// ЛЮМЬЕН
// 
class LumienBasicAtkTemplate : public SkillTemplate {
public:
    LumienBasicAtkTemplate() : SkillTemplate("lumien_basic", "Искажающий луч", 0, 1, false) {}
    double calculateValue(const StatArray& stats, int skillStacks) const override {
        return stats[StatIndex::Defense] * 1.0;
    }
};
class LumienSkillTemplate : public SkillTemplate {
public:
    LumienSkillTemplate() : SkillTemplate("lumien_skill", "Искажение времени", 2, 2, false) {}
    double calculateValue(const StatArray& stats, int skillStacks) const override {
        return stats[StatIndex::Mana] * 0.40;
    }
};
class LumienUltimateTemplate : public SkillTemplate {
public:
    LumienUltimateTemplate() : SkillTemplate("lumien_ult", "Коллапс", 4, 0, true) {}

    double calculateValue(const StatArray& stats, int skillStacks) const override {
        return (stats[StatIndex::Defense] + stats[StatIndex::Mana]) * skillStacks;
    }
};
class LumienTemplate : public CharacterTemplate {
public:
    LumienTemplate() : CharacterTemplate(
        "lumien",
        "Люмьен",
        WeaponType::Magic,
        { 110.0, 140.0, 5.0, 25.0, 0.05, 1.50, 0.0, 0.0 }, 
        { 12.0,  15.0,  1.0, 4.0,  0.00, 0.00, 0.0, 0.0 },
        {
            std::make_shared<LumienBasicAtkTemplate>(),
            std::make_shared<LumienSkillTemplate>(),
            std::make_shared<LumienUltimateTemplate>()
        }
    ) {
    }
};
// 
// САЙЛАС
// 
class SilasBasicAtkTemplate : public SkillTemplate {
public:
    SilasBasicAtkTemplate() : SkillTemplate("silas_basic", "Теневой выпад", 0, 2, false) {}
    double calculateValue(const StatArray& stats, int skillStacks) const override {
        return stats[StatIndex::Attack] * 1.0;
    }
};
class SilasSkillTemplate : public SkillTemplate {
public:
    SilasSkillTemplate() : SkillTemplate("silas_skill", "Подготовка к убийству", 3, 4, false) {}
    double calculateValue(const StatArray& stats, int skillStacks) const override {
        return 0.0;
    }
};
class SilasUltimateTemplate : public SkillTemplate {
public:
    SilasUltimateTemplate() : SkillTemplate("silas_ult", "Смертельный танец", 4, 0, true) {}
    double calculateValue(const StatArray& stats, int skillStacks) const override {
        double effectiveCritDamage = stats[StatIndex::CritDamage] * (1.0 + (skillStacks * 0.15));
        return stats[StatIndex::Attack] * effectiveCritDamage;
    }
};
class SilasTemplate : public CharacterTemplate {
public:
    SilasTemplate() : CharacterTemplate(
        "silas",
        "Сайлас",
        WeaponType::OneHanded,
        { 85.0, 50.0, 30.0, 8.0, 0.25, 2.00, 0.0, 0.0 }, 
        { 8.0,  4.0,  6.0,  1.0, 0.01, 0.00, 0.0, 0.0 }, 
        {
            std::make_shared<SilasBasicAtkTemplate>(),
            std::make_shared<SilasSkillTemplate>(),
            std::make_shared<SilasUltimateTemplate>()
        }
    ) {
    }
};
// 
// ФИНН
// 
class FinnBasicAtkTemplate : public SkillTemplate {
public:
    FinnBasicAtkTemplate() : SkillTemplate("finn_basic", "Укол рапирой", 0, 1, false) {}

    double calculateValue(const StatArray& stats, int skillStacks) const override {
        return stats[StatIndex::Attack] * 1.0;
    }
};
class FinnSkillTemplate : public SkillTemplate {
public:
    FinnSkillTemplate() : SkillTemplate("finn_skill", "Защитная стойка", 2, 2, false) {}

    double calculateValue(const StatArray& stats, int skillStacks) const override {
        return stats[StatIndex::Defense] * 1.5;
    }
};
class FinnUltimateTemplate : public SkillTemplate {
public:
    FinnUltimateTemplate() : SkillTemplate("finn_ult", "Финальный выпад", 4, 0, true) {}

    double calculateValue(const StatArray& stats, int skillStacks) const override {
        return (stats[StatIndex::Attack] + stats[StatIndex::Defense]) * skillStacks;
    }
};
class FinnTemplate : public CharacterTemplate {
public:
    FinnTemplate() : CharacterTemplate(
        "finn",
        "Финн",
        WeaponType::OneHanded,
        { 105.0, 60.0, 20.0, 18.0, 0.10, 1.50, 0.0, 0.0 }, 
        { 10.0,  5.0,  3.0,  3.0,  0.00, 0.00, 0.0, 0.0 },
        {
            std::make_shared<FinnBasicAtkTemplate>(),
            std::make_shared<FinnSkillTemplate>(),
            std::make_shared<FinnUltimateTemplate>()
        }
    ) {
    }
};
// 
// ЭЛИКС
// 
class ElixBasicAtkTemplate : public SkillTemplate {
public:
    ElixBasicAtkTemplate() : SkillTemplate("elix_basic", "Бросок колбы", 0, 1, false) {}
    double calculateValue(const StatArray& stats, int skillStacks) const override {
        return stats[StatIndex::Mana] * 0.6;
    }
};
class ElixSkillTemplate : public SkillTemplate {
public:
    ElixSkillTemplate() : SkillTemplate("elix_skill", "Исцеляющее зелье", 3, 2, false) {}
    double calculateValue(const StatArray& stats, int skillStacks) const override {
        return stats[StatIndex::HP] * 0.25;
    }
};
class ElixUltimateTemplate : public SkillTemplate {
public:
    ElixUltimateTemplate() : SkillTemplate("elix_ult", "Трансмутация", 4, 0, true) {}
    double calculateValue(const StatArray& stats, int skillStacks) const override {
        double baseDamage = stats[StatIndex::Mana] * 1.5;
        double stackBonus = skillStacks * 30.0;
        return baseDamage + stackBonus;
    }
};
class ElixTemplate : public CharacterTemplate {
public:
    ElixTemplate() : CharacterTemplate(
        "elix",
        "Эликс",
        WeaponType::Ranged,
        { 100.0, 150.0, 15.0, 12.0, 0.05, 1.50, 0.0, 0.0 },
        { 10.0,  15.0,  2.0,  2.0, 0.00, 0.00, 0.0, 0.0 },
        {
            std::make_shared<ElixBasicAtkTemplate>(),
            std::make_shared<ElixSkillTemplate>(),
            std::make_shared<ElixUltimateTemplate>()
        }
    ) {
    }
};
// 
// ТОРК
// 
class TorkBasicAtkTemplate : public SkillTemplate {
public:
    TorkBasicAtkTemplate() : SkillTemplate("tork_basic", "Выстрел из мушкета", 0, 1, false) {}
    double calculateValue(const StatArray& stats, int skillStacks) const override {
        return stats[StatIndex::Attack] * 1.0;
    }
};
class TorkSkillTemplate : public SkillTemplate {
public:
    TorkSkillTemplate() : SkillTemplate("tork_skill", "Осколочная граната", 3, 2, false) {}
    double calculateValue(const StatArray& stats, int skillStacks) const override {
        return (stats[StatIndex::Attack] * 0.8) + (stats[StatIndex::Defense] * 0.8);
    }
};
class TorkUltimateTemplate : public SkillTemplate {
public:
    TorkUltimateTemplate() : SkillTemplate("tork_ult", "Ковровая бомбардировка", 5, 0, true) {}
    double calculateValue(const StatArray& stats, int skillStacks) const override {
        return stats[StatIndex::Defense] * 2.0 * skillStacks;
    }
};
class TorkTemplate : public CharacterTemplate {
public:
    TorkTemplate() : CharacterTemplate(
        "tork",
        "Торк",
        WeaponType::Ranged,
        { 130.0, 60.0, 20.0, 30.0, 0.05, 1.50, 0.0, 0.0 },
        { 12.0,  5.0,  3.0,  5.0, 0.00, 0.00, 0.0, 0.0 },
        {
            std::make_shared<TorkBasicAtkTemplate>(),
            std::make_shared<TorkSkillTemplate>(),
            std::make_shared<TorkUltimateTemplate>()
        }
    ) {
    }
};
// 
// МИРА
//  
class MiraBasicAtkTemplate : public SkillTemplate {
public:
    MiraBasicAtkTemplate() : SkillTemplate("mira_basic", "Танец лезвий", 0, 1, false) {}
    double calculateValue(const StatArray& stats, int skillStacks) const override {
        return stats[StatIndex::Attack] * 0.7;
    }
};
class MiraSkillTemplate : public SkillTemplate {
public:
    MiraSkillTemplate() : SkillTemplate("mira_skill", "Кровавый пируэт", 3, 2, false) {}
    double calculateValue(const StatArray& stats, int skillStacks) const override {
        double vampMultiplier = 1.0 + (stats[StatIndex::Vampirism] * 5.0);
        return stats[StatIndex::Attack] * vampMultiplier;
    }
};
class MiraUltimateTemplate : public SkillTemplate {
public:
    MiraUltimateTemplate() : SkillTemplate("mira_ult", "Смертельный ритм", 4, 0, true) {}
    double calculateValue(const StatArray& stats, int skillStacks) const override {
        double baseDamage = stats[StatIndex::Attack] * 1.5;
        double stackDamage = skillStacks * 40.0;
        double vampMultiplier = 1.0 + (stats[StatIndex::Vampirism] * 2.0);
        return (baseDamage + stackDamage) * vampMultiplier;
    }
};
class MiraTemplate : public CharacterTemplate {
public:
    MiraTemplate() : CharacterTemplate(
        "mira",
        "Мира",
        WeaponType::OneHanded, 
        { 100.0, 50.0, 22.0, 12.0, 0.15, 1.50, 0.10, 0.0 }, 
        { 10.0,  5.0,  4.0,  2.0,  0.00, 0.00, 0.02, 0.0 },
        {
            std::make_shared<MiraBasicAtkTemplate>(),
            std::make_shared<MiraSkillTemplate>(),
            std::make_shared<MiraUltimateTemplate>()
        }
    ) {
    }
};
//
// РОЛАНД
// 
class RolandBasicAtkTemplate : public SkillTemplate {
public:
    RolandBasicAtkTemplate() : SkillTemplate("roland_basic", "Удар щитом", 0, 1, false) {}
    double calculateValue(const StatArray& stats, int skillStacks) const override {
        return stats[StatIndex::Defense] * 1.0;
    }
};
class RolandSkillTemplate : public SkillTemplate {
public:
    RolandSkillTemplate() : SkillTemplate("roland_skill", "Свет веры", 2, 1, false) {}
    double calculateValue(const StatArray& stats, int skillStacks) const override {
        return stats[StatIndex::Defense] * 1.5;
    }
};
class RolandUltimateTemplate : public SkillTemplate {
public:
    RolandUltimateTemplate() : SkillTemplate("roland_ult", "Небесный суд", 5, 0, true) {}
    double calculateValue(const StatArray& stats, int skillStacks) const override {
        return stats[StatIndex::Defense] * 0.8 * skillStacks;
    }
};
class RolandTemplate : public CharacterTemplate {
public:
    RolandTemplate() : CharacterTemplate(
        "roland",
        "Роланд",
        WeaponType::OneHanded, 
        { 160.0, 80.0, 5.0, 30.0, 0.05, 1.50, 0.0, 0.0 },
        { 15.0,  5.0,  1.0, 5.0,  0.00, 0.00, 0.0, 0.0 },
        {
            std::make_shared<RolandBasicAtkTemplate>(),
            std::make_shared<RolandSkillTemplate>(),
            std::make_shared<RolandUltimateTemplate>()
        }
    ) {
    }
};
//
// БОРН
//
class BornBasicAtkTemplate : public SkillTemplate {
public:
    BornBasicAtkTemplate() : SkillTemplate("born_basic", "Размашистый удар", 0, 1, false) {}
    double calculateValue(const StatArray& stats, int skillStacks) const override {
        return stats[StatIndex::Attack] * 1.2;
    }
};
class BornSkillTemplate : public SkillTemplate {
public:
    BornSkillTemplate() : SkillTemplate("born_skill", "Кровавая пелена", 3, 2, false) {}

    double calculateValue(const StatArray& stats, int skillStacks) const override {
        return stats[StatIndex::HP] * 0.15;
    }
};
class BornUltimateTemplate : public SkillTemplate {
public:
    BornUltimateTemplate() : SkillTemplate("born_ult", "Казнь", 4, 0, true) {}
    double calculateValue(const StatArray& stats, int skillStacks) const override {
        double baseDamage = stats[StatIndex::Attack] * 1.5;
        double stackMultiplier = 1.0 + skillStacks;
        return baseDamage * stackMultiplier;
    }
};
class BornTemplate : public CharacterTemplate {
public:
    BornTemplate() : CharacterTemplate(
        "born",
        "Борн",
        WeaponType::TwoHanded, 
        { 200.0, 40.0, 20.0, 15.0, 0.10, 1.80, 0.0, 0.0 }, 
        { 22.0,  4.0,  4.0,  2.0,  0.00, 0.00, 0.0, 0.0 },
        {
            std::make_shared<BornBasicAtkTemplate>(),
            std::make_shared<BornSkillTemplate>(),
            std::make_shared<BornUltimateTemplate>()
        }
    ) {
    }
};
// 
// МОРТИС
// 
class MortisBasicAtkTemplate : public SkillTemplate {
public:
    MortisBasicAtkTemplate() : SkillTemplate("mortis_basic", "Удар жнеца", 0, 1, false) {}
    double calculateValue(const StatArray& stats, int skillStacks) const override {
        return stats[StatIndex::Attack] * 1.0;
    }
};
class MortisSkillTemplate : public SkillTemplate {
public:
    MortisSkillTemplate() : SkillTemplate("mortis_skill", "Вытягивание души", 2, 1, false) {}
    double calculateValue(const StatArray& stats, int skillStacks) const override {
        return (stats[StatIndex::Attack] * 0.5) + (stats[StatIndex::Mana] * 0.5);
    }
};
class MortisUltimateTemplate : public SkillTemplate {
public:
    MortisUltimateTemplate() : SkillTemplate("mortis_ult", "Жнец", 4, 0, false) {}
    double calculateValue(const StatArray& stats, int skillStacks) const override {
        double baseDamage = stats[StatIndex::Attack] * 2.0;
        double stackMultiplier = 1.0 + (skillStacks * 0.10);
        return baseDamage * stackMultiplier;
    }
};
class MortisTemplate : public CharacterTemplate {
public:
    MortisTemplate() : CharacterTemplate(
        "mortis",
        "Мортис",
        WeaponType::TwoHanded, 
        { 180.0, 80.0, 18.0, 20.0, 0.05, 1.50, 0.15, 0.0 }, 
        { 18.0,  6.0,  3.0,  3.0,  0.00, 0.00, 0.01, 0.0 },
        {
            std::make_shared<MortisBasicAtkTemplate>(),
            std::make_shared<MortisSkillTemplate>(),
            std::make_shared<MortisUltimateTemplate>()
        }
    ) {
    }
};
// 
// ЗИГФРИД 
// 
class SiegfriedBasicAtkTemplate : public SkillTemplate {
public:
    SiegfriedBasicAtkTemplate() : SkillTemplate("siegfried_basic", "Удар молотом", 0, 1, false) {}
    double calculateValue(const StatArray& stats, int skillStacks) const override {
        return stats[StatIndex::Attack] * 1.0;
    }
};
class SiegfriedSkillTemplate : public SkillTemplate {
public:
    SiegfriedSkillTemplate() : SkillTemplate("siegfried_skill", "Антимагический удар", 3, 2, false) {}
    double calculateValue(const StatArray& stats, int skillStacks) const override {
        return stats[StatIndex::Defense] * 1.0 + stats[StatIndex::Mana] * 1.0;
    }
};
class SiegfriedUltimateTemplate : public SkillTemplate {
public:
    SiegfriedUltimateTemplate() : SkillTemplate("siegfried_ult", "Очищение", 5, 0, true) {}
    double calculateValue(const StatArray& stats, int skillStacks) const override {
        return stats[StatIndex::Mana] * 1.5 * skillStacks;
    }
};
class SiegfriedTemplate : public CharacterTemplate {
public:
    SiegfriedTemplate() : CharacterTemplate(
        "siegfried",
        "Зигфрид",
        WeaponType::TwoHanded, 
        { 150.0, 100.0, 15.0, 25.0, 0.05, 1.50, 0.0, 0.0 }, 
        { 15.0,  10.0,  2.0,  4.0,  0.00, 0.00, 0.0, 0.0 },
        {
            std::make_shared<SiegfriedBasicAtkTemplate>(),
            std::make_shared<SiegfriedSkillTemplate>(),
            std::make_shared<SiegfriedUltimateTemplate>()
        }
    ) {
    }
};
// 
// СИЛЬВАНА
// 
class SylvanaBasicAtkTemplate : public SkillTemplate {
public:
    SylvanaBasicAtkTemplate() : SkillTemplate("sylvana_basic", "Быстрая стрела", 0, 2, false) {}
    double calculateValue(const StatArray& stats, int skillStacks) const override {
        return stats[StatIndex::Attack] * 0.6;
    }
};
class SylvanaSkillTemplate : public SkillTemplate {
public:
    SylvanaSkillTemplate() : SkillTemplate("sylvana_skill", "Залп", 2, 1, false) {}
    double calculateValue(const StatArray& stats, int skillStacks) const override {
        return stats[StatIndex::Attack] * 1.2;
    }
};
class SylvanaUltimateTemplate : public SkillTemplate {
public:
    SylvanaUltimateTemplate() : SkillTemplate("sylvana_ult", "Звериная ярость", 4, 0, true) {}
    double calculateValue(const StatArray& stats, int skillStacks) const override {
        double baseDamage = stats[StatIndex::Attack] * 1.5;
        double stackBonus = skillStacks * (stats[StatIndex::Attack] * 0.4);
        return baseDamage + stackBonus;
    }
};
class SylvanaTemplate : public CharacterTemplate {
public:
    SylvanaTemplate() : CharacterTemplate(
        "sylvana",
        "Сильвана",
        WeaponType::Ranged,
        { 110.0, 60.0, 26.0, 10.0, 0.15, 1.80, 0.0, 0.0 }, 
        { 12.0,  5.0,  4.0,  1.5,  0.00, 0.00, 0.0, 0.0 },
        {
            std::make_shared<SylvanaBasicAtkTemplate>(),
            std::make_shared<SylvanaSkillTemplate>(),
            std::make_shared<SylvanaUltimateTemplate>()
        }
    ) {
    }
};
// 
// ДРЕЙК 
// 
class DrakeBasicAtkTemplate : public SkillTemplate {
public:
    DrakeBasicAtkTemplate() : SkillTemplate("drake_basic", "Удар с плеча", 0, 2, false) {}
    double calculateValue(const StatArray& stats, int skillStacks) const override {
        return stats[StatIndex::Attack] * 1.0;
    }
};
class DrakeSkillTemplate : public SkillTemplate {
public:
    DrakeSkillTemplate() : SkillTemplate("drake_skill", "Круговой удар", 2, 2, false) {}
    double calculateValue(const StatArray& stats, int skillStacks) const override {
        return stats[StatIndex::Attack] * 1.2;
    }
};
class DrakeUltimateTemplate : public SkillTemplate {
public:
    DrakeUltimateTemplate() : SkillTemplate("drake_ult", "Жестокий прием", 4, 0, true) {}
    double calculateValue(const StatArray& stats, int skillStacks) const override {
        double baseDamage = stats[StatIndex::Attack] * 1.5;
        double trueDamageBonus = skillStacks * 60.0;
        return baseDamage + trueDamageBonus;
    }
};
class DrakeTemplate : public CharacterTemplate {
public:
    DrakeTemplate() : CharacterTemplate(
        "drake",
        "Дрейк",
        WeaponType::TwoHanded,
        { 160.0, 50.0, 22.0, 18.0, 0.05, 1.50, 0.0, 0.0 }, 
        { 18.0,  5.0,  3.0,  3.0,  0.00, 0.00, 0.0, 0.0 },
        {
            std::make_shared<DrakeBasicAtkTemplate>(),
            std::make_shared<DrakeSkillTemplate>(),
            std::make_shared<DrakeUltimateTemplate>()
        }
    ) {
    }
};
// 
// КОРМАК
// 
class CormacBasicAtkTemplate : public SkillTemplate {
public:
    CormacBasicAtkTemplate() : SkillTemplate("cormac_basic", "Тяжелый болт", 0, 1, false) {}
    double calculateValue(const StatArray& stats, int skillStacks) const override {
        return stats[StatIndex::Attack] * 1.2;
    }
};
class CormacSkillTemplate : public SkillTemplate {
public:
    CormacSkillTemplate() : SkillTemplate("cormac_skill", "Залп из укрытия", 3, 2, false) {}
    double calculateValue(const StatArray& stats, int skillStacks) const override {
        return stats[StatIndex::HP] * 0.15;
    }
};
class CormacUltimateTemplate : public SkillTemplate {
public:
    CormacUltimateTemplate() : SkillTemplate("cormac_ult", "Осадный выстрел", 5, 0, true) {}
    double calculateValue(const StatArray& stats, int skillStacks) const override {
        return stats[StatIndex::Attack] * 1.8 * skillStacks;
    }
};
class CormacTemplate : public CharacterTemplate {
public:
    CormacTemplate() : CharacterTemplate(
        "cormac",
        "Кормак",
        WeaponType::Ranged,
        { 140.0, 60.0, 24.0, 20.0, 0.05, 1.50, 0.0, 0.0 }, 
        { 15.0,  5.0,  4.0,  4.0,  0.00, 0.00, 0.0, 0.0 },
        {
            std::make_shared<CormacBasicAtkTemplate>(),
            std::make_shared<CormacSkillTemplate>(),
            std::make_shared<CormacUltimateTemplate>()
        }
    ) {
    }
};


class GameDatabase {
public:
    static GameDatabase& getInstance() {
        static GameDatabase instance;
        return instance;
    }

    // Хранилища умных указателей
    std::unordered_map<std::string, std::shared_ptr<CharacterTemplate>> characters;
    std::unordered_map<std::string, std::shared_ptr<WeaponTemplate>> weapons; 
    std::unordered_map<std::string, std::shared_ptr<ItemTemplate>> items;

    void registerItem(const std::string& id, const std::string& name, ItemCategory category,
        const std::string& iconPath, int buyPrice = 25, int sellPrice = 20, int maxStack = 99) {
        items[id] = std::make_shared<ItemTemplate>(id, name, category, iconPath, buyPrice, sellPrice, maxStack);
    }

    void init() {
        // Инициализация персонажей
        characters["adeir"] = std::make_shared<AdeirTemplate>();
        characters["renwick"] = std::make_shared<FighterTemplate>();
        characters["garosh"] = std::make_shared<TankTemplate>();
        characters["vex"] = std::make_shared<VexTemplate>();
        characters["kaelim"] = std::make_shared<KaelimTemplate>();
        characters["velari"] = std::make_shared<VelariTemplate>();
        characters["tauros"] = std::make_shared<TaurosTemplate>();
        characters["lumien"] = std::make_shared<LumienTemplate>();
        characters["silas"] = std::make_shared<SilasTemplate>();
        characters["finn"] = std::make_shared<FinnTemplate>();
        characters["elix"] = std::make_shared<ElixTemplate>();
        characters["tork"] = std::make_shared<TorkTemplate>();
        characters["mira"] = std::make_shared<MiraTemplate>();
        characters["roland"] = std::make_shared<RolandTemplate>();
        characters["born"] = std::make_shared<BornTemplate>();
        characters["mortis"] = std::make_shared<MortisTemplate>();
        characters["siegfried"] = std::make_shared<SiegfriedTemplate>();
        characters["sylvana"] = std::make_shared<SylvanaTemplate>();
        characters["drake"] = std::make_shared<DrakeTemplate>();
        characters["cormac"] = std::make_shared<CormacTemplate>();

        // Инициализация шаблонов оружия
        weapons["rusty_blade"] = std::make_shared<RustyBladeTemplate>();
        weapons["old_cleaver"] = std::make_shared<OldCleaverTemplate>();
        weapons["berserker_axe"] = std::make_shared<BerserkerAxeTemplate>();
        weapons["vampire_blade"] = std::make_shared<VampireBladeTemplate>();

        // Инициализация предметов
        registerItem("bone_meal", "Костная мука", ItemCategory::Food, "assets/icons/bone_meal.png", 20 ,15);
        registerItem("rat_meat", "Сырое мясо крысы", ItemCategory::Food, "assets/icons/rat_meat.png", 20, 15);
        registerItem("rusty_mushroom", "Ржавый гриб", ItemCategory::Food, "assets/icons/rusty_mushroom.png", 20, 15);
        registerItem("glowing_moss", "Светящийся мох", ItemCategory::Food, "assets/icons/glowing_moss.png", 20, 15);
        registerItem("edible_ash", "Съедобный пепел", ItemCategory::Food, "assets/icons/edible_ash.png", 20, 15);
        registerItem("dirty_water", "Сборная вода", ItemCategory::Food, "assets/icons/dirty_water.png", 20, 15);

        // --- Этаж 2: Оазис Тишины ---
        registerItem("cave_wheat", "Пещерная пшеница", ItemCategory::Food, "assets/icons/cave_wheat.png", 20, 15);
        registerItem("goat_milk", "Сырое козье молоко", ItemCategory::Food, "assets/icons/goat_milk.png", 20, 15);
        registerItem("gutted_fish", "Потрошеная озерная рыба", ItemCategory::Food, "assets/icons/gutted_fish.png", 20, 15);
        registerItem("wild_berries", "Дикие ягоды", ItemCategory::Food, "assets/icons/wild_berries.png", 20, 15);
        registerItem("fresh_game", "Парное мясо дичи", ItemCategory::Food, "assets/icons/fresh_game.png", 20, 15);
        registerItem("greenhouse_apple", "Тепличные яблоки", ItemCategory::Food, "assets/icons/greenhouse_apple.png", 20, 15);

        // --- Этаж 3: Затонувший Акрополь ---
        registerItem("severed_tentacle", "Отрубленное щупальце", ItemCategory::Food, "assets/icons/severed_tentacle.png", 20, 15);
        registerItem("raw_seaweed", "Сырая водоросль", ItemCategory::Food, "assets/icons/raw_seaweed.png", 20, 15);
        registerItem("jellyfish_slime", "Слизь медузы", ItemCategory::Food, "assets/icons/jellyfish_slime.png", 20, 15);
        registerItem("shell_meat", "Мясо в панцире", ItemCategory::Food, "assets/icons/shell_meat.png", 20, 15);
        registerItem("ocean_salt", "Океаническая соль", ItemCategory::Food, "assets/icons/ocean_salt.png", 20, 15);
        registerItem("live_clam", "Живой моллюск", ItemCategory::Food, "assets/icons/live_clam.png", 20, 15);

        // --- Этаж 4: Раскаленные Магистрали ---
        registerItem("fire_grain", "Огнеупорные злаки", ItemCategory::Food, "assets/icons/fire_grain.png", 20, 15);
        registerItem("salamander_meat", "Мясо саламандры", ItemCategory::Food, "assets/icons/salamander_meat.png", 20, 15);
        registerItem("slag_root", "Острый шлаковый корень", ItemCategory::Food, "assets/icons/slag_root.png", 20, 15);
        registerItem("coal_dust", "Угольная пыль", ItemCategory::Food, "assets/icons/coal_dust.png", 20, 15);
        registerItem("magma_resin", "Смола магма-дерева", ItemCategory::Food, "assets/icons/magma_resin.png", 20, 15);
        registerItem("thermal_water", "Термальная вода", ItemCategory::Food, "assets/icons/thermal_water.png", 20, 15);

        // --- Этаж 5: Железный Карантин ---
        registerItem("protein_mass", "Белковая масса", ItemCategory::Food, "assets/icons/protein_mass.png", 20, 15);
        registerItem("synth_base", "Синтетическая основа", ItemCategory::Food, "assets/icons/synth_base.png", 20, 15);
        registerItem("saline", "Физраствор", ItemCategory::Food, "assets/icons/saline.png", 20, 15);
        registerItem("infected_mycelium", "Зараженная грибница", ItemCategory::Food, "assets/icons/infected_mycelium.png", 20, 15);
        registerItem("chem_concentrate", "Химический концентрат", ItemCategory::Food, "assets/icons/chem_concentrate.png", 20, 15);
        registerItem("vitamin_powder", "Витаминный порошок", ItemCategory::Food, "assets/icons/vitamin_powder.png", 20, 15);

        // --- Этаж 6: Изумрудный Свод ---
        registerItem("giant_fruit", "Тропический плод-гигант", ItemCategory::Food, "assets/icons/giant_fruit.png", 20, 15);
        registerItem("live_grub", "Живая личинка", ItemCategory::Food, "assets/icons/live_grub.png", 20, 15);
        registerItem("predator_nectar", "Нектар хищного цветка", ItemCategory::Food, "assets/icons/predator_nectar.png", 20, 15);
        registerItem("antitox_root", "Антитоксичный корень", ItemCategory::Food, "assets/icons/antitox_root.png", 20, 15);
        registerItem("lizard_meat", "Сырое мясо ящера", ItemCategory::Food, "assets/icons/lizard_meat.png", 20, 15);
        registerItem("sacred_vine_leaf", "Листья священной лианы", ItemCategory::Food, "assets/icons/sacred_vine_leaf.png", 20, 15);

        // --- Этаж 7: Застывшее Время ---
        registerItem("frost_truffle", "Морозный трюфель", ItemCategory::Food, "assets/icons/frost_truffle.png", 20, 15);
        registerItem("ice_apple", "Ледяное яблоко", ItemCategory::Food, "assets/icons/ice_apple.png", 20, 15);
        registerItem("snow_wolf_meat", "Мясо снежного волка", ItemCategory::Food, "assets/icons/snow_wolf_meat.png", 20, 15);
        registerItem("frozen_cream", "Застывшие сливки", ItemCategory::Food, "assets/icons/frozen_cream.png", 20, 15);
        registerItem("crystal_juice", "Кристаллизованный сок", ItemCategory::Food, "assets/icons/crystal_juice.png", 20, 15);
        registerItem("magic_ice_crumb", "Крошка магического льда", ItemCategory::Food, "assets/icons/magic_ice_crumb.png", 20, 15);

        // --- Этаж 8: Иллюзорный Мегаполис ---
        registerItem("food_powder", "Пищевой порошок", ItemCategory::Food, "assets/icons/food_powder.png", 20, 15);
        registerItem("neon_extract", "Неоновый экстракт", ItemCategory::Food, "assets/icons/neon_extract.png", 20, 15);
        registerItem("taste_matrix", "Вкусовая матрица", ItemCategory::Food, "assets/icons/taste_matrix.png", 20, 15);
        registerItem("plastic_protein", "Пластиковый протеин", ItemCategory::Food, "assets/icons/plastic_protein.png", 20, 15);
        registerItem("chem_sweetener", "Химический подсластитель", ItemCategory::Food, "assets/icons/chem_sweetener.png", 20, 15);
        registerItem("synth_water", "Синтетическая вода", ItemCategory::Food, "assets/icons/synth_water.png", 20, 15);

        // --- Этаж 9: Небесный Собор ---
        registerItem("golden_wheat", "Золотая пшеница", ItemCategory::Food, "assets/icons/golden_wheat.png", 20, 15);
        registerItem("lightbird_breast", "Грудка светоносной птицы", ItemCategory::Food, "assets/icons/lightbird_breast.png", 20, 15);
        registerItem("sky_nectar", "Сырой небесный нектар", ItemCategory::Food, "assets/icons/sky_nectar.png", 20, 15);
        registerItem("holy_water", "Святая вода", ItemCategory::Food, "assets/icons/holy_water.png", 20, 15);
        registerItem("blessed_seed", "Благословенные семена", ItemCategory::Food, "assets/icons/blessed_seed.png", 20, 15);
        registerItem("ether_spice", "Эфирная пряность", ItemCategory::Food, "assets/icons/ether_spice.png", 20, 15);
    }
};
