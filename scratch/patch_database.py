import os

db_path = r"include\Database.hpp"
with open(db_path, "r", encoding="utf-8") as f:
    content = f.read()

# 1. Add ArtifactSetInfo and ArtifactTemplate after ItemTemplate if not already present
marker_item_end = "    ImTextureID getImGuiTextureID() const {\n        return (ImTextureID)(uintptr_t)iconTexture.getNativeHandle();\n    }\n};"
if marker_item_end not in content:
    marker_item_end = "    ImTextureID getImGuiTextureID() const {\r\n        return (ImTextureID)(uintptr_t)iconTexture.getNativeHandle();\r\n    }\r\n};"

artifact_classes = """

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
};"""

if "class ArtifactTemplate" not in content:
    assert marker_item_end in content, "Could not find ItemTemplate end marker"
    content = content.replace(marker_item_end, marker_item_end + artifact_classes, 1)

# 2. Add extra weapons (Staff, Bow, Dagger, Bastion)
extra_weapons = """
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
"""

marker_vamp_end = '    }\n};'
# We can insert extra_weapons before ItemTemplate
marker_item_start = "// ==========================================\n// ИЕРАРХИЯ ШАБЛОНОВ ПРЕДМЕТОВ"
if marker_item_start not in content:
    marker_item_start = "// ==========================================\r\n// ИЕРАРХИЯ ШАБЛОНОВ ПРЕДМЕТОВ"

if "class ApprenticeStaffTemplate" not in content:
    assert marker_item_start in content, "Could not find ItemTemplate start marker"
    content = content.replace(marker_item_start, extra_weapons + "\n" + marker_item_start, 1)

# 3. Add artifacts & artifactSets containers and registration methods to GameDatabase
db_containers = """    // Хранилища умных указателей
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
"""

old_db_containers = """    // Хранилища умных указателей
    std::unordered_map<std::string, std::shared_ptr<CharacterTemplate>> characters;
    std::unordered_map<std::string, std::shared_ptr<WeaponTemplate>> weapons; 
    std::unordered_map<std::string, std::shared_ptr<ItemTemplate>> items;
    std::unordered_map<std::string, std::shared_ptr<EnemyTemplate>> enemies;"""

if "std::unordered_map<std::string, std::shared_ptr<ArtifactTemplate>> artifacts;" not in content:
    if old_db_containers in content:
        content = content.replace(old_db_containers, db_containers, 1)
    else:
        old_db_c_crlf = old_db_containers.replace("\n", "\r\n")
        assert old_db_c_crlf in content, "Could not find old_db_containers"
        content = content.replace(old_db_c_crlf, db_containers.replace("\n", "\r\n"), 1)

# 4. In GameDatabase::init(), register new weapons and artifact sets
artifact_init_code = """
        // Дополнительное оружие
        weapons["apprentice_staff"] = std::make_shared<ApprenticeStaffTemplate>();
        weapons["hunter_bow"] = std::make_shared<HunterBowTemplate>();
        weapons["shadow_dagger"] = std::make_shared<ShadowDaggerTemplate>();
        weapons["bastion_blade"] = std::make_shared<BastionBladeTemplate>();

        registerItem("apprentice_staff", "Посох Ученика", ItemCategory::Weapon, "assets/icons/weapon.png", 90, 45);
        registerItem("hunter_bow", "Охотничий Лук", ItemCategory::Weapon, "assets/icons/weapon.png", 100, 50);
        registerItem("shadow_dagger", "Кинжал Тени", ItemCategory::Weapon, "assets/icons/weapon.png", 140, 70);
        registerItem("bastion_blade", "Меч-Бастион", ItemCategory::Weapon, "assets/icons/weapon.png", 130, 65);

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
"""

marker_vamp_reg = 'weapons["vampire_blade"] = std::make_shared<VampireBladeTemplate>();'
if "set_guardian" not in content:
    assert marker_vamp_reg in content, "Could not find vampire_blade registration"
    content = content.replace(marker_vamp_reg, marker_vamp_reg + "\n" + artifact_init_code, 1)

with open(db_path, "w", encoding="utf-8") as f:
    f.write(content)

print("Database.hpp successfully patched!")
