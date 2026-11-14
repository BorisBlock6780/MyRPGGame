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
enum class ArtifactType { None, Weapon, Headgear, Costume, Amulet, Ring };

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

// Посох Ученика
class ApprenticeStaffTemplate : public WeaponTemplate {
public:
    ApprenticeStaffTemplate() : WeaponTemplate(
        "apprentice_staff", "Посох Ученика", WeaponType::Magic, WeaponRarity::Uncommon,
        25.0, 2.5,
        { 0, 20.0, 8.0, 0, 0, 0, 0, 0 },
        { 0, 4.0, 1.5, 0, 0, 0, 0, 0 },
        "Усиливает поток магической энергии и атаку заклинателя."
    ) {}
};

// Охотничий Лук
class HunterBowTemplate : public WeaponTemplate {
public:
    HunterBowTemplate() : WeaponTemplate(
        "hunter_bow", "Охотничий Лук", WeaponType::Ranged, WeaponRarity::Uncommon,
        28.0, 2.8,
        { 0, 0, 10.0, 0, 0.08, 0.15, 0, 0 },
        { 0, 0, 2.0, 0, 0.01, 0.02, 0, 0 },
        "Повышает меткость и вероятность критического попадания."
    ) {}
};

// Кинжал Тени
class ShadowDaggerTemplate : public WeaponTemplate {
public:
    ShadowDaggerTemplate() : WeaponTemplate(
        "shadow_dagger", "Кинжал Тени", WeaponType::OneHanded, WeaponRarity::Rare,
        34.0, 3.2,
        { 0, 0, 15.0, 0, 0.10, 0.25, 0, 0 },
        { 0, 0, 2.5, 0, 0.01, 0.04, 0, 0 },
        "Острый зачарованный клинок, наносящий смертоносные удары в уязвимые места."
    ) {}
};

// Меч-Бастион
class BastionBladeTemplate : public WeaponTemplate {
public:
    BastionBladeTemplate() : WeaponTemplate(
        "bastion_blade", "Меч-Бастион", WeaponType::OneHanded, WeaponRarity::Rare,
        30.0, 2.8,
        { 45.0, 0, 0, 18.0, 0, 0, 0, 0 },
        { 8.0, 0, 0, 3.0, 0, 0, 0, 0 },
        "Тяжелый щитовой меч, существенно увеличивающий здоровье и защиту владельца."
    ) {}
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

// ==========================================
// СИСТЕМА КОМПЛЕКТОВ И АРТЕФАКТОВ
// ==========================================
struct ArtifactSetInfo {
    std::string id;
    std::string name;
    std::string description;
    StatArray twoPieceBonus = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
    std::string twoPieceDesc;
    StatArray fourPieceBonus = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
    std::string fourPieceDesc;
};

class ArtifactTemplate : public ItemTemplate {
protected:
    ArtifactType artifactType;
    std::string setId;
    StatArray bonusStats;
    std::string effectDesc;

public:
    ArtifactTemplate(std::string id, std::string name, ArtifactType artType, std::string setId,
                     std::string iconPath, StatArray bonusStats, int buyPrice, int sellPrice, std::string effectDesc = "")
        : ItemTemplate(std::move(id), std::move(name),
                       (artType == ArtifactType::Ring || artType == ArtifactType::Amulet) ? ItemCategory::Jewelry : ItemCategory::Artifact,
                       std::move(iconPath), buyPrice, sellPrice, 1),
          artifactType(artType), setId(std::move(setId)), bonusStats(bonusStats), effectDesc(std::move(effectDesc)) {
    }

    ArtifactType getArtifactType() const { return artifactType; }
    const std::string& getSetId() const { return setId; }
    const StatArray& getBonusStats() const { return bonusStats; }
    const std::string& getEffectDesc() const { return effectDesc; }
};

// ШАБЛОН УМЕНИЙ
class SkillTemplate {
protected:
    std::string id;
    std::string name;
    int cooldown;
    int stacksGranted; // Сколько стаков дает навык
    bool consumesAllStacks; // Расходует ли все стаки при использовании
    std::string description;

public:
    SkillTemplate(std::string id, std::string name, int cooldown, int stacksGranted = 0, bool consumesAllStacks = false, std::string desc = "")
        : id(std::move(id)), name(std::move(name)), cooldown(cooldown), stacksGranted(stacksGranted), consumesAllStacks(consumesAllStacks), description(std::move(desc)) {
    }

    virtual ~SkillTemplate() = default;

    std::string getId() const { return id; }
    std::string getName() const { return name; }
    int getCooldown() const { return cooldown; }
    int getStacksGranted() const { return stacksGranted; }
    bool getConsumesAllStacks() const { return consumesAllStacks; }
    std::string getDescription() const { return description; }

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


// =============================================================================
// ВРАГИ И БОССЫ ЭТАЖЕЙ БАШНИ
// =============================================================================

enum class EnemyRank {
    Normal, // Базовый противник
    Elite,  // Элитный противник
    Boss    // Босс этажа
};

inline const char* getEnemyRankName(EnemyRank rank) {
    switch (rank) {
    case EnemyRank::Normal: return "Обычный";
    case EnemyRank::Elite:  return "ЭЛИТА";
    case EnemyRank::Boss:   return "БОСС ЭТАЖА";
    }
    return "Враг";
}

inline ImVec4 getEnemyRankColor(EnemyRank rank) {
    switch (rank) {
    case EnemyRank::Normal: return ImVec4(0.80f, 0.80f, 0.85f, 1.0f);
    case EnemyRank::Elite:  return ImVec4(1.00f, 0.80f, 0.20f, 1.0f);
    case EnemyRank::Boss:   return ImVec4(1.00f, 0.25f, 0.25f, 1.0f);
    }
    return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
}

// Структура атаки/навыка врага
struct EnemyAttack {
    std::string name;
    std::string description;
    double damageMultiplier = 1.0;
    int cooldown = 0;           // Перезарядка в ходах
    double defensePierce = 0.0; // Пробивание брони (0.0..1.0)
    double lifesteal = 0.0;     // Вампиризм (0.0..1.0)
    bool isAoe = false;         // Атака по всему отряду
};

class EnemyTemplate {
private:
    std::string id;
    std::string name;
    int floor = 1;
    EnemyRank rank = EnemyRank::Normal;
    double baseHp = 100.0;
    double baseAttack = 15.0;
    double baseDefense = 5.0;
    std::string description;
    std::string abilityName;
    std::string abilityDesc;
    std::vector<EnemyAttack> attacks;

