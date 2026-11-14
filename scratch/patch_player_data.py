import os

player_data_path = r"include\PlayerData.hpp"
with open(player_data_path, "r", encoding="utf-8") as f:
    content = f.read()

new_player_char = """class PlayerCharacter {
private:
    std::string templateId;
    int level = 1;
    int currentExp = 0;

    // Новая переменная для хранения текущих стаков скиллов в бою
    int skillStacks = 0;

    // ==========================================
    // СЛОТЫ ЭКИПИРОВКИ (1 Оружие + 4 Артефакта)
    // ==========================================
    std::string equippedWeapon;    // Слот Оружия (только оружие!)
    std::string equippedHeadgear;  // Слот Головного убора
    std::string equippedCostume;   // Слот Костюма / Доспеха
    std::string equippedAmulet;    // Слот Амулета
    std::string equippedRing;      // Слот Кольца

public:
    PlayerCharacter(std::string id, int lvl = 1) : templateId(std::move(id)), level(lvl) {}

    const std::string& getId() const { return templateId; }

    // Управление стаками
    void addSkillStack(int amount = 1) { skillStacks += amount; }
    void resetSkillStacks() { skillStacks = 0; }
    int getSkillStacks() const { return skillStacks; }
    int getLevel() const { return level; }
    std::string getName() const {
        return GameDatabase::getInstance().characters.at(templateId)->getName();
    }

    // --- Доступ к слотам экипировки ---
    const std::string& getEquippedWeapon() const { return equippedWeapon; }
    const std::string& getEquippedHeadgear() const { return equippedHeadgear; }
    const std::string& getEquippedCostume() const { return equippedCostume; }
    const std::string& getEquippedAmulet() const { return equippedAmulet; }
    const std::string& getEquippedRing() const { return equippedRing; }

    std::string getEquippedItem(ArtifactType type) const {
        switch (type) {
        case ArtifactType::Weapon:   return equippedWeapon;
        case ArtifactType::Headgear: return equippedHeadgear;
        case ArtifactType::Costume:  return equippedCostume;
        case ArtifactType::Amulet:   return equippedAmulet;
        case ArtifactType::Ring:     return equippedRing;
        default: return "";
        }
    }

    // --- Экипировка ОРУЖИЯ (Строгая валидация) ---
    bool equipWeapon(const std::string& weaponId, PlayerInventory& inventory) {
        auto& db = GameDatabase::getInstance();
        if (db.weapons.find(weaponId) == db.weapons.end()) {
            return false; // Такого оружия нет в базе
        }
        if (inventory.getItemCount(weaponId) <= 0) {
            return false; // Нет в инвентаре игрока
        }

        // Если уже есть надетое оружие — возвращаем его в инвентарь
        if (!equippedWeapon.empty()) {
            inventory.addItem(equippedWeapon, 1);
        }

        // Забираем новое из инвентаря и надеваем
        inventory.removeItem(weaponId, 1);
        equippedWeapon = weaponId;
        return true;
    }

    bool unequipWeapon(PlayerInventory& inventory) {
        if (equippedWeapon.empty()) return false;
        inventory.addItem(equippedWeapon, 1);
        equippedWeapon.clear();
        return true;
    }

    // --- Экипировка АРТЕФАКТОВ (СТРОГО по типу слота: шлем в шлем, кольцо в кольцо и т.д.) ---
    bool equipArtifact(ArtifactType slotType, const std::string& artifactId, PlayerInventory& inventory) {
        auto& db = GameDatabase::getInstance();
        auto it = db.artifacts.find(artifactId);
        if (it == db.artifacts.end()) {
            return false; // Не является артефактом
        }

        // СТРОЖАЙШАЯ ПРОВЕРКА СООТВЕТСТВИЯ СЛОТА!
        if (it->second->getArtifactType() != slotType) {
            return false; // Нельзя поместить артефакт другого типа!
        }

        if (inventory.getItemCount(artifactId) <= 0) {
            return false; // Нет предмета в инвентаре
        }

        // Снятие старого предмета из этого слота
        std::string* targetSlot = nullptr;
        switch (slotType) {
        case ArtifactType::Headgear: targetSlot = &equippedHeadgear; break;
        case ArtifactType::Costume:  targetSlot = &equippedCostume; break;
        case ArtifactType::Amulet:   targetSlot = &equippedAmulet; break;
        case ArtifactType::Ring:     targetSlot = &equippedRing; break;
        default: return false;
        }

        if (!targetSlot->empty()) {
            inventory.addItem(*targetSlot, 1);
        }

        inventory.removeItem(artifactId, 1);
        *targetSlot = artifactId;
        return true;
    }

    bool unequipArtifact(ArtifactType slotType, PlayerInventory& inventory) {
        std::string* targetSlot = nullptr;
        switch (slotType) {
        case ArtifactType::Headgear: targetSlot = &equippedHeadgear; break;
        case ArtifactType::Costume:  targetSlot = &equippedCostume; break;
        case ArtifactType::Amulet:   targetSlot = &equippedAmulet; break;
        case ArtifactType::Ring:     targetSlot = &equippedRing; break;
        default: return false;
        }

        if (targetSlot->empty()) return false;
        inventory.addItem(*targetSlot, 1);
        targetSlot->clear();
        return true;
    }

    // --- Подсчет активных комплектов артефактов (для бонусов за 2 и 4 предмета) ---
    std::unordered_map<std::string, int> getActiveSetCounts() const {
        std::unordered_map<std::string, int> counts;
        auto& db = GameDatabase::getInstance();
        auto checkPiece = [&](const std::string& artId) {
            if (artId.empty()) return;
            auto it = db.artifacts.find(artId);
            if (it != db.artifacts.end() && !it->second->getSetId().empty()) {
                counts[it->second->getSetId()]++;
            }
        };

        checkPiece(equippedHeadgear);
        checkPiece(equippedCostume);
        checkPiece(equippedAmulet);
        checkPiece(equippedRing);
        return counts;
    }

    // Полный пересчет всех характеристик (База + Рост за уровень + Оружие + Артефакты + Сет-бонусы)
    StatArray getTotalStats() const {
        const auto& db = GameDatabase::getInstance();
        const auto& tmpl = db.characters.at(templateId);
        StatArray total = tmpl->getBaseStats();
        const auto& growth = tmpl->getGrowthPerLevel();

        // 1. Прирост за уровень
        for (size_t i = 0; i < 8; ++i) {
            total[i] += growth[i] * (level - 1);
        }

        // 2. Бонусы экипированного ОРУЖИЯ
        if (!equippedWeapon.empty()) {
            auto itW = db.weapons.find(equippedWeapon);
            if (itW != db.weapons.end()) {
                total[StatIndex::Attack] += itW->second->getDamageAtLevel(level);
                StatArray wBonus = itW->second->getBonusStatsAtLevel(level);
                for (size_t i = 0; i < 8; ++i) {
                    total[i] += wBonus[i];
                }
            }
        }

        // 3. Бонусы экипированных АРТЕФАКТОВ (по слотам)
        auto addArtStats = [&](const std::string& artId, ArtifactType expectedType) {
            if (artId.empty()) return;
            auto itA = db.artifacts.find(artId);
            if (itA != db.artifacts.end() && itA->second->getArtifactType() == expectedType) {
                const auto& bStats = itA->second->getBonusStats();
                for (size_t i = 0; i < 8; ++i) {
                    total[i] += bStats[i];
                }
            }
        };

        addArtStats(equippedHeadgear, ArtifactType::Headgear);
        addArtStats(equippedCostume, ArtifactType::Costume);
        addArtStats(equippedAmulet, ArtifactType::Amulet);
        addArtStats(equippedRing, ArtifactType::Ring);

        // 4. СЕТ-БОНУСЫ (За 2 и за 4 предмета одного комплекта)
        auto setCounts = getActiveSetCounts();
        for (const auto& [setId, count] : setCounts) {
            auto itSet = db.artifactSets.find(setId);
            if (itSet != db.artifactSets.end()) {
                if (count >= 2) {
                    for (size_t i = 0; i < 8; ++i) {
                        total[i] += itSet->second.twoPieceBonus[i];
                    }
                }
                if (count >= 4) {
                    for (size_t i = 0; i < 8; ++i) {
                        total[i] += itSet->second.fourPieceBonus[i];
                    }
                }
            }
        }

        return total;
    }

    // Пример вызова навыка с учетом стаков
    double useSkill(size_t skillIndex) {
        const auto& tmpl = GameDatabase::getInstance().characters.at(templateId);
        const auto& skills = tmpl->getSkills();

        if (skillIndex >= skills.size()) return 0.0;

        StatArray currentStats = getTotalStats();

        // Передаем текущие стаки в полиморфный метод расчета урона
        double outputValue = skills[skillIndex]->calculateValue(currentStats, skillStacks);

        return outputValue;
    }
};"""

# Find old class PlayerCharacter
old_start = "class PlayerCharacter {"
old_end = "    double useSkill(size_t skillIndex) {\n        const auto& tmpl = GameDatabase::getInstance().characters.at(templateId);\n        const auto& skills = tmpl->getSkills();\n\n        if (skillIndex >= skills.size()) return 0.0;\n\n        StatArray currentStats = getTotalStats();\n\n        // Передаем текущие стаки в полиморфный метод расчета урона\n        double outputValue = skills[skillIndex]->calculateValue(currentStats, skillStacks);\n\n        return outputValue;\n    }\n};"

if old_end not in content:
    old_end = old_end.replace("\n", "\r\n")

assert old_start in content
idx1 = content.find(old_start)
idx2 = content.find(old_end) + len(old_end)
content = content[:idx1] + new_player_char + content[idx2:]

with open(player_data_path, "w", encoding="utf-8") as f:
    f.write(content)

print("PlayerData.hpp successfully updated!")
