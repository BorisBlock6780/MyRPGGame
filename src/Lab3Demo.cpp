#include "Lab3Demo.hpp"
#include <iostream>
#include <cassert>
#include <iomanip>

namespace Lab3 {

void runLab3Demo() {
    std::cout << "================================================================================" << std::endl;
    std::cout << "    ЛАБОРАТОРНАЯ РАБОТА 3 (ЧАСТЬ 2: C++ - УМНЫЕ УКАЗАТЕЛИ)" << std::endl;
    std::cout << "    Проект: MyRPGGame (Система пошагового боя и управления снаряжением)" << std::endl;
    std::cout << "================================================================================\n" << std::endl;

    // -------------------------------------------------------------------------
    // РАЗДЕЛ 1. ТАБЛИЦА ВЛАДЕНИЯ ПАМЯТЬЮ
    // -------------------------------------------------------------------------
    std::cout << "--- 1. ТАБЛИЦА ВЛАДЕНИЯ (Замена сырых указателей из ЛР 2) ---" << std::endl;
    std::cout << "+---------------------------------+---------------------+------------------+------------------------------+-------------------------------------------------------+" << std::endl;
    std::cout << "| Где был указатель               | На что указывает    | Кто владеет      | Заменяется на                | Почему                                                |" << std::endl;
    std::cout << "+---------------------------------+---------------------+------------------+------------------------------+-------------------------------------------------------+" << std::endl;
    std::cout << "| CharacterTemplate::activeWeapon | WeaponTemplate      | База / Инвентарь | std::weak_ptr<WeaponTemplate>| Персонаж не владеет оружием, а лишь наблюдает за ним. |" << std::endl;
    std::cout << "|                                 | (экипированное)     | (персонаж не     |                              | Если оружие уничтожено, lock() вернет nullptr без      |" << std::endl;
    std::cout << "|                                 |                     | владеет)         |                              | висячих указателей.                                   |" << std::endl;
    std::cout << "+---------------------------------+---------------------+------------------+------------------------------+-------------------------------------------------------+" << std::endl;
    std::cout << "| WeaponTemplate* shelf[3]        | Динамический массив | Внешний скоуп    | std::unique_ptr<             | Единоличное владение массивом объектов.               |" << std::endl;
    std::cout << "| (массив оружия в ЛР 2)          | объектов оружия     | (один владелец)  |   WeaponTemplate[]> /        | Автоматическое освобождение памяти без delete[].       |" << std::endl;
    std::cout << "|                                 |                     |                  | std::unique_ptr<Weapon> arr[]|                                                       |" << std::endl;
    std::cout << "+---------------------------------+---------------------+------------------+------------------------------+-------------------------------------------------------+" << std::endl;
    std::cout << "| Временный создаваемый объект    | Экземпляр оружия    | Создатель        | std::unique_ptr<             | Монопольное владение при создании; передача владения  |" << std::endl;
    std::cout << "| (new WeaponTemplate(...))       |                     | (один владелец)  |   WeaponTemplate>            | через std::move без копирования и без утечек памяти.  |" << std::endl;
    std::cout << "+---------------------------------+---------------------+------------------+------------------------------+-------------------------------------------------------+" << std::endl;
    std::cout << "| Реестр глобальной базы данных   | Базовые шаблоны     | Совместное       | std::shared_ptr<             | Несколько систем одновременно разделяют один объект.  |" << std::endl;
    std::cout << "| (GameDatabase::weapons)         | оружия и предметов  | владение         |   WeaponTemplate>            | Объект живет, пока нужен хотя бы одному владельцу.    |" << std::endl;
    std::cout << "+---------------------------------+---------------------+------------------+------------------------------+-------------------------------------------------------+\n" << std::endl;

    // -------------------------------------------------------------------------
    // РАЗДЕЛ 2. СЦЕНАРИЙ ИЗ ЛР 2 (КОМПОЗИЦИЯ, АГРЕГАЦИЯ И ПРОВЕРКИ ПРАВИЛ)
    // -------------------------------------------------------------------------
    std::cout << "--- 2. Сценарий предметной области (ЛР 2) с умными указателями ---" << std::endl;

    // 2.1 Создание оружия через std::make_shared (оружие существует в базе независимо)
    std::cout << "[Шаг 2.1] Создание оружия через std::make_shared (базовый владелец):" << std::endl;
    std::shared_ptr<WeaponTemplate> ironSword = std::make_shared<WeaponTemplate>("iron_sword", "Стальной Меч", 25.0);
    std::cout << "  Оружие: " << ironSword->getName() 
              << ", Базовый урон: " << ironSword->getBaseDamage() << std::endl;

    // 2.2 Композиция: создание персонажа (SkillTemplate встроен по значению)
    std::cout << "\n[Шаг 2.2] Создание персонажа (композиция SkillTemplate внутри CharacterTemplate):" << std::endl;
    {
        CharacterTemplate hero("adeir", "Адеир");
        std::cout << "  Герой создан: " << hero.getName() 
                  << " | Врожденное умение (композиция): " << hero.getInnateSkillName() << std::endl;

        // 2.3 Проверка правила 1: Атака без экипированного оружия запрещена
        std::cout << "\n[Проверка правила 1] Попытка атаковать без экипированного оружия:" << std::endl;
        double dmg0 = 0.0;
        if (!hero.performAttack(dmg0)) {
            std::cout << "  -> ПРАВИЛО СРАБОТАЛО: Атака запрещена, так как оружие не экипировано!" << std::endl;
        }

        // 2.4 Агрегация: экипировка внешнего оружия через std::weak_ptr
        std::cout << "\n[Шаг 2.4] Агрегация: передаем оружие герою:" << std::endl;
        hero.equipWeapon(ironSword);
        std::cout << "  Оружие экипировано. Активное оружие героя: " << hero.getActiveWeaponName() << std::endl;

        // 2.5 Проверка правила 2: Ограничение заточки оружия максимум 20-м уровнем
        std::cout << "\n[Проверка правила 2] Заточка оружия (максимум 20 уровней):" << std::endl;
        for (int lvl = 1; lvl < 20; ++lvl) {
            ironSword->upgrade();
        }
        std::cout << "  Текущий уровень: " << ironSword->getLevel() 
                  << " | Урон с заточкой: " << ironSword->getBaseDamage() << std::endl;
        bool canUpgradeMore = ironSword->upgrade();
        std::cout << "  Попытка заточить на 21-й уровень: " 
                  << (canUpgradeMore ? "Успешно" : "ОТКЛОНЕНО (Правило: максимум 20-й уровень)") << std::endl;

        // 2.6 Проверка правила 3: Отрицательный кулдаун умения запрещен
        std::cout << "\n[Проверка правила 3] Создание умения с отрицательным кулдауном (-3):" << std::endl;
        try {
            SkillTemplate brokenSkill("broken", "Сломанное умение", -3);
        } catch (const std::invalid_argument& ex) {
            std::cout << "  -> ПЕРЕХВАЧЕНО ИСКЛЮЧЕНИЕ: " << ex.what() << std::endl;
        }

        // 2.7 Боевой сценарий из ЛР 2: атака 1, серия атак, ультимейт
        std::cout << "\n=== ХОД БОЯ (Боевая симуляция) ===" << std::endl;
        double dmg1 = 0.0;
        if (hero.performAttack(dmg1)) {
            std::cout << "Применена 1 атака: Боец " << hero.getName() 
                      << " наносит " << dmg1 << " урона. Боевые стаки: " << hero.getSkillStacks() << std::endl;
        }

        std::cout << "\nПрименена серия атак (накопление стаков):" << std::endl;
        for (int i = 0; i < 8; ++i) {
            double seriesDmg = 0.0;
            hero.performAttack(seriesDmg);
            std::cout << "  Атака " << (i + 2) << ": урон " << seriesDmg 
                      << " | Боевые стаки: " << hero.getSkillStacks() << std::endl;
        }

        std::cout << "\nПрименен ультимативный навык (расходует накопленные стаки):" << std::endl;
        double ultDmg = hero.performUltimate();
        std::cout << "  >>> ВЗРЫВ СТИХИЙ! Нанесен урон: " << ultDmg 
                  << " | Стаки после сброса: " << hero.getSkillStacks() << std::endl;

        std::cout << "\nВыход из блока героя (демонстрация порядка вызова деструкторов при композиции):" << std::endl;
    }
    std::cout << "Блок героя завершен. Обратите внимание: деструктор целого CharacterTemplate вызван первым," << std::endl;
    std::cout << "а деструктор встроенной части SkillTemplate вызван автоматически следом за ним!\n" << std::endl;

    // -------------------------------------------------------------------------
    // РАЗДЕЛ 3. ДЕМОНСТРАЦИЯ std::unique_ptr
    // -------------------------------------------------------------------------
    std::cout << "================================================================================" << std::endl;
    std::cout << "--- 3. Демонстрация std::unique_ptr (Один владелец) ---" << std::endl;
    {
        std::cout << "Вход во внутренний блок..." << std::endl;
        std::unique_ptr<WeaponTemplate> dagger = std::make_unique<WeaponTemplate>("shadow_dagger", "Кинжал Тени", 30.0);
        std::cout << "  Создан unique_ptr на объект: " << dagger->getName() << std::endl;

        std::cout << "  Передаем владение через std::move в другой unique_ptr..." << std::endl;
        std::unique_ptr<WeaponTemplate> transferredDagger = std::move(dagger);

        std::cout << "  Проверка исходного указателя dagger: " 
                  << (dagger == nullptr ? "NULLPTR (исходный указатель пуст, владение передано!)" : "не пуст") << std::endl;
        std::cout << "  Проверка нового владельца transferredDagger: " 
                  << transferredDagger->getName() << std::endl;

        std::cout << "Выход из внутреннего блока без вызова delete..." << std::endl;
    }
    std::cout << "Выход завершен. Деструктор вызван автоматически при уничтожении unique_ptr!\n" << std::endl;

    // 3.2 Массивы с std::unique_ptr (замена динамических массивов new[] / delete[])
    std::cout << "[3.2 Массив на умных указателях std::unique_ptr]:" << std::endl;
    {
        std::cout << "Создание массива unique_ptr shelf[3] на стеке:" << std::endl;
        std::unique_ptr<WeaponTemplate> shelf[3];
        shelf[0] = std::make_unique<WeaponTemplate>("sword_1", "Клинок новичка", 12.0);
        shelf[1] = std::make_unique<WeaponTemplate>("staff_1", "Посох искр", 18.0);
        shelf[2] = std::make_unique<WeaponTemplate>("bow_1", "Охотничий лук", 15.0);

        for (int i = 0; i < 3; ++i) {
            std::cout << "  shelf[" << i << "]: " << shelf[i]->getName() << std::endl;
        }
        std::cout << "Выход из блока массива (освобождение происходит само в правильном порядке):" << std::endl;
    }
    std::cout << "Массив уничтожен автоматически без вызова delete[]!\n" << std::endl;

    // -------------------------------------------------------------------------
    // РАЗДЕЛ 4. ДЕМОНСТРАЦИЯ std::shared_ptr (Несколько владельцев)
    // -------------------------------------------------------------------------
    std::cout << "================================================================================" << std::endl;
    std::cout << "--- 4. Демонстрация std::shared_ptr (Разделяемое владение) ---" << std::endl;
    {
        std::shared_ptr<WeaponTemplate> sharedWeapon = 
            std::make_shared<WeaponTemplate>("vampire_blade", "Кровавый Пожиратель", 45.0);

        std::cout << "Создан sharedWeapon. Счетчик владельцев (use_count): " 
                  << sharedWeapon.use_count() << std::endl;

        {
            std::cout << "  Вход во вложенный блок. Создаем копию указателя (второй владелец)..." << std::endl;
            std::shared_ptr<WeaponTemplate> secondOwner = sharedWeapon;
            std::cout << "  Счетчик владельцев внутри блока: " << sharedWeapon.use_count() << std::endl;
            std::cout << "  Объект через secondOwner: " << secondOwner->getName() << std::endl;
            std::cout << "  Выход из вложенного блока (secondOwner уничтожается)..." << std::endl;
        }

        std::cout << "После выхода из вложенного блока счетчик владельцев: " 
                  << sharedWeapon.use_count() << std::endl;
        std::cout << "Объект все еще жив, так как остался один владелец (sharedWeapon)." << std::endl;
        std::cout << "Сбрасываем последнего владельца через sharedWeapon.reset()..." << std::endl;
        sharedWeapon.reset();
        std::cout << "sharedWeapon сброшен. Счетчик: " << sharedWeapon.use_count() << std::endl;
    }
    std::cout << "Раздел shared_ptr завершен.\n" << std::endl;

    // -------------------------------------------------------------------------
    // РАЗДЕЛ 5. ДЕМОНСТРАЦИЯ std::weak_ptr (Наблюдатель без владения)
    // -------------------------------------------------------------------------
    std::cout << "================================================================================" << std::endl;
    std::cout << "--- 5. Демонстрация std::weak_ptr (Наблюдатель) ---" << std::endl;
    {
        std::weak_ptr<WeaponTemplate> weaponObserver;

        {
            std::cout << "Вход во внутренний блок. Создаем оружие во временном shared_ptr..." << std::endl;
            std::shared_ptr<WeaponTemplate> tempWeapon = 
                std::make_shared<WeaponTemplate>("colossus_hammer", "Молот Колосса", 80.0);

            // Создаем weak_ptr из shared_ptr (не увеличивает счетчик владельцев!)
            weaponObserver = tempWeapon;
            std::cout << "weak_ptr создан. Счетчик владельцев tempWeapon.use_count(): " 
                      << tempWeapon.use_count() << " (счетчик НЕ увеличился!)" << std::endl;

            // Доступ к объекту через weak_ptr::lock()
            if (std::shared_ptr<WeaponTemplate> locked = weaponObserver.lock()) {
                std::cout << "  Успешный lock(): оружие существует -> " << locked->getName() << std::endl;
            }

            std::cout << "Выход из внутреннего блока (tempWeapon уничтожается)..." << std::endl;
        }

        // Проверяем weak_ptr после уничтожения последнего владельца
        std::cout << "После уничтожения владельца: проверяем weaponObserver.lock()..." << std::endl;
        if (std::shared_ptr<WeaponTemplate> locked = weaponObserver.lock()) {
            std::cout << "  Оружие найдено: " << locked->getName() << std::endl;
        } else {
            std::cout << "  -> lock() ВЕРНУЛ NULLPTR! Объект уничтожен, программа корректно обработала это!" << std::endl;
        }

        // Демонстрация в контексте персонажа
        std::cout << "\nИнтеграция с персонажем (защита от повисших указателей):" << std::endl;
        CharacterTemplate guard("guard", "Страж Шпиля");
        {
            auto temporaryBlade = std::make_shared<WeaponTemplate>("temp_blade", "Временный клинок", 20.0);
            guard.equipWeapon(temporaryBlade);
            std::cout << "  Страж экипировал: " << guard.getActiveWeaponName() << std::endl;
            double dmg = 0.0;
            guard.performAttack(dmg);
            std::cout << "  Атака временным клинком успешна: урон " << dmg << std::endl;
            std::cout << "  Клинок уничтожается при выходе из блока..." << std::endl;
        }
        std::cout << "  Клинок уничтожен. Пытаемся атаковать стражем:" << std::endl;
        double failDmg = 0.0;
        if (!guard.performAttack(failDmg)) {
            std::cout << "  -> Атака безопасно отклонена: activeWeapon.lock() вернул nullptr (нет повисшего указателя)!" << std::endl;
        }
    }

    std::cout << "\n================================================================================" << std::endl;
    std::cout << "    ДЕМОНСТРАЦИЯ ЛАБОРАТОРНОЙ РАБОТЫ 3 УСПЕШНО ЗАВЕРШЕНА" << std::endl;
    std::cout << "================================================================================" << std::endl;
}

} // namespace Lab3

// Если файл собирается как отдельный исполняемый файл Lab3Demo
#ifndef COMBINED_BUILD
int main() {
    Lab3::runLab3Demo();
    return 0;
}
#endif