    void initAttacks() {
        attacks.clear();
        // 1. Базовая атака врага
        std::string atk1Name = "Обычный выпад";
        if (name.find("крыса") != std::string::npos || name.find("Грызун") != std::string::npos) atk1Name = "Укус резцами";
        else if (name.find("скелет") != std::string::npos || name.find("Скелет") != std::string::npos || name.find("Рыцарь") != std::string::npos) atk1Name = "Удар ржавым клинком";
        else if (name.find("дух") != std::string::npos || name.find("Дух") != std::string::npos || name.find("призрак") != std::string::npos || name.find("Некромант") != std::string::npos) atk1Name = "Сфера праха";
        else if (name.find("жук") != std::string::npos || name.find("паук") != std::string::npos || name.find("ползун") != std::string::npos || name.find("Личинка") != std::string::npos) atk1Name = "Удар жвалами";
        else if (name.find("гончая") != std::string::npos || name.find("волк") != std::string::npos || name.find("Волк") != std::string::npos || name.find("зверь") != std::string::npos) atk1Name = "Раздирающий укус";
        else if (name.find("голем") != std::string::npos || name.find("Голем") != std::string::npos || name.find("страж") != std::string::npos || name.find("Страж") != std::string::npos) atk1Name = "Сокрушительный взмах";
        else if (name.find("культист") != std::string::npos || name.find("Маг") != std::string::npos || name.find("колдун") != std::string::npos) atk1Name = "Энергетический сгусток";
        else atk1Name = "Прямой выпад";

        attacks.push_back({ atk1Name, "Быстрая атака без перезарядки.", 1.0, 0, 0.0, 0.0, false });

        // 2. Особая способность
        std::string atk2Name = !abilityName.empty() ? abilityName : "Особый приём";
        std::string atk2Desc = !abilityDesc.empty() ? abilityDesc : "Усиленный удар врага.";
        double atk2Mult = (rank == EnemyRank::Boss) ? 1.75 : (rank == EnemyRank::Elite ? 1.50 : 1.35);
        double pierce = (atk2Desc.find("брон") != std::string::npos || atk2Desc.find("доспех") != std::string::npos || atk2Desc.find("Игнор") != std::string::npos) ? 0.25 : 0.0;
        double vamp = (atk2Desc.find("жизн") != std::string::npos || atk2Desc.find("кров") != std::string::npos || atk2Desc.find("вампир") != std::string::npos || atk2Desc.find("вытяг") != std::string::npos) ? 0.30 : 0.0;
        attacks.push_back({ atk2Name, atk2Desc, atk2Mult, 2, pierce, vamp, false });

        // 3. Сильнейшая способность / Яростный ультимейт
        if (rank == EnemyRank::Boss) {
            attacks.push_back({ "Гнев владыки (" + name + ")", 
                                "Разрушительная атака владыки этажа по всему отряду игрока.", 
                                2.30, 4, 0.30, 0.15, true });
        } else if (rank == EnemyRank::Elite) {
            attacks.push_back({ "Смертоносный шквал", 
                                "Тяжелая серия ударов элитного воина, пробивающая броню цели.", 
                                1.95, 3, 0.25, 0.0, false });
        } else {
            attacks.push_back({ "Яростный натиск", 
                                "Мощный отчаянный выпад с повышенным уроном.", 
                                1.65, 3, 0.0, 0.0, false });
        }
    }

public:
    EnemyTemplate(std::string id, std::string name, int floor, EnemyRank rank,
                  double hp, double atk, double def,
                  std::string desc, std::string abName = "", std::string abDesc = "")
        : id(std::move(id)), name(std::move(name)), floor(floor), rank(rank),
          baseHp(hp), baseAttack(atk), baseDefense(def),
          description(std::move(desc)), abilityName(std::move(abName)), abilityDesc(std::move(abDesc)) {
        initAttacks();
    }

    const std::string& getId() const { return id; }
    const std::string& getName() const { return name; }
    int getFloor() const { return floor; }
    EnemyRank getRank() const { return rank; }
    double getBaseHp() const { return baseHp; }
    double getBaseAttack() const { return baseAttack; }
    double getBaseDefense() const { return baseDefense; }
    const std::string& getDescription() const { return description; }
    const std::string& getAbilityName() const { return abilityName; }
    const std::string& getAbilityDesc() const { return abilityDesc; }
    const std::vector<EnemyAttack>& getAttacks() const { return attacks; }

