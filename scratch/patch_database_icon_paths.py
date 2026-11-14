import os

db_path = r"include\Database.hpp"
with open(db_path, "r", encoding="utf-8") as f:
    content = f.read()

replacements = [
    # Weapons
    ('registerItem("rusty_blade", "Ржавый клинок", ItemCategory::Weapon, "assets/icons/weapon.png"',
     'registerItem("rusty_blade", "Ржавый клинок", ItemCategory::Weapon, "assets/icons/rusty_blade.png"'),
    ('registerItem("old_cleaver", "Старый тесак", ItemCategory::Weapon, "assets/icons/weapon.png"',
     'registerItem("old_cleaver", "Старый тесак", ItemCategory::Weapon, "assets/icons/old_cleaver.png"'),
    ('registerItem("berserker_axe", "Топор берсерка", ItemCategory::Weapon, "assets/icons/weapon.png"',
     'registerItem("berserker_axe", "Топор берсерка", ItemCategory::Weapon, "assets/icons/berserker_axe.png"'),
    ('registerItem("vampire_blade", "Клинок вампира", ItemCategory::Weapon, "assets/icons/weapon.png"',
     'registerItem("vampire_blade", "Клинок вампира", ItemCategory::Weapon, "assets/icons/vampire_blade.png"'),
    ('registerItem("apprentice_staff", "Посох Ученика", ItemCategory::Weapon, "assets/icons/weapon.png"',
     'registerItem("apprentice_staff", "Посох Ученика", ItemCategory::Weapon, "assets/icons/apprentice_staff.png"'),
    ('registerItem("hunter_bow", "Охотничий Лук", ItemCategory::Weapon, "assets/icons/weapon.png"',
     'registerItem("hunter_bow", "Охотничий Лук", ItemCategory::Weapon, "assets/icons/hunter_bow.png"'),
    ('registerItem("shadow_dagger", "Кинжал Тени", ItemCategory::Weapon, "assets/icons/weapon.png"',
     'registerItem("shadow_dagger", "Кинжал Тени", ItemCategory::Weapon, "assets/icons/shadow_dagger.png"'),
    ('registerItem("bastion_blade", "Меч-Бастион", ItemCategory::Weapon, "assets/icons/weapon.png"',
     'registerItem("bastion_blade", "Меч-Бастион", ItemCategory::Weapon, "assets/icons/bastion_blade.png"'),

    # Materials
    ('registerItem("iron_ore", "Железная руда", ItemCategory::Material, "assets/icons/materials.png"',
     'registerItem("iron_ore", "Железная руда", ItemCategory::Material, "assets/icons/iron_ore.png"'),
    ('registerItem("scrap_metal", "Металлолом", ItemCategory::Material, "assets/icons/materials.png"',
     'registerItem("scrap_metal", "Металлолом", ItemCategory::Material, "assets/icons/scrap_metal.png"'),

    # Artifacts
    ('registerItem("ancient_core", "Древнее ядро", ItemCategory::Artifact, "assets/icons/artifact.png"',
     'registerItem("ancient_core", "Древнее ядро", ItemCategory::Artifact, "assets/icons/ancient_core.png"'),
    ('registerItem("sun_stone", "Солнечный камень", ItemCategory::Artifact, "assets/icons/artifact.png"',
     'registerItem("sun_stone", "Солнечный камень", ItemCategory::Artifact, "assets/icons/sun_stone.png"'),

    # Jewelry
    ('registerItem("ruby_ring", "Кольцо с рубином", ItemCategory::Jewelry, "assets/icons/jewelry.png"',
     'registerItem("ruby_ring", "Кольцо с рубином", ItemCategory::Jewelry, "assets/icons/ruby_ring.png"'),
    ('registerItem("gold_necklace", "Золотое ожерелье", ItemCategory::Jewelry, "assets/icons/jewelry.png"',
     'registerItem("gold_necklace", "Золотое ожерелье", ItemCategory::Jewelry, "assets/icons/gold_necklace.png"'),

    # Quest
    ('registerItem("oasis_key", "Ключ от Оазиса", ItemCategory::Quest, "assets/icons/quest.png"',
     'registerItem("oasis_key", "Ключ от Оазиса", ItemCategory::Quest, "assets/icons/oasis_key.png"'),
    ('registerItem("ancient_scroll", "Древний свиток", ItemCategory::Quest, "assets/icons/quest.png"',
     'registerItem("ancient_scroll", "Древний свиток", ItemCategory::Quest, "assets/icons/ancient_scroll.png"')
]

# Also register new weapon crafting ores in init()
new_ores = """
        // Редкие руды для крафта оружия
        registerItem("mithril_ore", "Мифриловая руда", ItemCategory::Material, "assets/icons/mithril_ore.png", 80, 40);
        registerItem("fire_crystal", "Огненный кристалл", ItemCategory::Material, "assets/icons/fire_crystal.png", 120, 60);
        registerItem("void_shard", "Осколок бездны", ItemCategory::Material, "assets/icons/void_shard.png", 150, 75);
        registerItem("celestial_ingot", "Небесный слиток", ItemCategory::Material, "assets/icons/celestial_ingot.png", 200, 100);
"""

for old_s, new_s in replacements:
    content = content.replace(old_s, new_s)

if "mithril_ore" not in content:
    target_mat = 'registerItem("scrap_metal", "Металлолом", ItemCategory::Material, "assets/icons/scrap_metal.png", 15, 8);'
    content = content.replace(target_mat, target_mat + new_ores, 1)

with open(db_path, "w", encoding="utf-8") as f:
    f.write(content)

print("Database icon paths successfully updated!")
