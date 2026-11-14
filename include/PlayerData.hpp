#pragma once
#include "Database.hpp"
#include <memory>
#include <vector>
#include <array>
#include <algorithm>
#include <iostream>

struct ItemStack {
    std::string itemId; // Ссылка на ID из GameDatabase ("apple")
    int count = 0;      // Количество предметов в одной карточке
};

class PlayerInventory {
private:
    std::vector<ItemStack> items; // Список стакающихся предметов
    int balance = 200;

public:
    PlayerInventory() = default;
    explicit PlayerInventory(int startBalance) : balance(std::max(0, startBalance)) {}

    // --- Управление балансом средств ---
    int getBalance() const { return balance; }

    void setBalance(int newBalance) {
        balance = std::max(0, newBalance); // Защита от отрицательного баланса
    }

    void addFunds(int amount) {
        if (amount > 0) {
            balance += amount;
        }
    }

    // Списание средств: возвращает false, если денег не хватает
    bool spendFunds(int amount) {
        if (amount < 0 || balance < amount) {
            return false; // Баланс не может уйти в минус!
        }
        balance -= amount;
        return true;
    }

    // Добавление предмета (Условие: 1 вид предмета = 1 слот)
    void addItem(const std::string& itemId, int amount = 1) {
        if (amount <= 0) return;

        // 1. Ищем, есть ли уже такой предмет в инвентаре
        for (auto& stack : items) {
            if (stack.itemId == itemId) {
                stack.count += amount; // Просто увеличиваем счётчик!
                return;
            }
        }

        // 2. Если такого предмета ещё нет, создаём для него 1 единственный новый слот
        items.push_back({ itemId, amount });
    }

    // Удаление/Расходование предмета (например, съели яблоко)
    bool removeItem(const std::string& itemId, int amount = 1) {
        if (amount <= 0) return false;

        for (auto it = items.begin(); it != items.end(); ++it) {
            if (it->itemId == itemId) {
                if (it->count >= amount) {
                    it->count -= amount;
                    if (it->count <= 0) {
                        items.erase(it); // Если израсходовали всё — удаляем слот
                    }
                    return true;
                }
                return false; // Недостаточно количества
            }
        }
        return false;
    }

    // Получить количество конкретного предмета в инвентаре
    int getItemCount(const std::string& itemId) const {
        for (const auto& stack : items) {
            if (stack.itemId == itemId) {
                return stack.count;
            }
        }
        return 0;
    }

    // Получить предметы ТОЛЬКО определенной категории (для фильтрации по вкладкам UI)
    std::vector<ItemStack> getItemsByCategory(ItemCategory category) const {
        std::vector<ItemStack> filtered;
        const auto& db = GameDatabase::getInstance().items;

        for (const auto& stack : items) {
            auto it = db.find(stack.itemId);
            if (it != db.end() && it->second->getCategory() == category) {
                filtered.push_back(stack);
            }
        }
        return filtered;
    }
};

class PlayerCharacter {
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
};

class PlayerProfile {
private:
    // 1. Все разблокированные персонажи игрока
    std::vector<std::shared_ptr<PlayerCharacter>> reserveCharacters; 

    // 2. Активный отряд (максимум 3 слота)
    // Использование shared_ptr позволяет слоту указывать на тот же объект, что лежит в резерве
    std::array<std::shared_ptr<PlayerCharacter>, 3> activeSquad = { nullptr, nullptr, nullptr };
        
public:
    // Получение нового персонажа — он сразу отправляется в резерв (режим ожидания)
    void obtainCharacter(const std::string& templateId) {
        auto newChar = std::make_shared<PlayerCharacter>(templateId);
        reserveCharacters.push_back(newChar); 

        // Автоматически ставим в свободный слот отряда, если отряд пуст
        for (auto& slot : activeSquad) {
            if (slot == nullptr) {
                slot = newChar;
                break;
            }
        }
    }

    // Проверка, есть ли уже персонаж с таким ID
    bool hasCharacter(const std::string& templateId) const {
        for (const auto& ch : reserveCharacters) {
            if (ch && ch->getId() == templateId) {
                return true;
            }
        }
        return false;
    }

    // Удаление персонажа из профиля (и из активного отряда)
    bool removeCharacter(const std::string& templateId) {
        auto it = std::find_if(reserveCharacters.begin(), reserveCharacters.end(),
            [&templateId](const std::shared_ptr<PlayerCharacter>& ch) {
                return ch && ch->getId() == templateId;
            });
        if (it == reserveCharacters.end()) {
            return false;
        }

        auto targetChar = *it;

        // Убираем из активного отряда
        for (auto& slot : activeSquad) {
            if (slot == targetChar) {
                slot = nullptr;
            }
        }

        reserveCharacters.erase(it);
        return true;
    }

    size_t getCharacterCount() const {
        return reserveCharacters.size();
    }

    void setSquadSlotDirectly(size_t slotIndex, std::shared_ptr<PlayerCharacter> character) {
        if (slotIndex < 3) {
            activeSquad[slotIndex] = character;
        }
    }

    // Поставить персонажа из резерва в конкретный слот отряда (0, 1 или 2)
    bool setSquadSlot(size_t slotIndex, size_t reserveIndex) {
        if (slotIndex >= 3 || reserveIndex >= reserveCharacters.size()) {
            return false;
        }

        auto targetChar = reserveCharacters[reserveIndex];

        // Убираем этого персонажа из других слотов, если он уже был в отряде (чтобы не было дубликатов)
        for (auto& slot : activeSquad) {
            if (slot == targetChar) {
                slot = nullptr;
            }
        }

        activeSquad[slotIndex] = targetChar;
        return true;
    }

    // Убрать персонажа из слота отряда
    void removeFromSquad(size_t slotIndex) {
        if (slotIndex < 3) {
            activeSquad[slotIndex] = nullptr;
        }
    }

    // Геттеры для UI
    const std::array<std::shared_ptr<PlayerCharacter>, 3>& getActiveSquad() const {
        return activeSquad;
    }

    const std::vector<std::shared_ptr<PlayerCharacter>>& getReserve() const {
        return reserveCharacters;
    }
};