    double getHpAtLevel(int level) const {
        double mult = 1.0 + (level - 1) * 0.18;
        return baseHp * mult;
    }
    double getAttackAtLevel(int level) const {
        double mult = 1.0 + (level - 1) * 0.14;
        return baseAttack * mult;
    }
    double getDefenseAtLevel(int level) const {
        double mult = 1.0 + (level - 1) * 0.12;
        return baseDefense * mult;
    }
};

inline void registerAllFloorEnemies(std::unordered_map<std::string, std::shared_ptr<EnemyTemplate>>& db) {
    auto add = [&](const std::string& id, const std::string& name, int floor, EnemyRank rank,
                   double hp, double atk, double def, const std::string& desc,
                   const std::string& abName, const std::string& abDesc) {
        db[id] = std::make_shared<EnemyTemplate>(id, name, floor, rank, hp, atk, def, desc, abName, abDesc);
    };

    // =========================================================================
    // ЭТАЖ 1: ПЕПЕЛЬНЫЙ НЕКРОПОЛЬ (ЗАБРОШЕННЫЙ ЛАГЕРЬ)
    // =========================================================================
    // Базовые (8)
    add("f1_necropolis_rat", "Чумная крыса", 1, EnemyRank::Normal, 60, 10, 2,
        "Голодный мутировавший грызун, разносящий пепельную лихорадку.", "Бешеный укус", "Периодический урон ядом.");
    add("f1_skeleton_scout", "Скелет-разведчик", 1, EnemyRank::Normal, 80, 14, 4,
        "Останки древнего воина с ржавым клинком.", "Костяной выпад", "Игнорирует 20% брони цели.");
    add("f1_ash_crawler", "Пепельный ползун", 1, EnemyRank::Normal, 70, 12, 3,
        "Паукообразная тварь из спрессованного пепла.", "Пепельная паутина", "Замедляет скорость атаки цели.");
    add("f1_tomb_scavenger", "Могильный мародер", 1, EnemyRank::Normal, 95, 13, 5,
        "Одичавший бродяга, разграбляющий склепы павших.", "Удар дубиной", "15% шанс оглушить на 1 ход.");
    add("f1_decayed_zombie", "Гниющий мертвец", 1, EnemyRank::Normal, 130, 9, 6,
        "Неуклюжее, но крайне живучее порождение некроза.", "Трупный смрад", "Снижает атаку атакующего на 10%.");
    add("f1_carrion_beetle", "Трупоядный жук", 1, EnemyRank::Normal, 75, 11, 8,
        "Панцирное насекомое, питающееся мертвечиной.", "Окаменение", "Повышает собственную защиту на 30%.");
    add("f1_restless_spirit", "Беспокойный дух", 1, EnemyRank::Normal, 55, 16, 1,
        "Эфирный призрак узника темницы.", "Леденящий вой", "Наносит магический урон холодом.");
    add("f1_bone_hound", "Костяная гончая", 1, EnemyRank::Normal, 85, 15, 3,
        "Скелет хищной собаки, охотящейся стаями.", "Смертоносный рывок", "Наносит двойной урон при первом ударе.");
    // Элита (3)
    add("f1_elite_grave_keeper", "Элитный могильщик", 1, EnemyRank::Elite, 260, 26, 14,
        "Исполин в лохмотьях с тяжелой железной лопатой.", "Закапывание заживо", "Оглушает цель на 2 хода и наносит мощный урон.");
    add("f1_elite_ash_necromancer", "Пепельный некромант", 1, EnemyRank::Elite, 210, 32, 8,
        "Чернокнижник, поднимающий павших воинов пеплом.", "Призыв скелетов", "Призывает костяного прислужника в бой.");
    add("f1_elite_spectral_knight", "Призрачный рыцарь", 1, EnemyRank::Elite, 290, 28, 16,
        "Фантом рыцаря в мерцающих латах.", "Призрачный раскол", "Разрушает 50% защиты цели на 3 раунда.");
    // Босс (1)
    add("f1_boss_bone_colossus", "Костяной Колосс", 1, EnemyRank::Boss, 750, 45, 25,
        "Исполинское чудовище, собранное некромантией из сотен скелетов узников башни.",
        "Пепельное сокрушение", "Наносит колоссальный урон всей группе и ослепляет пеплом.");

    // =========================================================================
    // ЭТАЖ 2: ОАЗИС ТИШИНЫ
    // =========================================================================
    // Базовые (8)
    add("f2_thorn_stalker", "Шипастый ловец", 2, EnemyRank::Normal, 110, 20, 7,
        "Хищная лоза, скрывающаяся в подземном подлеске.", "Шипастый залп", "Выпускает веер игл по двум целям.");
    add("f2_lake_lurker", "Озерный удильщик", 2, EnemyRank::Normal, 125, 22, 9,
        "Земноводный гуманоид с острыми как бритва плавниками.", "Удар плавником", "Шанс вызвать глубокое кровотечение.");
    add("f2_swamp_toad", "Болотная жаба", 2, EnemyRank::Normal, 140, 18, 8,
        "Раздувшаяся ядовитая амфибия с кислотной слизью.", "Кислотный плевок", "Снижает броню цели на 15%.");
    add("f2_oasis_viper", "Пещерная гадюка", 2, EnemyRank::Normal, 95, 25, 6,
        "Молниеносная змея с паралитическим ядом.", "Нейротоксин", "Парализует цель на 1 ход.");
    add("f2_corrupted_dryad", "Оскверненная дриада", 2, EnemyRank::Normal, 115, 21, 8,
        "Бывшая хранительница рощи, поглощенная тьмой башни.", "Опутывающие корни", "Обездвиживает и наносит урон природы.");
    add("f2_drowned_wanderer", "Утопленник оазиса", 2, EnemyRank::Normal, 160, 19, 10,
        "Тело незадачливого путника, скованное подводной тиной.", "Водный захват", "Притягивает и душит жертву водорослями.");
    add("f2_spore_spitter", "Споровик-плевун", 2, EnemyRank::Normal, 105, 23, 7,
        "Грибной полип, выбрасывающий облака токсичной пыльцы.", "Споровый взрыв", "Накладывает яд на всех врагов рядом.");
    add("f2_mire_crab", "Илистый краб", 2, EnemyRank::Normal, 150, 17, 18,
        "Речной панцирник с непробиваемой хитиновой броней.", "Сокрушающая клешня", "Наносит критический урон бронированным целям.");
    // Элита (3)
    add("f2_elite_overgrown_golem", "Заросший страж", 2, EnemyRank::Elite, 420, 36, 26,
        "Древний каменный голем, покрытый толстым слоем мха и лиан.", "Природная регенерация", "Восстанавливает 10% здоровья каждый ход.");
    add("f2_elite_siren_of_depths", "Сирена глубин", 2, EnemyRank::Elite, 340, 42, 16,
        "Прекрасная и ужасающая повелительница подземных вод.", "Чарующий вой", "Очаровывает одного из бойцов, заставляя пропустить ход.");
    add("f2_elite_ancient_hydra", "Болотная гидра", 2, EnemyRank::Elite, 460, 40, 20,
        "Трехглавая рептилия, обитающая в самых глубоких гротах.", "Тройной укус", "Наносит 3 быстрых последовательных удара.");
    // Босс (1)
    add("f2_boss_lotus_matriarch", "Матриарх Черного Лотоса", 2, EnemyRank::Boss, 1150, 58, 32,
        "Колоссальный плотоядный цветок, чьи ядовитые корни оплели весь второй этаж.",
        "Пыльца иллюзий", "Усыпляет весь отряд и вызывает всплеск кислотных корней.");

    // =========================================================================
    // ЭТАЖ 3: ЗАТОНУВШИЙ АКРОПОЛЬ
    // =========================================================================
    // Базовые (8)
    add("f3_triton_spearman", "Тритон-копейщик", 3, EnemyRank::Normal, 160, 28, 14,
        "Воин затопленного города с зазубренным трезубцем.", "Пронзающий выпад", "Наносит повышенный колющий урон.");
    add("f3_abyssal_jellyfish", "Бездная медуза", 3, EnemyRank::Normal, 130, 32, 10,
        "Полупрозрачный организм, излучающий электрическое сияние.", "Электрошок", "Шанс оглушить молнией.");
    add("f3_sunken_myrmidon", "Утонувший мирмидонец", 3, EnemyRank::Normal, 190, 26, 18,
        "Античный воин в покрытых ракушками бронзовых доспехах.", "Фаланга", "Повышает броню при блокировании.");
    add("f3_depth_strider", "Глубинный долгоног", 3, EnemyRank::Normal, 145, 30, 12,
        "Паукообразное ракообразное на длинных конечностях-спицах.", "Рассекающая игла", "Игнорирует защитные стойки.");
    add("f3_coral_golem", "Коралловый голем", 3, EnemyRank::Normal, 220, 24, 22,
        "Ожившая колония рифовых полипов небывалой прочности.", "Коралловые шипы", "Отражает 25% контактного урона.");
    add("f3_barnacle_hulk", "Моллюсковый громила", 3, EnemyRank::Normal, 240, 23, 20,
        "Мутировавший гигант с панцирем из окаменевших моллюсков.", "Приливный удар", "Отбрасывает цель назад.");
    add("f3_tide_weaver", "Ткач приливов", 3, EnemyRank::Normal, 150, 34, 11,
        "Жрец глубин, манипулирующий давлением воды.", "Водный серп", "Магический урон режущей струей воды.");
    add("f3_chasm_eel", "Угорь разлома", 3, EnemyRank::Normal, 135, 31, 13,
        "Змеевидная рыба с мощным биоэлектрическим зарядом.", "Цепная молния", "Поражает током до двух целей.");
    // Элита (3)
    add("f3_elite_naga_commander", "Командир наг", 3, EnemyRank::Elite, 520, 48, 28,
        "Четырехрукий полководец морского легиона с парными саблями.", "Водоворот клинков", "Наносит серию ударов по всем передовым бойцам.");
    add("f3_elite_kraken_spawn", "Отпрыск Кракена", 3, EnemyRank::Elite, 580, 45, 24,
        "Массивное головоногое чудовище со щупальцами-хлыстами.", "Удушающие объятия", "Обездвиживает и сжимает цель каждый раунд.");
    add("f3_elite_drowned_oracle", "Оракул глубин", 3, EnemyRank::Elite, 440, 52, 20,
        "Морская жрица, черпающая силу из затонувших святилищ.", "Водный кокон", "Накладывает на союзников поглощающий щит.");
    // Босс (1)
    add("f3_boss_leviathan_lord", "Левиафан Глубин", 3, EnemyRank::Boss, 1550, 75, 40,
        "Древний владыка затонувшего полиса, повелевающий приливами и цунами.",
        "Цунами Бездны", "Смывает строй врагов колоссальной волной, оглушая весь отряд.");

    // =========================================================================
    // ЭТАЖ 4: РАСКАЛЕННЫЕ МАГИСТРАЛИ
    // =========================================================================
    // Базовые (8)
    add("f4_magma_imp", "Магматический бес", 4, EnemyRank::Normal, 180, 38, 16,
        "Проворное огненное создание, метающее комья горящей смолы.", "Взрыв лавы", "Взрывается при смерти, поджигая врагов.");
    add("f4_fire_salamander", "Огненная саламандра", 4, EnemyRank::Normal, 210, 35, 20,
        "Ящерица из лавовых потоков, раскаленная добела.", "Пылающий след", "Оставляет огненный шлейф с ожогами.");
    add("f4_slag_golem", "Шлаковый автоматон", 4, EnemyRank::Normal, 280, 32, 28,
        "Сгусток остывающего шлака и шестерен из плавилен.", "Оплавление брони", "Разрушает броню цели на 20%.");
    add("f4_duergar_berserker", "Дворф-пеклорожденный", 4, EnemyRank::Normal, 250, 40, 22,
        "Безумный кузнец подземных недр с тяжелой секирой.", "Огненная секира", "Наносит критический урон от огня.");
    add("f4_cinder_wasp", "Пепельная оса", 4, EnemyRank::Normal, 160, 42, 14,
        "Раскаленное насекомое с жалом из обсидиана.", "Прожигающий укол", "Сильное периодическое горение.");
    add("f4_flame_hound", "Пылающий цербер", 4, EnemyRank::Normal, 220, 39, 18,
        "Охотничий пес из преисподней, изрыгающий огонь.", "Огненное дыхание", "Конусная атака огнем.");
    add("f4_magma_ooze", "Лавовый слизень", 4, EnemyRank::Normal, 260, 30, 24,
        "Живая лужа бурлящей магмы, поглощающая металл.", "Поглощение тепла", "Снижает сопротивление магии огня.");
    add("f4_boiler_construct", "Паровой котел", 4, EnemyRank::Normal, 290, 31, 26,
        "Шагающий бойлер под критическим давлением пара.", "Сброс пара", "Обжигает горячим паром и ослепляет.");
    // Элита (3)
    add("f4_elite_infernal_centurion", "Инфернальный центурион", 4, EnemyRank::Elite, 680, 56, 36,
        "Элитный командир пламенной когорты в базальтовых латах.", "Пламенный щит", "Поглощает урон и парирует огненной волной.");
    add("f4_elite_pyro_geomancer", "Пиромант недр", 4, EnemyRank::Elite, 540, 65, 24,
        "Маг, управляющий извержениями магматических жил.", "Столб магмы", "Вызывает из-под земли фонтан лавы под случайным врагом.");
    add("f4_elite_volcanic_drake", "Вулканический драконид", 4, EnemyRank::Elite, 720, 60, 32,
        "Окрыленный ящер из самого жерла магматической магистрали.", "Метеоритное пике", "Пикирует сверху с огненным взрывом.");
    // Босс (1)
    add("f4_boss_ignis_titan", "Игнис, Пожиратель Лавы", 4, EnemyRank::Boss, 2100, 95, 48,
        "Древний титан из застывшего обсидиана с пульсирующим мини-солнцем в груди.",
        "Вулканический катаклизм", "Заливает арену лавой, нанося урон огнем каждый раунд.");

    // =========================================================================
    // ЭТАЖ 5: ЖЕЛЕЗНЫЙ КАРАНТИН
    // =========================================================================
    // Базовые (8)
    add("f5_plague_mutant", "Чумной мутант", 5, EnemyRank::Normal, 240, 45, 22,
        "Жертва генетических опытов с деформированными костяными серпами.", "Заражение крови", "Снижает эффективность лечения на 50%.");
    add("f5_surgical_drone", "Хирургический дрон", 5, EnemyRank::Normal, 200, 52, 18,
        "Левитирующий механизм с вибрирующими лазерными скальпелями.", "Прицельный надрез", "Гарантированный критический урон.");
    add("f5_infected_warden", "Зараженный надзиратель", 5, EnemyRank::Normal, 310, 42, 30,
        "Бывший охранник изолятора с тяжелым щитом и электрошокером.", "Шоковый разряд", "Оглушает цель на 1 ход.");
    add("f5_toxic_crawler", "Токсичный ползун", 5, EnemyRank::Normal, 220, 48, 20,
        "Многоногий биомутант, распыляющий токсичные химикаты.", "Химический аэрозоль", "Отравляет воздух в радиусе 3 метров.");
    add("f5_cyber_hound", "Кибер-ищейка", 5, EnemyRank::Normal, 260, 49, 24,
        "Механизированная собака с сенсорами жизненных показателей.", "Сканирование бреши", "Повышает крит союзников по выбранной цели.");
    add("f5_bio_leech", "Био-пиявка", 5, EnemyRank::Normal, 230, 44, 21,
        "Лабораторный паразит, выкачивающий кровь и силу.", "Высасывание жизни", "Лечится на 100% от нанесенного урона.");
    add("f5_serum_maniac", "Сывороточный берсерк", 5, EnemyRank::Normal, 340, 50, 16,
        "Накачанный стимуляторами подопытный, не чувствующий боли.", "Безудержная ярость", "Увеличивает атаку по мере потери здоровья.");
    add("f5_scrap_turret", "Карантинная турель", 5, EnemyRank::Normal, 280, 46, 32,
        "Автоматическая станция подавления нарушителей периметра.", "Пулеметная очередь", "Серия из 4 быстрых выстрелов.");
    // Элита (3)
    add("f5_elite_amalgam_beast", "Биомеханический амальгам", 5, EnemyRank::Elite, 850, 70, 38,
        "Сросшаяся масса из тел трех подопытных и стального каркаса.", "Раздавливание", "Хватает двоих бойцов и сталкивает их лбами.");
    add("f5_elite_head_surgeon", "Главный хирург Карантина", 5, EnemyRank::Elite, 720, 80, 30,
        "Обезумевший кибернетический врач с набором ампутационных пил.", "Наркоз без согласия", "Усыпляет цель и наносит точный смертоносный разрез.");
    add("f5_elite_quarantine_enforcer", "Тяжелый штурмовик", 5, EnemyRank::Elite, 920, 68, 44,
        "Тяжелый робот-ликвидатор со встроенным напалмовым огнеметом.", "Очищение огнем", "Сплошная волна напалма по всем врагам.");
    // Босс (1)
    add("f5_boss_subject_zero", "Объект Ноль", 5, EnemyRank::Boss, 2750, 115, 55,
        "Первый и самый смертоносный генетический монстр лаборатории, обладающий адаптивным ДНК.",
        "Протокол Стерилизации", "Распыляет смертоносный газ и переходит в режим гипер-мутации.");

    // =========================================================================
    // ЭТАЖ 6: ИЗУМРУДНЫЙ СВОД
    // =========================================================================
    // Базовые (8)
    add("f6_jungle_raptor", "Джунглевый раптор", 6, EnemyRank::Normal, 310, 58, 28,
        "Смертоносный хищник первобытных лесов под куполом.", "Стайный наскок", "Наносит повышенный урон, если рядом другие хищники.");
    add("f6_giant_mantis", "Гигантский богомол", 6, EnemyRank::Normal, 290, 65, 26,
        "Огромное насекомое с пилообразными передними лапами.", "Обезглавливание", "Шанс нанести критический удар x3.");
    add("f6_spore_gorilla", "Споровая горилла", 6, EnemyRank::Normal, 420, 52, 34,
        "Могучий примат, симбиотически сросшийся с ядовитыми грибами.", "Удар по наковальне", "Оглушает переднюю линию.");
    add("f6_vine_strangler", "Лиана-душитель", 6, EnemyRank::Normal, 330, 55, 30,
        "Плотоядное растение, свисающее с крон исполинских деревьев.", "Удушающий капкан", "Лишает цель возможности применять навыки.");
    add("f6_toxic_hornet", "Ядовитый шершень", 6, EnemyRank::Normal, 260, 68, 22,
        "Крылатый агрессор со смертоносной дозой токсина в жале.", "Нейропаралич", "Замедляет реакцию на 40%.");
    add("f6_primal_hunter", "Первобытный следопыт", 6, EnemyRank::Normal, 320, 60, 27,
        "Охотник изолированного купольного племени.", "Отравленный дротик", "Точный выстрел с сильным ядом.");
    add("f6_carnivorous_flora", "Плотоядный росянник", 6, EnemyRank::Normal, 360, 53, 32,
        "Ловушка из клейких листьев и пищеварительных ферментов.", "Заглатывание", "Поглощает цель на 1 ход.");
    add("f6_chameleon_stalker", "Хамелеон-охотник", 6, EnemyRank::Normal, 280, 64, 25,
        "Мастер камуфляжа с длинным липким языком.", "Удар из засады", "Первый удар из невидимости наносит двойной урон.");
    // Элита (3)
    add("f6_elite_apex_carnotaur", "Карнотавр-вершитель", 6, EnemyRank::Elite, 1050, 88, 48,
        "Бронированный динозавр с рогами из костяного монолита.", "Неудержимый таран", "Сбивает с ног и разламывает щиты.");
    add("f6_elite_swarm_queen", "Королева роя инсектов", 6, EnemyRank::Elite, 920, 95, 38,
        "Владычица многотысячного роя шершней и жуков Свода.", "Призыв роя", "Призывает стаю шершней каждый второй ход.");
    add("f6_elite_ancient_treant", "Древний энт джунглей", 6, EnemyRank::Elite, 1200, 78, 56,
        "Оживший исполинский баобаб, помнящий строительство башни.", "Гнев чащи", "Поднимает частокол корней, защищая союзников.");
    // Босс (1)
    add("f6_boss_emerald_behemoth", "Изумрудный Бегемот", 6, EnemyRank::Boss, 3500, 138, 68,
        "Исполинский древний титан флоры и фауны, сокрушающий скалы своим панцирем.",
        "Землетрясение Свода", "Сотрясает весь купол, обрушивая многотонные каменные глыбы.");

    // =========================================================================
    // ЭТАЖ 7: ЗАСТЫВШЕЕ ВРЕМЯ
    // =========================================================================
    // Базовые (8)
    add("f7_frost_wraith", "Ледяной призрак", 7, EnemyRank::Normal, 380, 74, 34,
        "Бестелесный дух, замедляющий само течение времени вокруг.", "Аура абсолютного нуля", "Снижает скорость действий всей группы.");
    add("f7_chrono_automaton", "Хроно-автоматон", 7, EnemyRank::Normal, 420, 70, 42,
        "Механизм с маятниковым сердцем из вечного льда.", "Откат позиции", "Возвращает уровень здоровья к состоянию прошлого хода.");
    add("f7_ice_shard_gargoyle", "Горгулья ледяного пика", 7, EnemyRank::Normal, 390, 78, 38,
        "Каменно-ледяное чудище с крыльями из острых кристаллов.", "Град осколков", "Обстреливает всех врагов ледяными иглами.");
    add("f7_frozen_remnant", "Замерзший легионер", 7, EnemyRank::Normal, 480, 68, 48,
        "Рыцарь, вмерзший в ледник тысячи лет назад и пробужденный магией.", "Ледяной щит", "Поглощает 30% входящего урона.");
    add("f7_blizzard_wolf", "Волк метели", 7, EnemyRank::Normal, 370, 76, 32,
        "Хищник, сотканный из вьюги и снежной пыли.", "Дыхание пурги", "Ослепляет метелью, снижая меткость на 35%.");
    add("f7_crystal_golem", "Кристальный голем", 7, EnemyRank::Normal, 510, 66, 52,
        "Конструкт из чистого хроно-кристалла.", "Зеркальное отражение", "Отражает магические заклинания обратно во врага.");
    add("f7_time_dilation_wisp", "Огонек замедления", 7, EnemyRank::Normal, 320, 85, 28,
        "Сгусток хрономантической энергии, пожирающий чужие секунды.", "Кража времени", "Отнимает скорость у врага и отдает боссу.");
    add("f7_glacier_beetle", "Ледниковый жук", 7, EnemyRank::Normal, 440, 65, 54,
        "Огромный жук с панцирем из многовекового тороса.", "Ледяная броня", "Замораживает атакующих бойцов ближнего боя.");
    // Элита (3)
    add("f7_elite_chrono_knight", "Рыцарь Вечности", 7, EnemyRank::Elite, 1350, 105, 60,
        "Паладин вне времени, закованный в мерцающие хроно-доспехи.", "Временная петля", "При получении летального урона воскресает с 30% HP.");
    add("f7_elite_frost_archmage", "Морозный архимаг", 7, EnemyRank::Elite, 1100, 120, 42,
        "Магистр льда древней академии башни.", "Ледяная комета", "Обрушивает колоссальную глыбу льда, оглушая на 2 хода.");
    add("f7_elite_temporal_stalker", "Хроно-ассасин", 7, EnemyRank::Elite, 1180, 115, 48,
        "Убийца, перемещающийся сквозь застывшие мгновения.", "Мгновенное рассечение", "Три удара в обход физической брони.");
    // Босс (1)
    add("f7_boss_chronos_the_frozen", "Кронос Застывший", 7, EnemyRank::Boss, 4400, 165, 78,
        "Владыка песочных часов башни, наполовину скованный вечным космическим льдом.",
        "Остановка Времени", "Останавливает время на 1 полный раунд, проводя серию свободных атак.");

    // =========================================================================
    // ЭТАЖ 8: ИЛЛЮЗОРНЫЙ МЕГАПОЛИС
    // =========================================================================
    // Базовые (8)
    add("f8_neon_stalker", "Неоновый сталкер", 8, EnemyRank::Normal, 460, 92, 44,
        "Кибернетический наемник с парными плазменными клинками.", "Лазерный вихрь", "Наносит рассекающий урон чистой плазмой.");
    add("f8_holo_phantom", "Голографический фантом", 8, EnemyRank::Normal, 390, 105, 35,
        "Проекция павшего бойца, сбивающая прицелы сенсоров.", "Голографический обман", "50% шанс уклониться от любой атаки.");
    add("f8_enforcer_android", "Андроид-силовик", 8, EnemyRank::Normal, 580, 88, 55,
        "Полицейский робот из титанового сплава с силовым щитом.", "Разгон демонстраций", "Таран щитом с оглушением на 1 ход.");
    add("f8_cyber_cultist", "Кибер-культист", 8, EnemyRank::Normal, 440, 96, 40,
        "Фанатик машинного разума, взламывающий аугментации врагов.", "Нейровзлом", "Накладывает безмолвие и перегрузку.");
    add("f8_glitch_hound", "Глитч-гончая", 8, EnemyRank::Normal, 430, 98, 42,
        "Искаженная цифровой аномалией боевая собака.", "Мерцающий скачок", "Телепортируется за спину слабейшего героя.");
    add("f8_drone_sniper", "Дрон-снайпер", 8, EnemyRank::Normal, 370, 115, 32,
        "Высокоточная летающая платформа с рейлганом.", "Импульсный выстрел", "Пробивает 80% брони цели.");
    add("f8_plasma_construct", "Плазменный конструкт", 8, EnemyRank::Normal, 520, 90, 48,
        "Энергетическое ядро, управляющее защитными полями.", "Плазменный перегрев", "Наносит урон огнем и электричеством.");
    add("f8_data_leech", "Пожиратель данных", 8, EnemyRank::Normal, 410, 94, 38,
        "Нановирусный рой, высасывающий энергию навыков.", "Опустошение ресурса", "Сжигает шкалу энергии/маны цели.");
    // Элита (3)
    add("f8_elite_cyborg_warlord", "Киборг-полководец", 8, EnemyRank::Elite, 1650, 135, 72,
        "Тяжелобронированный лидер кибернетического синдиката.", "Плазменный залп", "Ракетный обстрел по всем бойцам группы.");
    add("f8_elite_neural_hacker", "Нейро-хакер Мегаполиса", 8, EnemyRank::Elite, 1380, 145, 52,
        "Гений виртуальной сети, переписывающий реальность на лету.", "Перехват контроля", "Заставляет одного героя атаковать союзника.");
    add("f8_elite_matrix_assassin", "Ассасин Матрицы", 8, EnemyRank::Elite, 1480, 140, 60,
        "Сверхбыстрый ниндзя с мономолекулярной катаной.", "Удар сквозь измерения", "Игнорирует любые щиты и бессмертие.");
    // Босс (1)
    add("f8_boss_overseer_ai", "Архитектор «Нексус»", 8, EnemyRank::Boss, 5400, 195, 92,
        "Центральный суперкомпьютер башни, материализовавший цифровое тело божества.",
        "Системный Перезапуск", "Сбрасывает все баффы игроков и перегружает арену смертельным током.");

    // =========================================================================
    // ЭТАЖ 9: НЕБЕСНЫЙ СОБОР
    // =========================================================================
    // Базовые (8)
    add("f9_radiant_zealot", "Лучезарный фанатик", 9, EnemyRank::Normal, 540, 115, 52,
        "Воин света с пылающим двуручным мечом веры.", "Карающий меч", "Наносит повышенный священный урон.");
    add("f9_cherub_archer", "Херувим-лучник", 9, EnemyRank::Normal, 460, 128, 42,
        "Крылатый стрелок со световыми стрелами правосудия.", "Стрела правосудия", "Снижает урон цели на 25%.");
    add("f9_stained_glass_golem", "Витражный голем", 9, EnemyRank::Normal, 660, 108, 65,
        "Конструкт из освященного соборного витражного стекла.", "Ослепительный блик", "Ослепляет атакующих на 2 хода.");
    add("f9_light_acolyte", "Служитель света", 9, EnemyRank::Normal, 500, 110, 48,
        "Монах Небесного Собора, исцеляющий братьев по вере.", "Святое исцеление", "Восстанавливает здоровье раненому союзнику.");
    add("f9_seraph_sentinel", "Страж-серафим", 9, EnemyRank::Normal, 620, 118, 58,
        "Четырехкрылый рыцарь с золотым ростовым щитом.", "Божественный заслон", "Принимает урон за союзников на себя.");
    add("f9_blind_inquisitor", "Слепой инквизитор", 9, EnemyRank::Normal, 530, 125, 50,
        "Судья еретиков, видящий души сквозь грехи.", "Суд над грешником", "Наносит урон, пропорциональный атаке цели.");
    add("f9_golden_griffon", "Золотой грифон", 9, EnemyRank::Normal, 570, 120, 54,
        "Благородный зверь небесных высот.", "Небесный клекот", "Оглушает передний ряд мощной звуковой волной.");
    add("f9_sacred_censer", "Парящая кадильница", 9, EnemyRank::Normal, 480, 112, 60,
        "Священный артефакт, источающий очищающий ладан.", "Очищающий ладан", "Снимает отрицательные эффекты со своих войск.");
    // Элита (3)
    add("f9_elite_high_templar", "Высший тамплиер", 9, EnemyRank::Elite, 1950, 160, 85,
        "Верховный паладин в сияющих доспехах из небесного золота.", "Молот Возмездия", "Сокрушает броню и наносит тяжелый святой урон.");
    add("f9_elite_valkyrie_arbiter", "Валькирия-судья", 9, EnemyRank::Elite, 1800, 172, 74,
        "Крылатая дева битвы с копьем из чистого света.", "Божественное пике", "Пикирует сквозь строй врагов, раня всех бойцов.");
    add("f9_elite_archangel_avenger", "Мстительный архангел", 9, EnemyRank::Elite, 2050, 168, 80,
        "Шестикрылый вестник гнева небес с огненным мечом.", "Пламя Очищения", "Испепеляет святым пламенем, нанося огромный урон.");
    // Босс (1)
    add("f9_boss_archon_luminaris", "Архонт Люминарис", 9, EnemyRank::Boss, 6600, 235, 108,
        "Патриарх Небесного Собора, полубог в короне из застывших звездных лучей.",
        "Гнев Небес", "Обрушивает столб ослепительного звездного света, нанося катастрофический урон.");

    // =========================================================================
    // ЭТАЖ 10: СЕРДЦЕ БАШНИ (ТРОН ПЕРВОРОДНОГО)
    // =========================================================================
    // Базовые (8)
    add("f10_void_walker", "Странник Пустоты", 10, EnemyRank::Normal, 680, 145, 65,
        "Существо из первичного вакуума, расщепляющее материю.", "Искажение пространства", "Игнорирует 50% любой защиты.");
    add("f10_chaos_spawn", "Порождение Хаоса", 10, EnemyRank::Normal, 620, 155, 58,
        "Нестабильный сгусток первичной материи башни.", "Случайная мутация", "Случайно повышает одну из своих характеристик на 40%.");
    add("f10_astral_horror", "Астральный кошмар", 10, EnemyRank::Normal, 590, 162, 54,
        "Тварь из самых глубоких уголков космического безумия.", "Пожирание рассудка", "Вводит бойца в замешательство на 2 хода.");
    add("f10_entropy_golem", "Голем Энтропии", 10, EnemyRank::Normal, 820, 138, 80,
        "Исполин из черных дыр и гравитационных аномалий.", "Гравитационный захват", "Притягивает врагов в одну точку, нанося урон.");
    add("f10_singularity_wisp", "Огонек Сингулярности", 10, EnemyRank::Normal, 510, 175, 48,
        "Микроскопическая черная дыра, поглощающая свет и жизнь.", "Микро-коллапс", "Взрывается при получении удара.");
    add("f10_oblivion_knight", "Рыцарь Забвения", 10, EnemyRank::Normal, 760, 148, 72,
        "Воин в доспехах из антиматерии с клинком небытия.", "Клинок Забвения", "Стирает баффы цели при каждом ударе.");
    add("f10_rift_stalker", "Охотник Разлома", 10, EnemyRank::Normal, 640, 158, 60,
        "Хищник, шагающий сквозь щели между измерениями.", "Шаг в небытие", "Неуязвим на 1 ход после атаки.");
    add("f10_cosmic_leech", "Космическая пиявка", 10, EnemyRank::Normal, 550, 150, 56,
        "Паразит ткани мироздания, поглощающий сущность душ.", "Поглощение души", "Крадет 20% максимального здоровья цели.");
    // Элита (3)
    add("f10_elite_abyss_monarch", "Монарх Бездны", 10, EnemyRank::Elite, 2400, 205, 98,
        "Владыка темных звездных скоплений, повелевающий гравитацией.", "Сфера Сингулярности", "Стягивает всех героев в центр с колоссальным уроном.");
    add("f10_elite_entropy_herald", "Глашатай Энтропии", 10, EnemyRank::Elite, 2200, 218, 88,
        "Вестник окончательной гибели вселенной.", "Аура Распада", "Наносит периодический чистый урон сквозь любые щиты.");
    add("f10_elite_primordial_warden", "Первородный Страж Трона", 10, EnemyRank::Elite, 2650, 198, 105,
        "Древнейший защитник покоев Творца Башни.", "Абсолютный Монолит", "Иммунитет к контролю и оглушению.");
    // Босс (1)
    add("f10_boss_primordial_origin", "Первородный Создатель", 10, EnemyRank::Boss, 8800, 290, 130,
        "Демиург, воздвигший Башню как горнило для создания высшей расы. Повелитель пространства и времени.",
        "Большой Взрыв: Разрыв Реальности", "Стирает материю реальности, нанося разрушительный космический урон всему живому.");
}

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
    std::unordered_map<std::string, std::shared_ptr<EnemyTemplate>> enemies;
    std::unordered_map<std::string, std::shared_ptr<ArtifactTemplate>> artifacts;
    std::unordered_map<std::string, ArtifactSetInfo> artifactSets;

