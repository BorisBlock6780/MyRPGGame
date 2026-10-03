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

public:
    // Добавление предмета (Условие: 1 вид предмета = 1 слот)
    void addItem(const std::string& itemId, int amount = 1) {
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

public:
    PlayerCharacter(std::string id, int lvl = 1) : templateId(std::move(id)), level(lvl) {}

    // Управление стаками
    void addSkillStack(int amount = 1) { skillStacks += amount; }
    void resetSkillStacks() { skillStacks = 0; }
    int getSkillStacks() const { return skillStacks; }

    StatArray getTotalStats() const {
        const auto& tmpl = GameDatabase::getInstance().characters.at(templateId);
        StatArray total = tmpl->getBaseStats();
        const auto& growth = tmpl->getGrowthPerLevel();

        for (size_t i = 0; i < 8; ++i) {
            total[i] += growth[i] * (level - 1);
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

