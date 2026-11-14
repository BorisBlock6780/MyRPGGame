#pragma once
#include <string>
#include <memory>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <algorithm>

namespace Lab3 {

// =============================================================================
// 1. ШАБЛОН УМЕНИЯ (SkillTemplate)
// =============================================================================
class SkillTemplate {
private:
    std::string id;
    std::string name;
    int cooldown;
    int stacksGranted;

public:
    SkillTemplate(std::string id, std::string name, int cooldown, int stacksGranted = 1)
        : id(std::move(id)), name(std::move(name)), cooldown(cooldown), stacksGranted(std::max(0, stacksGranted)) {
        
        // Правило предметной области: кулдаун не может быть отрицательным
        if (this->cooldown < 0) {
            throw std::invalid_argument("Кулдаун умения не может быть отрицательным!");
        }
        std::cout << "  [+] Конструктор SkillTemplate: " << this->name 
                  << " (Кулдаун: " << this->cooldown << ")" << std::endl;
    }

    ~SkillTemplate() {
        std::cout << "  [-] Деструктор SkillTemplate: " << name << std::endl;
    }

    const std::string& getId() const { return id; }
    const std::string& getName() const { return name; }
    int getCooldown() const { return cooldown; }
    int getStacksGranted() const { return stacksGranted; }

    double calculateValue(double baseStat, int skillStacks) const {
        return (baseStat * 0.60) + (skillStacks * 15.0);
    }
};

// =============================================================================
// 2. ШАБЛОН ОРУЖИЯ (WeaponTemplate)
// =============================================================================
class WeaponTemplate {
private:
    std::string id;
    std::string name;
    double baseDamage;
    int level;
    const int maxLevel = 20;

public:
    WeaponTemplate()
        : id("unnamed"), name("Базовый клинок"), baseDamage(10.0), level(1) {
        std::cout << "  [+] Конструктор по умолчанию WeaponTemplate: " << name << std::endl;
    }

    WeaponTemplate(std::string id, std::string name, double baseDamage, int initialLevel = 1)
        : id(std::move(id)), name(std::move(name)), baseDamage(std::max(0.0, baseDamage)), level(initialLevel) {
        if (level < 1 || level > maxLevel) {
            throw std::out_of_range("Уровень заточки должен быть в пределах от 1 до 20!");
        }
        std::cout << "  [+] Конструктор с параметрами WeaponTemplate: " << this->name 
                  << " (Ур. " << this->level << ", Базовый урон: " << this->baseDamage << ")" << std::endl;
    }

    virtual ~WeaponTemplate() {
        std::cout << "  [-] Деструктор WeaponTemplate: " << name 
                  << " (Ур. " << level << ")" << std::endl;
    }

    const std::string& getId() const { return id; }
    const std::string& getName() const { return name; }
    int getLevel() const { return level; }
    int getMaxLevel() const { return maxLevel; }

    // Динамический расчет урона (+8% за уровень заточки выше первого)
    virtual double getBaseDamage() const {
        return baseDamage * (1.0 + (level - 1) * 0.08);
    }

    // Правило предметной области: уровень заточки не может превышать 20-й
    bool upgrade() {
        if (level >= maxLevel) {
            return false;
        }
        level++;
        return true;
    }
};

// =============================================================================
// 3. ШАБЛОН ПЕРСОНАЖА (CharacterTemplate)
// =============================================================================
class CharacterTemplate {
private:
    std::string id;
    std::string name;
    int skillStacks;

    // КОМПОЗИЦИЯ: часть (SkillTemplate) встроена по значению.
    // Время жизни части строго совпадает с временем жизни целого (CharacterTemplate).
    SkillTemplate innateSkill;

    // АГРЕГАЦИЯ: слабая ссылка std::weak_ptr вместо сырого указателя WeaponTemplate*.
    // Персонаж не владеет оружием, а лишь наблюдает за ним.
    // Если оружие уничтожено в базе/инвентаре, lock() вернет nullptr без повисшего указателя.
    std::weak_ptr<WeaponTemplate> activeWeapon;

public:
    CharacterTemplate(std::string id, std::string name)
        : id(std::move(id)), name(std::move(name)), skillStacks(0),
          innateSkill(this->id + "_innate", "Врожденный взрыв стихий", 0, 1) {
        std::cout << "[+] Конструктор CharacterTemplate: " << this->name << std::endl;
    }

    virtual ~CharacterTemplate() {
        std::cout << "[-] Деструктор CharacterTemplate: " << name << std::endl;
    }

    const std::string& getId() const { return id; }
    const std::string& getName() const { return name; }
    int getSkillStacks() const { return skillStacks; }
    const std::string& getInnateSkillName() const { return innateSkill.getName(); }

    void addSkillStack(int amount = 1) {
        if (amount > 0) skillStacks += amount;
    }

    void resetSkillStacks() {
        skillStacks = 0;
    }

    // Агрегация: подключение оружия через std::shared_ptr, хранение через std::weak_ptr
    void equipWeapon(const std::shared_ptr<WeaponTemplate>& weapon) {
        activeWeapon = weapon;
    }

    void unequipWeapon() {
        activeWeapon.reset();
    }

    bool hasEquippedWeapon() const {
        return !activeWeapon.expired();
    }

    std::string getActiveWeaponName() const {
        if (auto weaponSp = activeWeapon.lock()) {
            return weaponSp->getName();
        }
        return "нет";
    }

    // Проверка правила: Запрещено атаковать без оружия (если activeWeapon.lock() == nullptr)
    bool performAttack(double& damageDealt) {
        auto weaponSp = activeWeapon.lock();
        if (!weaponSp) {
            damageDealt = 0.0;
            return false; // Срабатывание правила
        }

        addSkillStack(innateSkill.getStacksGranted());
        double innateBonus = innateSkill.calculateValue(15.0, skillStacks);
        damageDealt = weaponSp->getBaseDamage() + innateBonus;
        return true;
    }

    double performUltimate() {
        double weaponDmg = 0.0;
        if (auto weaponSp = activeWeapon.lock()) {
            weaponDmg = weaponSp->getBaseDamage();
        }
        double ultDmg = weaponDmg + (innateSkill.calculateValue(30.0, skillStacks) * 1.5);
        resetSkillStacks();
        return ultDmg;
    }
};

// Главная функция демонстрации Лабораторной работы 3 (Часть 2)
void runLab3Demo();

} // namespace Lab3