    void registerArtifact(const std::string& id, const std::string& name, ArtifactType artType,
                          const std::string& setId, const std::string& iconPath,
                          const StatArray& bonusStats, int buyPrice = 160, int sellPrice = 80,
                          const std::string& effectDesc = "") {
        auto art = std::make_shared<ArtifactTemplate>(id, name, artType, setId, iconPath, bonusStats, buyPrice, sellPrice, effectDesc);
        artifacts[id] = art;
        items[id] = art;
    }

    void registerArtifactSet(const std::string& id, const std::string& name, const std::string& desc,
                             const StatArray& twoBonus, const std::string& twoDesc,
                             const StatArray& fourBonus, const std::string& fourDesc) {
        artifactSets[id] = { id, name, desc, twoBonus, twoDesc, fourBonus, fourDesc };
    }


    void registerItem(const std::string& id, const std::string& name, ItemCategory category,
        const std::string& iconPath, int buyPrice = 25, int sellPrice = 20, int maxStack = 99) {
        items[id] = std::make_shared<ItemTemplate>(id, name, category, iconPath, buyPrice, sellPrice, maxStack);
    }

    void init() {
        registerAllFloorEnemies(enemies);
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

        // Дополнительное оружие
        weapons["apprentice_staff"] = std::make_shared<ApprenticeStaffTemplate>();
        weapons["hunter_bow"] = std::make_shared<HunterBowTemplate>();
        weapons["shadow_dagger"] = std::make_shared<ShadowDaggerTemplate>();
        weapons["bastion_blade"] = std::make_shared<BastionBladeTemplate>();

        registerItem("apprentice_staff", "Посох Ученика", ItemCategory::Weapon, "assets/icons/apprentice_staff.png", 90, 45);
        registerItem("hunter_bow", "Охотничий Лук", ItemCategory::Weapon, "assets/icons/hunter_bow.png", 100, 50);
        registerItem("shadow_dagger", "Кинжал Тени", ItemCategory::Weapon, "assets/icons/shadow_dagger.png", 140, 70);
        registerItem("bastion_blade", "Меч-Бастион", ItemCategory::Weapon, "assets/icons/bastion_blade.png", 130, 65);

        // ==========================================
        // ИНИЦИАЛИЗАЦИЯ КОМПЛЕКТОВ АРТЕФАКТОВ
        // ==========================================
        // КОМПЛЕКТ 1: «СТРАЖ ВЕЧНОСТИ» (Защита / Танкование)
        registerArtifactSet("set_guardian", "Страж Вечности",
            "Древний латный комплект рыцарей Башни. Дарует колоссальный запас стойкости и щитов.",
            { 60.0, 0, 0, 15.0, 0, 0, 0, 0 }, "[2 предм.]: +60 HP, +15 Защиты",
            { 150.0, 0, 0, 35.0, 0, 0, 0, 0.10 }, "[4 предм.]: +150 HP, +35 Защиты, +10% Сопротивления урону");

        registerArtifact("guardian_helm", "Шлем Стража", ArtifactType::Headgear, "set_guardian",
            "assets/icons/guardian_helm.png", { 40.0, 0, 0, 10.0, 0, 0, 0, 0 }, 180, 90, "+40 HP, +10 Защиты");
        registerArtifact("guardian_armor", "Латы Стража", ArtifactType::Costume, "set_guardian",
            "assets/icons/guardian_armor.png", { 80.0, 0, 0, 20.0, 0, 0, 0, 0 }, 240, 120, "+80 HP, +20 Защиты");
        registerArtifact("guardian_amulet", "Амулет Стойкости", ArtifactType::Amulet, "set_guardian",
            "assets/icons/guardian_amulet.png", { 30.0, 0, 0, 8.0, 0, 0, 0, 0 }, 160, 80, "+30 HP, +8 Защиты");
        registerArtifact("guardian_ring", "Перстень Защитника", ArtifactType::Ring, "set_guardian",
            "assets/icons/guardian_ring.png", { 25.0, 0, 0, 10.0, 0, 0, 0, 0 }, 150, 75, "+25 HP, +10 Защиты");

        // КОМПЛЕКТ 2: «ПЕПЕЛЬНЫЙ ЧАРОДЕЙ» (Магия / Инферно)
        registerArtifactSet("set_ash", "Пепельный Чародей",
            "Реликвии повелителей пламени. Насыщают заклинания яростной огненной мощью.",
            { 0, 45.0, 20.0, 0, 0, 0, 0, 0 }, "[2 предм.]: +45 Маны, +20 Силы атаки",
            { 0, 90.0, 45.0, 0, 0, 0, 0, 0.15 }, "[4 предм.]: +90 Маны, +45 Атаки, +15% Бонус урона умений");

        registerArtifact("ash_hood", "Капюшон Пепла", ArtifactType::Headgear, "set_ash",
            "assets/icons/ash_hood.png", { 0, 30.0, 8.0, 0, 0, 0, 0, 0 }, 180, 90, "+30 Маны, +8 Атаки");
        registerArtifact("ash_robe", "Мантия Инферно", ArtifactType::Costume, "set_ash",
            "assets/icons/ash_robe.png", { 0, 60.0, 18.0, 0, 0, 0, 0, 0 }, 240, 120, "+60 Маны, +18 Атаки");
        registerArtifact("ash_amulet", "Кулон Пламени", ArtifactType::Amulet, "set_ash",
            "assets/icons/ash_amulet.png", { 0, 25.0, 10.0, 0, 0, 0, 0, 0 }, 160, 80, "+25 Маны, +10 Атаки");
        registerArtifact("ash_ring", "Кольцо Искры", ArtifactType::Ring, "set_ash",
            "assets/icons/ash_ring.png", { 0, 20.0, 7.0, 0, 0, 0, 0, 0 }, 150, 75, "+20 Маны, +7 Атаки");

        // КОМПЛЕКТ 3: «ЯРОСТЬ БЕРСЕРКА» (Крит / Физический урон)
        registerArtifactSet("set_berserker", "Ярость Берсерка",
            "Окровавленные реликвии гладиаторов арены. Повышают силу атаки и сокрушительный крит.",
            { 0, 0, 30.0, 0, 0.12, 0, 0, 0 }, "[2 предм.]: +30 Атаки, +12% Крит. шанса",
            { 0, 0, 65.0, 0, 0.18, 0.35, 0, 0 }, "[4 предм.]: +65 Атаки, +18% Крит. шанса, +35% Крит. урона");

        registerArtifact("berserker_mask", "Маска Берсерка", ArtifactType::Headgear, "set_berserker",
            "assets/icons/berserker_mask.png", { 0, 0, 12.0, 0, 0.06, 0, 0, 0 }, 180, 90, "+12 Атаки, +6% Крит");
        registerArtifact("berserker_harness", "Доспех Ярости", ArtifactType::Costume, "set_berserker",
            "assets/icons/berserker_harness.png", { 0, 0, 25.0, 0, 0.10, 0, 0, 0 }, 240, 120, "+25 Атаки, +10% Крит");
        registerArtifact("berserker_amulet", "Амулет Крови", ArtifactType::Amulet, "set_berserker",
            "assets/icons/berserker_amulet.png", { 0, 0, 10.0, 0, 0.05, 0, 0, 0 }, 160, 80, "+10 Атаки, +5% Крит");
        registerArtifact("berserker_ring", "Перстень Резни", ArtifactType::Ring, "set_berserker",
            "assets/icons/berserker_ring.png", { 0, 0, 14.0, 0, 0.06, 0, 0, 0 }, 150, 75, "+14 Атаки, +6% Крит");

        // КОМПЛЕКТ 4: «ТЕНЬ РАЗЛОМА» (Вампиризм / Ловкость)
        registerArtifactSet("set_rift", "Тень Разлома",
            "Одеяния ассасинов пустоты. Даруют похищение жизненных сил врагов при каждом ударе.",
            { 0, 0, 20.0, 0, 0, 0, 0.10, 0 }, "[2 предм.]: +20 Атаки, +10% Вампиризма",
            { 0, 0, 42.0, 0, 0, 0.25, 0.18, 0 }, "[4 предм.]: +42 Атаки, +18% Вампиризма, +25% Крит. урона");

        registerArtifact("rift_cowl", "Капюшон Тени", ArtifactType::Headgear, "set_rift",
            "assets/icons/rift_cowl.png", { 0, 0, 10.0, 0, 0.05, 0, 0.04, 0 }, 180, 90, "+10 Атаки, +5% Крит, +4% Вампиризма");
        registerArtifact("rift_vestment", "Одеяние Бездны", ArtifactType::Costume, "set_rift",
            "assets/icons/rift_vestment.png", { 0, 0, 20.0, 0, 0.08, 0, 0.06, 0 }, 240, 120, "+20 Атаки, +8% Крит, +6% Вампиризма");
        registerArtifact("rift_talisman", "Амулет Ночи", ArtifactType::Amulet, "set_rift",
            "assets/icons/rift_talisman.png", { 0, 0, 8.0, 0, 0.04, 0, 0.05, 0 }, 160, 80, "+8 Атаки, +4% Крит, +5% Вампиризма");
        registerArtifact("rift_ring", "Перстень Пустоты", ArtifactType::Ring, "set_rift",
            "assets/icons/rift_ring.png", { 0, 0, 10.0, 0, 0.04, 0, 0.05, 0 }, 150, 75, "+10 Атаки, +4% Крит, +5% Вампиризма");


        // Оружие как предметы инвентаря
        registerItem("rusty_blade", "Ржавый клинок", ItemCategory::Weapon, "assets/icons/rusty_blade.png", 50, 25);
        registerItem("old_cleaver", "Старый тесак", ItemCategory::Weapon, "assets/icons/old_cleaver.png", 60, 30);
        registerItem("berserker_axe", "Топор берсерка", ItemCategory::Weapon, "assets/icons/berserker_axe.png", 150, 75);
        registerItem("vampire_blade", "Клинок вампира", ItemCategory::Weapon, "assets/icons/vampire_blade.png", 300, 150);

        // Материалы
        registerItem("iron_ore", "Железная руда", ItemCategory::Material, "assets/icons/iron_ore.png", 25, 15);
        registerItem("scrap_metal", "Металлолом", ItemCategory::Material, "assets/icons/scrap_metal.png", 15, 8);
        // Редкие руды для крафта оружия
        registerItem("mithril_ore", "Мифриловая руда", ItemCategory::Material, "assets/icons/mithril_ore.png", 80, 40);
        registerItem("fire_crystal", "Огненный кристалл", ItemCategory::Material, "assets/icons/fire_crystal.png", 120, 60);
        registerItem("void_shard", "Осколок бездны", ItemCategory::Material, "assets/icons/void_shard.png", 150, 75);
        registerItem("celestial_ingot", "Небесный слиток", ItemCategory::Material, "assets/icons/celestial_ingot.png", 200, 100);


        // Артефакты
        registerItem("ancient_core", "Древнее ядро", ItemCategory::Artifact, "assets/icons/ancient_core.png", 200, 100);
        registerItem("sun_stone", "Солнечный камень", ItemCategory::Artifact, "assets/icons/sun_stone.png", 250, 120);

        // Драгоценности
        registerItem("ruby_ring", "Кольцо с рубином", ItemCategory::Jewelry, "assets/icons/ruby_ring.png", 120, 60);
        registerItem("gold_necklace", "Золотое ожерелье", ItemCategory::Jewelry, "assets/icons/gold_necklace.png", 180, 90);

        // Квестовые предметы
        registerItem("oasis_key", "Ключ от Оазиса", ItemCategory::Quest, "assets/icons/oasis_key.png", 0, 0);
        registerItem("ancient_scroll", "Древний свиток", ItemCategory::Quest, "assets/icons/ancient_scroll.png", 0, 0);

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