#pragma execution_character_set("utf-8")
#include "ScreenManager.hpp"
#include "Database.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <unordered_set>
#include <imgui.h>
#include <algorithm>
#include "PlayerData.hpp"

#ifdef _WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

// =============================================================================
// ГЛОБАЛЬНАЯ ТЕМА ОФОРМЛЕНИЯ ImGui
// =============================================================================
std::string ScreenManager::getKeyDisplayName(sf::Keyboard::Key key) {
    if (key >= sf::Keyboard::A && key <= sf::Keyboard::Z) {
        return std::string(1, static_cast<char>('A' + (key - sf::Keyboard::A)));
    }
    if (key >= sf::Keyboard::Num0 && key <= sf::Keyboard::Num9) {
        return std::string(1, static_cast<char>('0' + (key - sf::Keyboard::Num0)));
    }
    if (key >= sf::Keyboard::Numpad0 && key <= sf::Keyboard::Numpad9) {
        return "Num " + std::string(1, static_cast<char>('0' + (key - sf::Keyboard::Numpad0)));
    }
    if (key >= sf::Keyboard::F1 && key <= sf::Keyboard::F12) {
        return "F" + std::to_string(key - sf::Keyboard::F1 + 1);
    }
    switch (key) {
    case sf::Keyboard::Space:     return "Пробел";
    case sf::Keyboard::Enter:     return "Enter";
    case sf::Keyboard::Tab:       return "Tab";
    case sf::Keyboard::BackSpace: return "Backspace";
    case sf::Keyboard::Escape:    return "Esc";
    case sf::Keyboard::LControl:  return "L-Ctrl";
    case sf::Keyboard::RControl:  return "R-Ctrl";
    case sf::Keyboard::LShift:    return "L-Shift";
    case sf::Keyboard::RShift:    return "R-Shift";
    case sf::Keyboard::LAlt:      return "L-Alt";
    case sf::Keyboard::RAlt:      return "R-Alt";
    case sf::Keyboard::Left:      return "Влево";
    case sf::Keyboard::Right:     return "Вправо";
    case sf::Keyboard::Up:        return "Вверх";
    case sf::Keyboard::Down:      return "Вниз";
    case sf::Keyboard::Tilde:     return "~";
    default:                      return "#" + std::to_string(static_cast<int>(key));
    }
}


std::string ScreenManager::getFloorTitle(int floor) {
    switch (floor) {
    case 1:  return "ЭТАЖ 1: ПЕПЕЛЬНЫЙ НЕКРОПОЛЬ";
    case 2:  return "ЭТАЖ 2: ОАЗИС ТИШИНЫ";
    case 3:  return "ЭТАЖ 3: ЗАТОНУВШИЙ АКРОПОЛЬ";
    case 4:  return "ЭТАЖ 4: РАСКАЛЕННЫЕ МАГИСТРАЛИ";
    case 5:  return "ЭТАЖ 5: ЖЕЛЕЗНЫЙ КАРАНТИН";
    case 6:  return "ЭТАЖ 6: ИЗУМРУДНЫЙ СВОД";
    case 7:  return "ЭТАЖ 7: ЗАСТЫВШЕЕ ВРЕМЯ";
    case 8:  return "ЭТАЖ 8: ИЛЛЮЗОРНЫЙ МЕГАПОЛИС";
    case 9:  return "ЭТАЖ 9: НЕБЕСНЫЙ СОБОР";
    case 10: return "ЭТАЖ 10: СЕРДЦЕ БАШНИ";
    default: return "ЭТАЖ " + std::to_string(floor);
    }
}

std::string ScreenManager::getSkillDetailedDescription(const std::shared_ptr<SkillTemplate>& skill, const CombatUnit& hero, size_t sIdx) {
    if (!skill) return "";
    std::string desc = skill->getDescription();
    
    if (desc.empty()) {
        const std::string& name = skill->getName();
        if (sIdx == 0) {
            desc = "Базовый боевой удар без перезарядки. Наносит стабильный урон.";
        } else if (sIdx == 1) {
            if (name.find("щит") != std::string::npos || name.find("Щит") != std::string::npos ||
                name.find("кожа") != std::string::npos || name.find("Кожа") != std::string::npos) {
                desc = "Защитный навык: формирует энергетический щит, поглощающий урон.";
            } else if (name.find("исцел") != std::string::npos || name.find("дыхание") != std::string::npos || name.find("свет") != std::string::npos) {
                desc = "Навык восстановления: восстанавливает здоровье персонажа.";
            } else {
                desc = "Тактическая способность: наносит повышенный урон выбранной цели.";
            }
        } else {
            desc = "Ультимативное умение колоссальной мощи. Сокрушает вражеские ряды!";
        }
    }

    std::string full = desc + "\n\n";

    if (sIdx == 0) {
        full += "- Категория: Обычная атака (Готово)\n";
    } else if (skill->getCooldown() > 0) {
        full += "- Перезарядка: " + std::to_string(skill->getCooldown()) + " ходов\n";
    } else {
        full += "- Перезарядка: Готово к применению\n";
    }

    if (skill->getStacksGranted() > 0) {
        full += "- Накапливает: +" + std::to_string(skill->getStacksGranted()) + " стак(ов) при ударе\n";
    }
    if (skill->getConsumesAllStacks()) {
        full += "- Расход: поглощает ВСЕ накопленные стаки (текущих: " + std::to_string(hero.skillStacks) + ")\n";
    }

    double currentVal = skill->calculateValue(hero.stats, hero.skillStacks);
    full += "- Расчетная боевая сила: ~" + std::to_string(static_cast<int>(currentVal));

    return full;
}
void ScreenManager::applyGameStyle() {
    ImGuiStyle& style = ImGui::GetStyle();

    // Скругления элементов (RPG стиль)
    style.WindowRounding    = 8.0f;
    style.ChildRounding     = 6.0f;
    style.FrameRounding     = 5.0f;
    style.GrabRounding      = 4.0f;
    style.PopupRounding     = 6.0f;
    style.ScrollbarRounding = 6.0f;
    style.TabRounding       = 6.0f;

    // Отступы
    style.WindowPadding     = ImVec2(12.0f, 12.0f);
    style.FramePadding      = ImVec2(8.0f, 5.0f);
    style.ItemSpacing       = ImVec2(8.0f, 7.0f);
    style.ItemInnerSpacing  = ImVec2(6.0f, 5.0f);
    style.ScrollbarSize     = 14.0f;
    style.GrabMinSize       = 12.0f;

    // Границы
    style.WindowBorderSize  = 1.0f;
    style.ChildBorderSize   = 1.0f;
    style.FrameBorderSize   = 1.0f;
    style.PopupBorderSize   = 1.0f;

    ImVec4* c = style.Colors;

    // Фон окон — благородный темный полупрозрачный сланец
    c[ImGuiCol_WindowBg]             = ImVec4(0.06f, 0.07f, 0.11f, 0.88f);
    c[ImGuiCol_ChildBg]              = ImVec4(0.09f, 0.10f, 0.15f, 0.72f);
    c[ImGuiCol_PopupBg]              = ImVec4(0.08f, 0.09f, 0.14f, 0.96f);

    // Границы — античное полированное золото
    c[ImGuiCol_Border]               = ImVec4(0.70f, 0.55f, 0.25f, 0.70f);
    c[ImGuiCol_BorderShadow]         = ImVec4(0.00f, 0.00f, 0.00f, 0.35f);

    // Рамки элементов (Slider, Input, Combo)
    c[ImGuiCol_FrameBg]              = ImVec4(0.12f, 0.13f, 0.18f, 0.85f);
    c[ImGuiCol_FrameBgHovered]       = ImVec4(0.22f, 0.22f, 0.30f, 0.95f);
    c[ImGuiCol_FrameBgActive]        = ImVec4(0.30f, 0.30f, 0.40f, 1.00f);

    // Заголовок окна
    c[ImGuiCol_TitleBg]              = ImVec4(0.10f, 0.09f, 0.14f, 0.95f);
    c[ImGuiCol_TitleBgActive]        = ImVec4(0.20f, 0.16f, 0.10f, 1.00f);
    c[ImGuiCol_TitleBgCollapsed]     = ImVec4(0.08f, 0.08f, 0.12f, 0.75f);

    // Полоса прокрутки
    c[ImGuiCol_ScrollbarBg]          = ImVec4(0.05f, 0.05f, 0.08f, 0.80f);
    c[ImGuiCol_ScrollbarGrab]        = ImVec4(0.35f, 0.30f, 0.22f, 0.90f);
    c[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.55f, 0.45f, 0.28f, 1.00f);
    c[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.70f, 0.55f, 0.32f, 1.00f);

    // Кнопки — темная бронза с золотым отблеском
    c[ImGuiCol_Button]               = ImVec4(0.20f, 0.18f, 0.16f, 0.90f);
    c[ImGuiCol_ButtonHovered]        = ImVec4(0.38f, 0.30f, 0.18f, 1.00f);
    c[ImGuiCol_ButtonActive]         = ImVec4(0.55f, 0.42f, 0.20f, 1.00f);

    // Заголовки
    c[ImGuiCol_Header]               = ImVec4(0.22f, 0.19f, 0.14f, 0.85f);
    c[ImGuiCol_HeaderHovered]        = ImVec4(0.35f, 0.29f, 0.18f, 0.95f);
    c[ImGuiCol_HeaderActive]         = ImVec4(0.48f, 0.38f, 0.22f, 1.00f);

    // Сепаратор — золотистая линия
    c[ImGuiCol_Separator]            = ImVec4(0.60f, 0.48f, 0.24f, 0.55f);
    c[ImGuiCol_SeparatorHovered]     = ImVec4(0.80f, 0.65f, 0.30f, 0.80f);
    c[ImGuiCol_SeparatorActive]      = ImVec4(0.95f, 0.80f, 0.35f, 1.00f);

    // Выделение (слайдер / CheckMark)
    c[ImGuiCol_CheckMark]            = ImVec4(0.95f, 0.80f, 0.30f, 1.00f);
    c[ImGuiCol_SliderGrab]           = ImVec4(0.75f, 0.60f, 0.25f, 1.00f);
    c[ImGuiCol_SliderGrabActive]     = ImVec4(0.95f, 0.80f, 0.35f, 1.00f);

    // Полосы прогресса
    c[ImGuiCol_PlotHistogram]        = ImVec4(0.22f, 0.78f, 0.35f, 1.00f);
    c[ImGuiCol_PlotHistogramHovered] = ImVec4(0.30f, 0.90f, 0.45f, 1.00f);

    // Текст — мягкий пергаментный
    c[ImGuiCol_Text]                 = ImVec4(0.95f, 0.93f, 0.88f, 1.00f);
    c[ImGuiCol_TextDisabled]         = ImVec4(0.55f, 0.53f, 0.48f, 1.00f);

    // Вкладки
    c[ImGuiCol_Tab]                  = ImVec4(0.14f, 0.14f, 0.18f, 0.85f);
    c[ImGuiCol_TabHovered]           = ImVec4(0.35f, 0.30f, 0.20f, 1.00f);
    c[ImGuiCol_TabActive]            = ImVec4(0.28f, 0.22f, 0.14f, 1.00f);
    c[ImGuiCol_TabUnfocused]         = ImVec4(0.10f, 0.10f, 0.14f, 0.80f);
    c[ImGuiCol_TabUnfocusedActive]   = ImVec4(0.20f, 0.16f, 0.12f, 1.00f);
}

// =============================================================================
// ЗАГРУЗКА РЕСУРСОВ
// =============================================================================
bool ScreenManager::loadResources() {
    if (!mainMenuBgTexture.loadFromFile("assets/bg/MainMenu.png")) {
        std::cerr << "[ScreenManager] Фон assets/bg/MainMenu.png не найден!" << std::endl;
    } else {
        mainMenuBgTexture.setSmooth(true);
        mainMenuBgSprite.setTexture(mainMenuBgTexture);
    }

    if (!combatBgTexture.loadFromFile("assets/bg/combat_bg.png")) {
        std::cerr << "[ScreenManager] Фон assets/bg/combat_bg.png не найден!" << std::endl;
    } else {
        combatBgTexture.setSmooth(true);
        combatBgSprite.setTexture(combatBgTexture);
    }

    if (!lobbyBgTexture.loadFromFile("assets/bg/lobby_bg.png")) {
        std::cerr << "[ScreenManager] Фон assets/bg/lobby_bg.png не найден!" << std::endl;
    } else {
        lobbyBgTexture.setSmooth(true);
        lobbyBgSprite.setTexture(lobbyBgTexture);
    }

    // Загрузка и процедурная генерация фонов для всех 10 этажей карты
    struct FloorThemeColor { sf::Color top; sf::Color bottom; sf::Color glow; };
    FloorThemeColor themes[11] = {
        { sf::Color(16, 14, 26), sf::Color(8, 8, 14), sf::Color(25, 20, 35) },
        // 1. Пепельный Некрополь
        { sf::Color(38, 32, 42), sf::Color(14, 12, 18), sf::Color(70, 50, 75) },
        // 2. Оазис Тишины
        { sf::Color(14, 45, 36), sf::Color(6, 18, 15), sf::Color(30, 85, 65) },
        // 3. Затонувший Акрополь
        { sf::Color(12, 34, 65), sf::Color(5, 14, 30), sf::Color(25, 65, 110) },
        // 4. Раскаленные Магистрали
        { sf::Color(75, 24, 12), sf::Color(28, 8, 8), sf::Color(125, 45, 15) },
        // 5. Железный Карантин
        { sf::Color(25, 46, 30), sf::Color(14, 22, 26), sf::Color(55, 80, 45) },
        // 6. Изумрудный Свод
        { sf::Color(18, 56, 24), sf::Color(8, 25, 12), sf::Color(45, 95, 35) },
        // 7. Застывшее Время
        { sf::Color(20, 52, 78), sf::Color(8, 24, 40), sf::Color(45, 95, 130) },
        // 8. Иллюзорный Мегаполис
        { sf::Color(58, 16, 68), sf::Color(18, 8, 30), sf::Color(105, 30, 115) },
        // 9. Небесный Собор
        { sf::Color(72, 56, 20), sf::Color(30, 24, 12), sf::Color(125, 95, 35) },
        // 10. Сердце Башни
        { sf::Color(38, 12, 62), sf::Color(8, 4, 20), sf::Color(85, 25, 125) }
    };

    for (int f = 1; f <= 10; ++f) {
        std::string floorPath = "assets/bg/floor_" + std::to_string(f) + ".png";
        if (!floorBgTextures[f].loadFromFile(floorPath)) {
            // Процедурный синтез красивого текстурированного фона этажа
            sf::Image img;
            img.create(1280, 720);
            const auto& th = themes[f];
            for (unsigned int y = 0; y < 720; ++y) {
                float ty = static_cast<float>(y) / 720.0f;
                sf::Uint8 r = static_cast<sf::Uint8>(th.top.r * (1.0f - ty) + th.bottom.r * ty);
                sf::Uint8 g = static_cast<sf::Uint8>(th.top.g * (1.0f - ty) + th.bottom.g * ty);
                sf::Uint8 b = static_cast<sf::Uint8>(th.top.b * (1.0f - ty) + th.bottom.b * ty);

                for (unsigned int x = 0; x < 1280; ++x) {
                    float dx = (static_cast<float>(x) - 640.0f) / 640.0f;
                    float dy = (static_cast<float>(y) - 360.0f) / 360.0f;
                    float dist = dx * dx + dy * dy;
                    float vignette = std::clamp(1.0f - dist * 0.45f, 0.25f, 1.0f);
                    
                    // Легкий шум и свечение
                    float glow = std::clamp(1.0f - dist * 0.9f, 0.0f, 1.0f) * 0.35f;
                    sf::Uint8 cr = static_cast<sf::Uint8>(std::min(255.0f, (r * vignette + th.glow.r * glow)));
                    sf::Uint8 cg = static_cast<sf::Uint8>(std::min(255.0f, (g * vignette + th.glow.g * glow)));
                    sf::Uint8 cb = static_cast<sf::Uint8>(std::min(255.0f, (b * vignette + th.glow.b * glow)));
                    img.setPixel(x, y, sf::Color(cr, cg, cb));
                }
            }
            img.saveToFile(floorPath);
            floorBgTextures[f].loadFromFile(floorPath);
        }
        floorBgTextures[f].setSmooth(true);
        floorBgSprites[f].setTexture(floorBgTextures[f]);
    }

    std::unordered_map<ItemTab, std::string> paths = {
        {ItemTab::Weapons,    "assets/icons/weapon.png"},
        {ItemTab::Artifacts,  "assets/icons/artifact.png"},
        {ItemTab::Food,       "assets/icons/food.png"},
        {ItemTab::Materials,  "assets/icons/materials.png"},
        {ItemTab::Jewelry,    "assets/icons/jewelry.png"},
        {ItemTab::QuestItems, "assets/icons/quest.png"}
    };

    for (const auto& [tab, path] : paths) {
        if (!tabTextures[tab].loadFromFile(path)) {
            std::cerr << "[ScreenManager] Иконка не найдена: " << path << std::endl;
        } else {
            tabTextures[tab].setSmooth(true);
        }
    }
    nodeIconTextures["shop"].loadFromFile("assets/icons/jewelry.png");
    nodeIconTextures["combat"].loadFromFile("assets/icons/combat.png");
    nodeIconTextures["combat_elite"].loadFromFile("assets/icons/combat_elite.png");
    nodeIconTextures["combat_boss"].loadFromFile("assets/icons/combat_boss.png");
    nodeIconTextures["resource"].loadFromFile("assets/icons/materials.png");
    nodeIconTextures["cooking"].loadFromFile("assets/icons/food.png");
    nodeIconTextures["crafting"].loadFromFile("assets/icons/artifact.png");
    nodeIconTextures["mining"].loadFromFile("assets/icons/mining.png");

    slotIconTextures["slot_weapon"].loadFromFile("assets/icons/slot_weapon.png");
    slotIconTextures["slot_headgear"].loadFromFile("assets/icons/slot_headgear.png");
    slotIconTextures["slot_costume"].loadFromFile("assets/icons/slot_costume.png");
    slotIconTextures["slot_amulet"].loadFromFile("assets/icons/slot_amulet.png");
    slotIconTextures["slot_ring"].loadFromFile("assets/icons/slot_ring.png");

    for (const auto& [charId, _] : GameDatabase::getInstance().characters) {
        characterPortraits[charId].loadFromFile("assets/portraits/" + charId + ".png");
        characterPortraits[charId].setSmooth(true);
    }

    return true;

}

// =============================================================================
// ИНИЦИАЛИЗАЦИЯ ТОЧЕК КАРТЫ
// =============================================================================
void ScreenManager::initMapPoints() {
    mapPoints.clear();

    auto addCombat = [&](const std::string& id, const std::string& title, const std::string& desc,
                         int floor, ImVec2 pos, std::vector<EnemySpawnInfo> enemies, bool isTemp = false) {
        MapPointOfInterest pt;
        pt.id = id;
        pt.title = title;
        pt.description = desc;
        pt.floor = floor;
        pt.position = pos;
        pt.type = MapNodeType::Combat;
        if (id.find("boss") != std::string::npos) {
            pt.iconKey = "combat_boss";
        } else if (id.find("elite") != std::string::npos) {
            pt.iconKey = "combat_elite";
        } else {
            pt.iconKey = "combat";
        }
        pt.isTemporary = isTemp;
        pt.enemies = std::move(enemies);
        mapPoints.push_back(pt);
    };

    auto addShop = [&](const std::string& id, const std::string& title, const std::string& desc,
                       int floor, ImVec2 pos, const std::string& name, int balance,
                       std::vector<std::pair<std::string, int>> inventory) {
        MapPointOfInterest pt;
        pt.id = id;
        pt.title = title;
        pt.description = desc;
        pt.floor = floor;
        pt.position = pos;
        pt.type = MapNodeType::Shop;
        pt.iconKey = "shop";
        pt.isTemporary = false;
        pt.merchantName = name;
        pt.merchantInventory.setBalance(balance);
        for (const auto& [itemId, count] : inventory) {
            pt.merchantInventory.addItem(itemId, count);
        }
        mapPoints.push_back(pt);
    };

    auto addUtility = [&](const std::string& id, const std::string& title, const std::string& desc,
                          int floor, ImVec2 pos, MapNodeType type, const std::string& icon) {
        MapPointOfInterest pt;
        pt.id = id;
        pt.title = title;
        pt.description = desc;
        pt.floor = floor;
        pt.position = pos;
        pt.type = type;
        pt.iconKey = icon;
        pt.isTemporary = false;
        mapPoints.push_back(pt);
    };

    auto addMining = [&](const std::string& id, const std::string& title, const std::string& desc,
                         int floor, ImVec2 pos) {
        MapPointOfInterest pt;
        pt.id = id;
        pt.title = title;
        pt.description = desc;
        pt.floor = floor;
        pt.position = pos;
        pt.type = MapNodeType::ResourceMine;
        pt.iconKey = "mining";
        pt.isTemporary = false;
        mapPoints.push_back(pt);
    };

    // =========================================================================
    // ЭТАЖ 1: ПЕПЕЛЬНЫЙ НЕКРОПОЛЬ
    // =========================================================================
    addShop("f1_merchant", "Лавка старьевщика", "Скупает лом и продает базовое снаряжение Некрополя.",
            1, ImVec2(220.0f, 440.0f), "Старьевщик Некрополя", 350,
            {{"rusty_blade", 3}, {"iron_ore", 15}, {"bone_meal", 10}, {"dirty_water", 10}});
    addUtility("f1_cooking", "Пепельный костер", "Место для приготовления полевой еды.", 1, ImVec2(780.0f, 220.0f), MapNodeType::Cooking, "cooking");
    addCombat("f1_combat_rats", "Засада падальщиков", "Стая голодных крыс и трупоедов преграждает путь.",
              1, ImVec2(340.0f, 250.0f), {{"f1_necropolis_rat", "Чумная крыса", 1, 3}, {"f1_carrion_beetle", "Трупоядный жук", 1, 2}});
    addCombat("f1_combat_skel", "Костяной аванпост", "Укрепление восставших скелетов-разведчиков.",
              1, ImVec2(600.0f, 320.0f), {{"f1_skeleton_scout", "Скелет-разведчик", 2, 2}, {"f1_bone_hound", "Костяная гончая", 2, 2}, {"f1_decayed_zombie", "Гниющий мертвец", 2, 1}});
    addCombat("f1_elite_camp", "Склеп Могильщика", "[ЭЛИТА] Логово элитного смотрителя склепа и его мародеров.",
              1, ImVec2(450.0f, 520.0f), {{"f1_elite_grave_keeper", "Элитный могильщик", 3, 1}, {"f1_tomb_scavenger", "Могильный мародер", 2, 2}});
    addCombat("f1_boss_camp", "Логово Колосса", "[БОСС] Арена Костяного Колосса, повелителя некрополя.",
              1, ImVec2(880.0f, 450.0f), {{"f1_boss_bone_colossus", "Костяной Колосс", 5, 1}, {"f1_skeleton_scout", "Скелет-разведчик", 3, 2}});


    addMining("f1_mining_iron", "Железная жила", "Богатая жила железной руды и обломков металла в катакомбах.",
              1, ImVec2(500.0f, 170.0f));
    addCombat("f1_combat_patrol", "Дозор нежити", "Отряд бродячих скелетов патрулирует склепы.",
              1, ImVec2(170.0f, 260.0f), {{"f1_skeleton_scout", "Скелет-разведчик", 2, 3}});
    addCombat("f1_combat_ghouls", "Логово упырей", "Несколько ненасытных упырей рыщут в поисках свежей плоти.",
              1, ImVec2(720.0f, 510.0f), {{"f1_decayed_zombie", "Разложившийся зомби", 2, 2}, {"f1_bone_hound", "Костяная гончая", 2, 2}});    // =========================================================================
    // ЭТАЖ 2: ОАЗИС ТИШИНЫ
    // =========================================================================
    addShop("f2_merchant", "Торговец Оазиса", "Провизия и свежие припасы подземного оазиса.",
            2, ImVec2(200.0f, 460.0f), "Хранитель Оазиса", 600,
            {{"cave_wheat", 15}, {"goat_milk", 10}, {"gutted_fish", 8}, {"wild_berries", 20}, {"greenhouse_apple", 12}});
    addUtility("f2_cooking", "Полевая кухня", "Полевой очаг для кулинарии.", 2, ImVec2(780.0f, 200.0f), MapNodeType::Cooking, "cooking");
    addUtility("f2_crafting", "Мастерская Оазиса", "Верстак для создания снаряжения.", 2, ImVec2(500.0f, 350.0f), MapNodeType::Crafting, "crafting");
    addCombat("f2_combat_lake", "Озерные хищники", "Амфибии и хищные рыбы у кромки подземного озера.",
              2, ImVec2(350.0f, 240.0f), {{"f2_lake_lurker", "Озерный удильщик", 4, 2}, {"f2_swamp_toad", "Болотная жаба", 4, 2}});
    addCombat("f2_combat_thorns", "Шипастые заросли", "Хищные растения и ядовитые змеи в глубине грота.",
              2, ImVec2(650.0f, 380.0f), {{"f2_thorn_stalker", "Шипастый ловец", 5, 3}, {"f2_oasis_viper", "Пещерная гадюка", 5, 2}});
    addCombat("f2_elite_camp", "Роща Заросшего Стража", "[ЭЛИТА] Древний голем, охраняющий священные ростки.",
              2, ImVec2(420.0f, 540.0f), {{"f2_elite_overgrown_golem", "Заросший страж", 8, 1}, {"f2_corrupted_dryad", "Оскверненная дриада", 6, 2}});
    addCombat("f2_boss_camp", "Сердце Черного Лотоса", "[БОСС] Гигантский цветок-паразит, оплетший корни башни.",
              2, ImVec2(880.0f, 460.0f), {{"f2_boss_lotus_matriarch", "Матриарх Черного Лотоса", 10, 1}, {"f2_spore_spitter", "Споровик-плевун", 7, 2}});


    addMining("f2_mining_vein", "Заросшая жила", "Кварцевая жила с прожилками мифрила среди корней древних деревьев.",
              2, ImVec2(240.0f, 210.0f));
    addCombat("f2_combat_spiders", "Паучья засада", "Гигантские ядовитые пауки спустились с кроны.",
              2, ImVec2(560.0f, 490.0f), {{"f2_thorn_stalker", "Шипастый ловец", 5, 3}});
    addCombat("f2_combat_vipers", "Змеиный выводок", "Гнездо болотных гадюк у водопоя.",
              2, ImVec2(730.0f, 320.0f), {{"f2_oasis_viper", "Болотная гадюка", 5, 3}, {"f2_lake_lurker", "Озерный выползень", 5, 2}});    // =========================================================================
    // ЭТАЖ 3: ЗАТОНУВШИЙ АКРОПОЛЬ
    // =========================================================================
    addShop("f3_merchant", "Лавка Водолаза", "Редкие морепродукты и античные реликвии со дна.",
            3, ImVec2(200.0f, 440.0f), "Ныряльщик за жемчугом", 800,
            {{"severed_tentacle", 10}, {"raw_seaweed", 20}, {"shell_meat", 15}, {"ocean_salt", 25}, {"live_clam", 10}});
    addUtility("f3_cooking", "Очаг на плоту", "Костер на плавучей платформе акрополя.", 3, ImVec2(760.0f, 200.0f), MapNodeType::Cooking, "cooking");
    addCombat("f3_combat_patrol", "Затонувший патруль", "Тритоны и светящиеся медузы патрулируют затопленные залы.",
              3, ImVec2(340.0f, 250.0f), {{"f3_triton_spearman", "Тритон-копейщик", 8, 3}, {"f3_abyssal_jellyfish", "Бездная медуза", 8, 2}});
    addCombat("f3_combat_reefs", "Коралловые рифы", "Ожившие кораллы и длинноногие ракообразные среди колонн.",
              3, ImVec2(620.0f, 300.0f), {{"f3_coral_golem", "Коралловый голем", 9, 2}, {"f3_depth_strider", "Глубинный долгоног", 9, 2}});
    addCombat("f3_elite_camp", "Аванпост Наг", "[ЭЛИТА] Четырехрукий полководец морского легиона и угри разлома.",
              3, ImVec2(460.0f, 520.0f), {{"f3_elite_naga_commander", "Командир наг", 13, 1}, {"f3_chasm_eel", "Угорь разлома", 10, 2}});
    addCombat("f3_boss_camp", "Трон Затонувшего Полиса", "[БОСС] Древний Левиафан, владыка затопленного города.",
              3, ImVec2(880.0f, 450.0f), {{"f3_boss_leviathan_lord", "Левиафан Глубин", 15, 1}, {"f3_elite_kraken_spawn", "Отпрыск Кракена", 12, 1}});


    addMining("f3_mining_crystals", "Подводная жила", "Океанические кристаллы и минералы на шельфе.",
              3, ImVec2(480.0f, 210.0f));
    addCombat("f3_combat_raiders", "Налетчики глубин", "Группа тритонов с трезубцами перекрыла путь.",
              3, ImVec2(180.0f, 270.0f), {{"f3_triton_spearman", "Тритон-копейщик", 9, 3}});
    addCombat("f3_combat_chasm", "Твари расщелины", "Глубоководные угри выплыли на свет факелов.",
              3, ImVec2(720.0f, 530.0f), {{"f3_chasm_eel", "Угорь бездны", 10, 2}, {"f3_abyssal_jellyfish", "Глубинная медуза", 9, 2}});    // =========================================================================
    // ЭТАЖ 4: РАСКАЛЕННЫЕ МАГИСТРАЛИ
    // =========================================================================
    addShop("f4_merchant", "Дворфийская фактория", "Огнеупорные злаки и дары магматических озер.",
            4, ImVec2(220.0f, 450.0f), "Кузнец Торвальд", 1100,
            {{"fire_grain", 20}, {"salamander_meat", 12}, {"coal_dust", 30}, {"magma_resin", 10}, {"iron_ore", 25}});
    addUtility("f4_crafting", "Кузница Магмы", "Горн на геотермальной энергии.", 4, ImVec2(780.0f, 200.0f), MapNodeType::Crafting, "crafting");
    addCombat("f4_combat_magma", "Магматический рубеж", "Огненные бесы и саламандры в потоках лавы.",
              4, ImVec2(350.0f, 240.0f), {{"f4_magma_imp", "Магматический бес", 12, 4}, {"f4_fire_salamander", "Огненная саламандра", 12, 2}});
    addCombat("f4_combat_slag", "Шлаковый забой", "Шлаковые големы и безумные пеклорожденные рудокопы.",
              4, ImVec2(600.0f, 320.0f), {{"f4_slag_golem", "Шлаковый автоматон", 13, 2}, {"f4_duergar_berserker", "Дворф-пеклорожденный", 13, 2}});
    addCombat("f4_elite_camp", "Инфернальная застава", "[ЭЛИТА] Бронированный центурион и адские гончие.",
              4, ImVec2(450.0f, 540.0f), {{"f4_elite_infernal_centurion", "Инфернальный центурион", 18, 1}, {"f4_flame_hound", "Пылающий цербер", 15, 2}});
    addCombat("f4_boss_camp", "Жерло Вечного Пламени", "[БОСС] Титан Игнис, рожденный в сердце магматического разлома.",
              4, ImVec2(880.0f, 460.0f), {{"f4_boss_ignis_titan", "Игнис, Пожиратель Лавы", 20, 1}, {"f4_elite_volcanic_drake", "Вулканический драконид", 17, 1}});


    addMining("f4_mining_magma", "Магматическая жила", "Раскаленная жила с огненными кристаллами и рудой.",
              4, ImVec2(490.0f, 190.0f));
    addCombat("f4_combat_flame", "Огненная стража", "Духи пламени кружат вокруг жерла.",
              4, ImVec2(210.0f, 260.0f), {{"f4_magma_imp", "Магматический бес", 13, 3}, {"f4_flame_hound", "Пламенная гончая", 14, 2}});
    addCombat("f4_combat_duergar", "Аванпост дуэргаров", "Патруль темных кузнецов охраняет подходы к плавильням.",
              4, ImVec2(710.0f, 490.0f), {{"f4_duergar_berserker", "Дуэргар-берсерк", 14, 3}});    // =========================================================================
    // ЭТАЖ 5: ЖЕЛЕЗНЫЙ КАРАНТИН
    // =========================================================================
    addShop("f5_merchant", "Пункт Снабжения", "Синтетическая белковая масса и медикаменты.",
            5, ImVec2(200.0f, 440.0f), "Офицер Снабжения", 1400,
            {{"protein_mass", 25}, {"synth_base", 15}, {"saline", 20}, {"chem_concentrate", 12}, {"vitamin_powder", 18}});
    addUtility("f5_cooking", "Пищевой автоклав", "Стерилизатор и автоклав для питательных пайков.", 5, ImVec2(760.0f, 220.0f), MapNodeType::Cooking, "cooking");
    addCombat("f5_combat_box", "Карантинный бокс", "Лабораторные мутанты и вышедшие из строя хирургические дроны.",
              5, ImVec2(340.0f, 250.0f), {{"f5_plague_mutant", "Чумной мутант", 17, 3}, {"f5_surgical_drone", "Хирургический дрон", 17, 2}});
    addCombat("f5_combat_scrap", "Сектор утилизации", "Токсичные ползуны и кибер-ищейки охраны лаборатории.",
              5, ImVec2(600.0f, 320.0f), {{"f5_toxic_crawler", "Токсичный ползун", 18, 2}, {"f5_cyber_hound", "Кибер-ищейка", 18, 2}});
    addCombat("f5_elite_camp", "Лаборатория Хирургии", "[ЭЛИТА] Безумный Главный хирург с охраной надзирателей.",
              5, ImVec2(440.0f, 520.0f), {{"f5_elite_head_surgeon", "Главный хирург Карантина", 23, 1}, {"f5_infected_warden", "Зараженный надзиратель", 20, 2}});
    addCombat("f5_boss_camp", "Изолятор Объекта Ноль", "[БОСС] Объект Ноль — совершенный генетический хищник.",
              5, ImVec2(880.0f, 450.0f), {{"f5_boss_subject_zero", "Объект Ноль", 25, 1}, {"f5_elite_amalgam_beast", "Биомеханический амальгам", 22, 1}});


    addMining("f5_mining_scrap", "Свалка сплавов", "Залежи редких металлических сплавов и мифрила среди обломков механизмов.",
              5, ImVec2(470.0f, 200.0f));
    addCombat("f5_combat_drones", "Рой дронов", "Автономные хирургические дроны перешли в режим охоты.",
              5, ImVec2(200.0f, 260.0f), {{"f5_surgical_drone", "Хирургический дрон", 17, 3}, {"f5_toxic_crawler", "Токсичный ползун", 17, 2}});
    addCombat("f5_combat_hounds", "Кибер-гончие", "Усиленные псы рыщут по коридорам комплекса.",
              5, ImVec2(710.0f, 520.0f), {{"f5_cyber_hound", "Кибер-гончая", 18, 3}});    // =========================================================================
    // ЭТАЖ 6: ИЗУМРУДНЫЙ СВОД
    // =========================================================================
    addShop("f6_merchant", "Хижина Шамана", "Экзотические плоды гигантских джунглей и редкий нектар.",
            6, ImVec2(200.0f, 450.0f), "Шаман Древоземов", 1800,
            {{"giant_fruit", 15}, {"predator_nectar", 12}, {"antitox_root", 18}, {"lizard_meat", 15}, {"sacred_vine_leaf", 10}});
    addUtility("f6_cooking", "Очаг Племени", "Традиционный очаг охотников купола.", 6, ImVec2(780.0f, 200.0f), MapNodeType::Cooking, "cooking");
    addCombat("f6_combat_hunt", "Охотничьи угодья", "Стая джунглевых рапторов и ядовитые шершни.",
              6, ImVec2(350.0f, 240.0f), {{"f6_jungle_raptor", "Джунглевый раптор", 22, 3}, {"f6_toxic_hornet", "Ядовитый шершень", 22, 2}});
    addCombat("f6_combat_mantis", "Ущелье богомолов", "Гигантские богомолы и тяжелые споровые гориллы.",
              6, ImVec2(620.0f, 300.0f), {{"f6_giant_mantis", "Гигантский богомол", 23, 2}, {"f6_spore_gorilla", "Споровая горилла", 23, 2}});
    addCombat("f6_elite_camp", "Гнездо Карнотавра", "[ЭЛИТА] Вершитель пищевой цепочки первобытного купола.",
              6, ImVec2(460.0f, 520.0f), {{"f6_elite_apex_carnotaur", "Карнотавр-вершитель", 28, 1}, {"f6_vine_strangler", "Лиана-душитель", 25, 2}});
    addCombat("f6_boss_camp", "Сердце Изумрудного Свода", "[БОСС] Исполинский бронированный ящер размером с холм.",
              6, ImVec2(880.0f, 460.0f), {{"f6_boss_emerald_behemoth", "Изумрудный Бегемот", 30, 1}, {"f6_elite_ancient_treant", "Древний энт джунглей", 27, 1}});


    addMining("f6_mining_jungle", "Алмазная жила", "Ископаемые кристаллы среди корней древних исполинов.",
              6, ImVec2(510.0f, 200.0f));
    addCombat("f6_combat_raptors", "Стая хищников", "Быстрые джунглевые рапторы окружили тропу.",
              6, ImVec2(190.0f, 270.0f), {{"f6_jungle_raptor", "Джунглевый раптор", 22, 3}});
    addCombat("f6_combat_hornets", "Гнездо шершней", "Рой ядовитых гигантских шершней в ветвях.",
              6, ImVec2(720.0f, 500.0f), {{"f6_toxic_hornet", "Токсичный шершень", 21, 4}});    // =========================================================================
    // ЭТАЖ 7: ЗАСТЫВШЕЕ ВРЕМЯ
    // =========================================================================
    addShop("f7_merchant", "Хроно-лавка", "Морозные деликатесы и кристаллы застывшего времени.",
            7, ImVec2(220.0f, 440.0f), "Хрономант-отшельник", 2200,
            {{"frost_truffle", 10}, {"ice_apple", 20}, {"snow_wolf_meat", 15}, {"crystal_juice", 12}, {"magic_ice_crumb", 25}});
    addUtility("f7_crafting", "Хроно-верстак", "Мастерская в стазис-поле.", 7, ImVec2(760.0f, 200.0f), MapNodeType::Crafting, "crafting");
    addCombat("f7_combat_frost", "Ледяной заслон", "Ледяные призраки и белые волки метели.",
              7, ImVec2(340.0f, 250.0f), {{"f7_frost_wraith", "Ледяной призрак", 27, 3}, {"f7_blizzard_wolf", "Волк метели", 27, 2}});
    addCombat("f7_combat_clock", "Зал маятников", "Хроно-автоматоны и кристальные големы.",
              7, ImVec2(620.0f, 320.0f), {{"f7_chrono_automaton", "Хроно-автоматон", 28, 2}, {"f7_crystal_golem", "Кристальный голем", 28, 2}});
    addCombat("f7_elite_camp", "Бастион Вечности", "[ЭЛИТА] Паладин вне времени в мерцающих хроно-латах.",
              7, ImVec2(440.0f, 520.0f), {{"f7_elite_chrono_knight", "Рыцарь Вечности", 33, 1}, {"f7_frozen_remnant", "Замерзший легионер", 30, 2}});
    addCombat("f7_boss_camp", "Маятник Судеб", "[БОСС] Хранитель песочных часов башни, скованный вечным льдом.",
              7, ImVec2(880.0f, 450.0f), {{"f7_boss_chronos_the_frozen", "Кронос Застывший", 35, 1}, {"f7_elite_frost_archmage", "Морозный архимаг", 32, 1}});


    addMining("f7_mining_ice", "Ледяная жила", "Мифриловые самородки, вмороженные в вековой лед.",
              7, ImVec2(490.0f, 210.0f));
    addCombat("f7_combat_wolves", "Стая метели", "Свирепые снежные волки преграждают перевал.",
              7, ImVec2(190.0f, 280.0f), {{"f7_blizzard_wolf", "Снежный волк", 26, 3}});
    addCombat("f7_combat_golems", "Кристальные големы", "Стражи ледника восстают из толщи снега.",
              7, ImVec2(710.0f, 510.0f), {{"f7_crystal_golem", "Кристальный голем", 27, 2}, {"f7_frozen_remnant", "Ледяной остаток", 26, 2}});    // =========================================================================
    // ЭТАЖ 8: ИЛЛЮЗОРНЫЙ МЕГАПОЛИС
    // =========================================================================
    addShop("f8_merchant", "Неоновый дилер", "Синтетическая еда, неоновые концентраты и плазменные батареи.",
            8, ImVec2(200.0f, 440.0f), "Кибер-бармен 2.0", 2800,
            {{"food_powder", 30}, {"neon_extract", 15}, {"taste_matrix", 12}, {"plastic_protein", 20}, {"synth_water", 25}});
    addUtility("f8_crafting", "Кибер-мастерская", "Станция пайки и сборки аугментаций.", 8, ImVec2(780.0f, 220.0f), MapNodeType::Crafting, "crafting");
    addCombat("f8_combat_alley", "Неоновый переулок", "Неоновые сталкеры и летающие дроны-снайперы.",
              8, ImVec2(340.0f, 240.0f), {{"f8_neon_stalker", "Неоновый сталкер", 32, 3}, {"f8_drone_sniper", "Дрон-снайпер", 32, 2}});
    addCombat("f8_combat_net", "Серверная подсеть", "Голографические фантомы и опасные глитч-гончие.",
              8, ImVec2(620.0f, 320.0f), {{"f8_holo_phantom", "Голографический фантом", 33, 2}, {"f8_glitch_hound", "Глитч-гончая", 33, 2}});
    addCombat("f8_elite_camp", "Штаб Киборгов", "[ЭЛИТА] Киборг-полководец синдиката и андроиды подавления.",
              8, ImVec2(450.0f, 520.0f), {{"f8_elite_cyborg_warlord", "Киборг-полководец", 38, 1}, {"f8_enforcer_android", "Андроид-силовик", 35, 2}});
    addCombat("f8_boss_camp", "Ядро Сети Нексус", "[БОСС] Центральный ИИ башни, материализовавший цифровое тело божества.",
              8, ImVec2(880.0f, 460.0f), {{"f8_boss_overseer_ai", "Архитектор «Нексус»", 40, 1}, {"f8_elite_matrix_assassin", "Ассасин Матрицы", 37, 1}});


    addMining("f8_mining_shadow", "Теневая жила", "Жила с осколками пустоты и редкими металлами в переулках мегаполиса.",
              8, ImVec2(490.0f, 220.0f));
    addCombat("f8_combat_androids", "Патруль синдиката", "Тяжеловооруженные андроиды зачищают сектор.",
              8, ImVec2(190.0f, 270.0f), {{"f8_enforcer_android", "Андроид-каратель", 31, 3}});
    addCombat("f8_combat_glitch", "Глитч-фантомы", "Искаженные цифровые фантомы мерцают в неоновом тумане.",
              8, ImVec2(720.0f, 510.0f), {{"f8_holo_phantom", "Голо-фантом", 31, 2}, {"f8_glitch_hound", "Глитч-гончая", 31, 2}});    // =========================================================================
    // ЭТАЖ 9: НЕБЕСНЫЙ СОБОР
    // =========================================================================
    addShop("f9_merchant", "Монастырская лавка", "Священная провизия, золотая пшеница и небесный нектар.",
            9, ImVec2(200.0f, 450.0f), "Брат Августин", 3500,
            {{"golden_wheat", 20}, {"lightbird_breast", 15}, {"sky_nectar", 10}, {"holy_water", 25}, {"blessed_seed", 15}, {"ether_spice", 8}});
    addUtility("f9_cooking", "Священная трапезная", "Освященная печь Небесного Собора.", 9, ImVec2(760.0f, 200.0f), MapNodeType::Cooking, "cooking");
    addCombat("f9_combat_gates", "Врата Инквизиции", "Лучезарные фанатики и херувимы с луками света.",
              9, ImVec2(340.0f, 250.0f), {{"f9_radiant_zealot", "Лучезарный фанатик", 37, 3}, {"f9_cherub_archer", "Херувим-лучник", 37, 2}});
    addCombat("f9_combat_altar", "Алтарь Очищения", "Витражные големы и служители света.",
              9, ImVec2(600.0f, 320.0f), {{"f9_stained_glass_golem", "Витражный голем", 38, 2}, {"f9_light_acolyte", "Служитель света", 38, 2}});
    addCombat("f9_elite_camp", "Верховный Трибунал", "[ЭЛИТА] Паладин Небесного Золота и серафимы.",
              9, ImVec2(450.0f, 520.0f), {{"f9_elite_high_templar", "Высший тамплиер", 43, 1}, {"f9_seraph_sentinel", "Страж-серафим", 40, 2}});
    addCombat("f9_boss_camp", "Престол Небес", "[БОСС] Архонт Люминарис — шестикрылый патриарх Небесного Собора.",
              9, ImVec2(880.0f, 450.0f), {{"f9_boss_archon_luminaris", "Архонт Люминарис", 45, 1}, {"f9_elite_archangel_avenger", "Мстительный архангел", 42, 1}});


    addMining("f9_mining_void", "Жила разлома", "Чистейшие осколки пустоты и небесные металлы у Престола Света.",
              9, ImVec2(500.0f, 210.0f));
    addCombat("f9_combat_seraphs", "Карающий авангард", "Серафимы-часовые преграждают путь грешникам.",
              9, ImVec2(200.0f, 270.0f), {{"f9_seraph_sentinel", "Серафим-часовой", 36, 3}});
    addCombat("f9_combat_zealots", "Круг фанатиков", "Ревностные последователи света отражают вторжение.",
              9, ImVec2(720.0f, 500.0f), {{"f9_radiant_zealot", "Лучезарный фанатик", 36, 3}});    // =========================================================================
    // ЭТАЖ 10: СЕРДЦЕ БАШНИ (ТРОН ПЕРВОРОДНОГО)
    // =========================================================================
    addUtility("f10_crafting", "Алтарь Сингулярности", "Кузница на грани ткани реальности.", 10, ImVec2(220.0f, 440.0f), MapNodeType::Crafting, "crafting");
    addUtility("f10_cooking", "Источник Эфира", "Очаг из чистой космической энергии.", 10, ImVec2(780.0f, 220.0f), MapNodeType::Cooking, "cooking");
    addCombat("f10_combat_rift", "Астральный разлом", "Странники Пустоты и огоньки сингулярности.",
              10, ImVec2(340.0f, 250.0f), {{"f10_void_walker", "Странник Пустоты", 42, 3}, {"f10_singularity_wisp", "Огонек Сингулярности", 42, 2}});
    addCombat("f10_combat_oblivion", "Омут Забвения", "Рыцари Забвения и големы энтропии.",
              10, ImVec2(600.0f, 320.0f), {{"f10_oblivion_knight", "Рыцарь Забвения", 43, 2}, {"f10_entropy_golem", "Голем Энтропии", 43, 2}});
    addCombat("f10_elite_camp", "Цитадель Бездны", "[ЭЛИТА] Монарх темных звездных скоплений и порождения хаоса.",
              10, ImVec2(450.0f, 520.0f), {{"f10_elite_abyss_monarch", "Монарх Бездны", 48, 1}, {"f10_chaos_spawn", "Порождение Хаоса", 45, 2}});
    addCombat("f10_boss_camp", "Трон Первородного", "[БОСС] Создатель Башни, повелевающий законами мироздания.",
              10, ImVec2(880.0f, 450.0f), {{"f10_boss_primordial_origin", "Первородный Создатель", 50, 1}, {"f10_elite_primordial_warden", "Первородный Страж Трона", 47, 1}});

    addMining("f10_mining_celestial", "Небесная жила", "Небесные слитки и самородки божественной руды в ядре мироздания.",
              10, ImVec2(490.0f, 210.0f));
    addCombat("f10_combat_wisps", "Сгустки сингулярности", "Странствующие искры первородной сингулярности искажают пространство.",
              10, ImVec2(200.0f, 270.0f), {{"f10_singularity_wisp", "Искра сингулярности", 42, 3}});
    addCombat("f10_combat_knights", "Рыцари забвения", "Элитная стража бездны не пропускает никого к истоку.",
              10, ImVec2(710.0f, 510.0f), {{"f10_oblivion_knight", "Рыцарь забвения", 43, 2}, {"f10_void_walker", "Странник пустоты", 42, 2}});}

void ScreenManager::renderBackground(sf::RenderWindow& window) {
    sf::Vector2u winSize = window.getSize();
    if (winSize.x == 0 || winSize.y == 0) return;

    sf::Sprite* activeSprite = nullptr;

    if (currentScreen == ScreenState::MainMenu || currentScreen == ScreenState::Settings || currentScreen == ScreenState::Saves) {
        if (mainMenuBgTexture.getSize().x > 0) {
            activeSprite = &mainMenuBgSprite;
        }
    } else if (currentScreen == ScreenState::Combat) {
        if (combatBgTexture.getSize().x > 0) {
            activeSprite = &combatBgSprite;
        }
    } else if (currentScreen == ScreenState::Map) {
        // НА КАЖДОМ ЭТАЖЕ КАРТЫ СВОЙ ФОН В СООТВЕТСТВИИ С ТЕМАТИКОЙ
        if (currentFloor >= 1 && currentFloor <= 10 && floorBgTextures[currentFloor].getSize().x > 0) {
            activeSprite = &floorBgSprites[currentFloor];
        } else if (lobbyBgTexture.getSize().x > 0) {
            activeSprite = &lobbyBgSprite;
        }
    } else {
        if (lobbyBgTexture.getSize().x > 0) {
            activeSprite = &lobbyBgSprite;
        }
    }

    if (activeSprite && activeSprite->getTexture()) {
        sf::Vector2u texSize = activeSprite->getTexture()->getSize();
        if (texSize.x > 0 && texSize.y > 0) {
            activeSprite->setScale(
                static_cast<float>(winSize.x) / texSize.x,
                static_cast<float>(winSize.y) / texSize.y
            );
            window.draw(*activeSprite);
            return;
        }
    }

    // Процедурный темный фэнтези-градиент
    sf::VertexArray bg(sf::Quads, 4);
    bg[0].position = sf::Vector2f(0.f, 0.f);
    bg[1].position = sf::Vector2f(static_cast<float>(winSize.x), 0.f);
    bg[2].position = sf::Vector2f(static_cast<float>(winSize.x), static_cast<float>(winSize.y));
    bg[3].position = sf::Vector2f(0.f, static_cast<float>(winSize.y));

    bg[0].color = sf::Color(16, 14, 26);
    bg[1].color = sf::Color(24, 20, 36);
    bg[2].color = sf::Color(8, 8, 14);
    bg[3].color = sf::Color(12, 10, 20);
    window.draw(bg);
}

ScreenManager::ScreenManager() {
    // Инициализация базы данных и точек карты
    GameDatabase::getInstance().init();
    initMapPoints();

    // Начальный состав отряда
    if (playerProfile.getCharacterCount() == 0) {
        playerProfile.obtainCharacter("adeir");
        playerProfile.obtainCharacter("renwick");
        playerProfile.obtainCharacter("garosh");
    }

    initAchievements();
    loadGlobalAchievements();
    initQuests();

    logConsole("[Система] Добро пожаловать в игру! Нажмите ~ для консоли.", ImVec4(0.4f, 0.85f, 1.0f, 1.0f));
    logConsole("[Система] Введите 'help' для списка доступных команд.", ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
}

// Перехват клавиши ~ (Tilde)
void ScreenManager::processEvent(const sf::Event& event) {
    // 1. Режим переназначения клавиши в настройках
    if (remappingActionIndex >= 0 && event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Escape) {
            remappingActionIndex = -1;
            return;
        }
        if (event.key.code != sf::Keyboard::Unknown && event.key.code != sf::Keyboard::Tilde) {
            combatKeybinds.setKey(static_cast<CombatAction>(remappingActionIndex), event.key.code);
            remappingActionIndex = -1;
            return;
        }
    }

    // 2. Если открыт диалог — перехватываем нажатия клавиш только для него
    if (isDialogueOpen) {
        if (event.type == sf::Event::KeyPressed) {
            if (currentDialogueIndex >= 0 && currentDialogueIndex < static_cast<int>(currentDialogue.size())) {
                const auto& node = currentDialogue[currentDialogueIndex];
                if (node.options.empty()) {
                    advanceDialogue(node.nextNodeIndex);
                }
            }
        }
        return;
    }

    // 3. Обычная обработка клавиши ~ для консоли
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Tilde) {
            isConsoleOpen = !isConsoleOpen;
            return;
        }
    }

    // 4. Управление с клавиатуры для боя:
    // Клавиши 1-3 переключают персонажей, q - ульта, w - навык, e - обычная атака
    if (currentScreen == ScreenState::Combat && !isConsoleOpen && !ImGui::GetIO().WantTextInput) {
        if (event.type == sf::Event::KeyPressed) {
            sf::Keyboard::Key key = event.key.code;

            // Смена активного бойца 1..3
            bool isSlot0 = (key == combatKeybinds.selectHero1) || (combatKeybinds.selectHero1 == sf::Keyboard::Num1 && key == sf::Keyboard::Numpad1);
            bool isSlot1 = (key == combatKeybinds.selectHero2) || (combatKeybinds.selectHero2 == sf::Keyboard::Num2 && key == sf::Keyboard::Numpad2);
            bool isSlot2 = (key == combatKeybinds.selectHero3) || (combatKeybinds.selectHero3 == sf::Keyboard::Num3 && key == sf::Keyboard::Numpad3);

            if (isSlot0) {
                if (combatPhase == CombatPhase::PlayerTurn && combatSquad.size() > 0 && combatSquad[0].isAlive()) {
                    activeCharacterIndex = 0;
                    logCombat("Вы переключились на бойца: " + combatSquad[0].name, ImVec4(0.4f, 1.0f, 0.4f, 1.0f));
                }
            }
            else if (isSlot1) {
                if (combatPhase == CombatPhase::PlayerTurn && combatSquad.size() > 1 && combatSquad[1].isAlive()) {
                    activeCharacterIndex = 1;
                    logCombat("Вы переключились на бойца: " + combatSquad[1].name, ImVec4(0.4f, 1.0f, 0.4f, 1.0f));
                }
            }
            else if (isSlot2) {
                if (combatPhase == CombatPhase::PlayerTurn && combatSquad.size() > 2 && combatSquad[2].isAlive()) {
                    activeCharacterIndex = 2;
                    logCombat("Вы переключились на бойца: " + combatSquad[2].name, ImVec4(0.4f, 1.0f, 0.4f, 1.0f));
                }
            }
            // Способности:
            // e - обычная атака (индекс 0)
            else if (key == combatKeybinds.basicAttack) {
                processPlayerSkill(0);
            }
            // w - навык (индекс 1)
            else if (key == combatKeybinds.skill) {
                processPlayerSkill(1);
            }
            // q - ульта (индекс 2)
            else if (key == combatKeybinds.ultimate) {
                processPlayerSkill(2);
            }
        }
    }
}

// -- ДИАЛОГ --
void ScreenManager::advanceDialogue(int nextIndex) {
    if (nextIndex == END_DIALOGUE) {
        isDialogueOpen = false;
        currentDialogue.clear();
        currentDialogueIndex = 0;
        return;
    }

    if (nextIndex == NEXT_SEQUENTIAL) {
        currentDialogueIndex++;
    }
    else {
        currentDialogueIndex = nextIndex;
    }

    if (currentDialogueIndex < 0 || currentDialogueIndex >= static_cast<int>(currentDialogue.size())) {
        isDialogueOpen = false;
        currentDialogue.clear();
        currentDialogueIndex = 0;
    }
}

void ScreenManager::renderDialogue() {
    if (currentDialogueIndex < 0 || currentDialogueIndex >= static_cast<int>(currentDialogue.size())) {
        isDialogueOpen = false;
        return;
    }

    const auto& node = currentDialogue[currentDialogueIndex];
    ImVec2 displaySize = ImGui::GetIO().DisplaySize;

    float marginX = 40.0f;
    float marginBottom = 25.0f;
    float windowHeight = 230.0f;
    ImVec2 windowSize = ImVec2(displaySize.x - (marginX * 2.0f), windowHeight);
    ImVec2 windowPos = ImVec2(marginX, displaySize.y - windowHeight - marginBottom);

    ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);
    ImGui::SetNextWindowFocus();

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoSavedSettings;

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.08f, 0.09f, 0.12f, 0.96f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.65f, 0.55f, 0.30f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 2.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20.0f, 15.0f));

    ImGui::Begin("##DialogueOverlayWindow", nullptr, flags);

    SpeakerInfo speaker = getSpeakerInfo(node.speaker);

    ImGui::TextColored(speaker.color, "[ %s ]", speaker.name.c_str());
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::PushTextWrapPos(ImGui::GetWindowWidth() - 25.0f);
    ImGui::TextUnformatted(node.text.c_str());
    ImGui::PopTextWrapPos();

    ImGui::Spacing();

    if (!node.options.empty()) {
        ImGui::Spacing();
        for (size_t i = 0; i < node.options.size(); ++i) {
            ImGui::PushID(static_cast<int>(i));
            std::string btnLabel = " > " + node.options[i].text;
            if (ImGui::Button(btnLabel.c_str(), ImVec2(windowSize.x - 40.0f, 32.0f))) {
                advanceDialogue(node.options[i].targetNodeIndex);
            }
            ImGui::PopID();
        }
    }
    else {
        ImGui::SetCursorPosY(windowHeight - 35.0f);
        ImGui::SetCursorPosX(windowSize.x - 360.0f);
        ImGui::TextDisabled("[Нажмите любую клавишу для продолжения...]");
    }

    ImGui::End();

    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(2);
}

// =============================================================================
// НАСТРОЙКИ: РЕАЛИЗАЦИЯ МЕТОДОВ
// =============================================================================

void ScreenManager::setWindowMode(WindowMode mode) {
    pendingWindowMode = mode;
    windowModeChanged = true;
}

void ScreenManager::setMusicVolume(float vol) {
    musicVolume = std::clamp(vol, 0.0f, 100.0f);
}

void ScreenManager::setSfxVolume(float vol) {
    sfxVolume = std::clamp(vol, 0.0f, 100.0f);
}

const char* ScreenManager::getLanguageCode() const {
    switch (currentLanguage) {
    case GameLanguage::Russian: return "ru";
    case GameLanguage::English: return "en";
    }
    return "ru";
}

const char* ScreenManager::getLanguageName() const {
    switch (currentLanguage) {
    case GameLanguage::Russian: return "\xd0\xa0\xd1\x83\xd1\x81\xd1\x81\xd0\xba\xd0\xb8\xd0\xb9";       // "Русский" in UTF-8 bytes
    case GameLanguage::English: return "English";
    }
    return "\xd0\xa0\xd1\x83\xd1\x81\xd1\x81\xd0\xba\xd0\xb8\xd0\xb9";
}

void ScreenManager::applyWindowMode(sf::RenderWindow& window, WindowMode mode) {
#ifdef _WIN32
    HWND hwnd = window.getSystemHandle();
    if (!hwnd) return;

    LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
    style &= ~WS_OVERLAPPEDWINDOW;
    style |= WS_POPUP;
    SetWindowLongPtr(hwnd, GWL_STYLE, style);

    HMONITOR hMon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    GetMonitorInfo(hMon, &mi);

    int posX = mi.rcMonitor.left;
    int posY = mi.rcMonitor.top;
    int winW = mi.rcMonitor.right - mi.rcMonitor.left;
    int winH = mi.rcMonitor.bottom - mi.rcMonitor.top;

    SetWindowPos(hwnd, (mode == WindowMode::Fullscreen) ? HWND_TOPMOST : HWND_NOTOPMOST,
                 posX, posY, winW, winH, SWP_FRAMECHANGED | SWP_SHOWWINDOW);

    window.setSize(sf::Vector2u(winW, winH));
    window.setView(sf::View(sf::FloatRect(0.f, 0.f, static_cast<float>(winW), static_cast<float>(winH))));
#endif
    currentWindowMode = mode;
}

void ScreenManager::update(sf::RenderWindow& window) {
    if (shouldQuit) {
        window.close();
        return;
    }

    if (windowModeChanged) {
        applyWindowMode(window, pendingWindowMode);
        windowModeChanged = false;
    }
}

// =============================================================================
// ГЛАВНЫЙ РЕНДЕР
// =============================================================================
void ScreenManager::renderCurrentScreen() {
    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoBringToFrontOnFocus;

    std::string windowTitle = "ScreenContainer##" + std::to_string(static_cast<int>(currentScreen));

    // Полупрозрачный фон контейнера экранов для видимости фоновой графики игры
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.04f, 0.05f, 0.08f, 0.50f));
    ImGui::Begin(windowTitle.c_str(), nullptr, windowFlags);

    if (isDialogueOpen) {
        ImGui::BeginDisabled(true);
    }

    switch (currentScreen) {
    case ScreenState::MainMenu: renderMainMenu(); break;
    case ScreenState::Settings: renderSettings(); break;
    case ScreenState::Saves: renderSaves(); break;
    case ScreenState::Lobby: renderLobby(); break;
    case ScreenState::Shop: renderShop(); break;
    case ScreenState::Inventory: renderInventory(); break;
    case ScreenState::Map: renderMap(); break;
    case ScreenState::Achievements: renderAchievements(); break;
    case ScreenState::SquadEditor: renderSquadEditor(); break;
    case ScreenState::CharacterEditor: renderCharacterEditor(); break;
    case ScreenState::Combat: renderCombat(); break;
    case ScreenState::Crafting: renderCrafting(); break;
    case ScreenState::Cooking: renderCooking(); break;
    case ScreenState::Mining: renderMining(); break;
    case ScreenState::Quests: renderQuests(); break;
    }

    if (isDialogueOpen) {
        ImGui::EndDisabled();
    }

    ImGui::End();
    ImGui::PopStyleColor(); // ImGuiCol_WindowBg

    // Отрисовка диалогового окна поверх текущего экрана
    if (isDialogueOpen) {
        renderDialogue();
    }

    // Рендерим консоль поверх основного экрана (если она открыта)
    if (isConsoleOpen) {
        renderConsole();
    }
}

// =============================================================================
// КОНСОЛЬ РАЗРАБОТЧИКА И КОМАНДЫ
// =============================================================================

static std::string toLowerCmdStr(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return s;
}

static std::vector<std::string> splitCmdTokens(const std::string& str) {
    std::vector<std::string> tokens;
    std::string token;
    for (char ch : str) {
        if (std::isspace(static_cast<unsigned char>(ch))) {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
        } else {
            token += ch;
        }
    }
    if (!token.empty()) {
        tokens.push_back(token);
    }
    return tokens;
}

static const char* getItemCategoryDisplayName(ItemCategory cat) {
    switch (cat) {
    case ItemCategory::Food: return "Еда (Food)";
    case ItemCategory::Weapon: return "Оружие (Weapon)";
    case ItemCategory::Material: return "Материалы (Material)";
    case ItemCategory::Artifact: return "Артефакты (Artifact)";
    case ItemCategory::Jewelry: return "Драгоценности (Jewelry)";
    case ItemCategory::Quest: return "Квестовые (Quest)";
    }
    return "Неизвестно";
}

void ScreenManager::logConsole(const std::string& message, ImVec4 color) {
    consoleHistory.push_back({ message, color });
    if (consoleHistory.size() > 200) {
        consoleHistory.erase(consoleHistory.begin());
    }
    scrollToBottomConsole = true;
}

void ScreenManager::executeConsoleCommand(const std::string& rawLine) {
    auto tokens = splitCmdTokens(rawLine);
    if (tokens.empty()) return;

    // Выводим в лог введенную строку команды
    logConsole("> " + rawLine, ImVec4(0.75f, 0.75f, 0.85f, 1.0f));

    std::string cmd = toLowerCmdStr(tokens[0]);

    // Команда: help
    if (cmd == "help" || cmd == "помощь" || cmd == "?") {
        logConsole("=== СПИСОК ДОСТУПНЫХ КОМАНД ===", ImVec4(1.0f, 0.85f, 0.3f, 1.0f));
        logConsole("1. Выдача предметов/героев: give <тип> <id> [кол-во]", ImVec4(0.4f, 0.9f, 1.0f, 1.0f));
        logConsole("   give all [кол-во] — выдать всех героев и все предметы", ImVec4(1.0f, 0.85f, 0.25f, 1.0f));
        logConsole("2. Удаление предметов/героев: remove <тип> <id> [кол-во]", ImVec4(0.4f, 0.9f, 1.0f, 1.0f));
        logConsole("3. Достижения: achievement <добавить|удалить> <имя_или_id>", ImVec4(0.95f, 0.80f, 0.35f, 1.0f));
        logConsole("   Пример: achievement добавить global_floor_5", ImVec4(0.85f, 0.85f, 0.85f, 1.0f));
        logConsole("           достижение удалить Первые шаги", ImVec4(0.85f, 0.85f, 0.85f, 1.0f));
        logConsole("4. Очистка: clear (очистить историю)", ImVec4(0.8f, 0.8f, 0.8f, 1.0f));
        return;
    }

    // Команда: achievement / ach / достижение
    if (cmd == "achievement" || cmd == "ach" || cmd == "достижение") {
        if (tokens.size() < 3) {
            logConsole("Ошибка: недостаточно параметров команды!", ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
            logConsole("Синтаксис: achievement <добавить|удалить> <имя_или_id_достижения>", ImVec4(1.0f, 0.85f, 0.35f, 1.0f));
            logConsole("Примеры:", ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
            logConsole("  achievement добавить global_floor_5", ImVec4(0.85f, 0.85f, 0.85f, 1.0f));
            logConsole("  достижение удалить Первые шаги", ImVec4(0.85f, 0.85f, 0.85f, 1.0f));
            return;
        }

        std::string action = toLowerCmdStr(tokens[1]);
        bool isAdd = (action == "add" || action == "give" || action == "добавить" || action == "выдать");
        bool isRemove = (action == "remove" || action == "del" || action == "delete" || action == "удалить" || action == "забрать");

        if (!isAdd && !isRemove) {
            logConsole("Неизвестное действие: '" + tokens[1] + "'. Допустимо: добавить / удалить (add / remove).", ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
            return;
        }

        std::string target;
        for (size_t t = 2; t < tokens.size(); ++t) {
            if (!target.empty()) target += " ";
            target += tokens[t];
        }

        if (isAdd) {
            if (unlockAchievement(target)) {
                logConsole("[Достижения] Игроку успешно выдано достижение: '" + target + "'!", ImVec4(0.35f, 1.0f, 0.45f, 1.0f));
            } else {
                logConsole("[Ошибка] Достижение '" + target + "' не найдено!", ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
            }
        } else {
            if (lockAchievement(target)) {
                logConsole("[Достижения] Достижение '" + target + "' удалено у игрока.", ImVec4(1.0f, 0.85f, 0.35f, 1.0f));
            } else {
                logConsole("[Ошибка] Достижение '" + target + "' не найдено!", ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
            }
        }
        return;
    }

    // Команда: clear
    if (cmd == "clear" || cmd == "cls") {
        consoleHistory.clear();
        logConsole("[Консоль] История консоли очищена.", ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
        return;
    }

    // Команда: give
    if (cmd == "give" || cmd == "выдать") {
        if (tokens.size() < 2) {
            logConsole("Ошибка: недостаточно аргументов!", ImVec4(1.0f, 0.35f, 0.35f, 1.0f));
            logConsole("Синтаксис: give <категория> <id> [количество]  ИЛИ  give all [кол-во]", ImVec4(1.0f, 0.75f, 0.3f, 1.0f));
            logConsole("Для персонажей: give character <id>", ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
            return;
        }

        std::string secondArg = toLowerCmdStr(tokens[1]);

        // ==========================================
        // СПЕЦИАЛЬНЫЙ РЕЖИМ: give all
        // ==========================================
        if (secondArg == "all" || secondArg == "все" || secondArg == "всё") {
            auto& db = GameDatabase::getInstance();

            // 1. Выдаем всех персонажей игры
            int charsAdded = 0;
            for (const auto& [charId, charTmpl] : db.characters) {
                if (!playerProfile.hasCharacter(charId)) {
                    playerProfile.obtainCharacter(charId);
                    charsAdded++;
                }
            }
            tempSquad = playerProfile.getActiveSquad();

            // 2. Выдаем все предметы игры
            int itemsCount = 0;
            int defaultAmount = 10;
            if (tokens.size() >= 3) {
                try {
                    defaultAmount = std::max(1, std::min(99999, std::stoi(tokens[2])));
                } catch (...) {
                    defaultAmount = 10;
                }
            }

            for (const auto& [itemId, itemTmpl] : db.items) {
                playerInventory.addItem(itemId, defaultAmount);
                itemsCount++;
            }

            logConsole("=== РЕЖИМ ТЕСТИРОВАНИЯ: GIVE ALL ===", ImVec4(1.0f, 0.85f, 0.25f, 1.0f));
            logConsole("Успешно: разблокированы все персонажи (" + std::to_string(db.characters.size()) + 
                       " шт., новых в профиль: " + std::to_string(charsAdded) + ").", ImVec4(0.3f, 1.0f, 0.4f, 1.0f));
            logConsole("Успешно: выданы все предметы игры (" + std::to_string(itemsCount) + 
                       " видов по " + std::to_string(defaultAmount) + " шт. в инвентарь).", ImVec4(0.3f, 1.0f, 0.4f, 1.0f));
            return;
        }

        if (tokens.size() < 3) {
            logConsole("Ошибка: недостаточно аргументов!", ImVec4(1.0f, 0.35f, 0.35f, 1.0f));
            logConsole("Синтаксис: give <категория> <id> [количество]", ImVec4(1.0f, 0.75f, 0.3f, 1.0f));
            logConsole("Для персонажей: give character <id>", ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
            return;
        }

        std::string catStr = toLowerCmdStr(tokens[1]);

        // --- КАТЕГОРИЯ: ПЕРСОНАЖИ ---
        if (catStr == "character" || catStr == "char" || catStr == "hero" || 
            catStr == "персонаж" || catStr == "герой") {

            // Логичное ограничение: для персонажей количество не указывается
            if (tokens.size() > 3) {
                logConsole("Ограничение: для персонажей нельзя указывать количество! Персонаж уникален и выдается в одном экземпляре.", ImVec4(1.0f, 0.5f, 0.2f, 1.0f));
                return;
            }

            std::string charId = toLowerCmdStr(tokens[2]);
            auto& dbChars = GameDatabase::getInstance().characters;
            auto it = dbChars.find(charId);
            if (it == dbChars.end()) {
                logConsole("Ошибка: персонаж с ID '" + charId + "' не найден в базе данных!", ImVec4(1.0f, 0.35f, 0.35f, 1.0f));
                logConsole("Доступные персонажи: adeir, renwick, garosh, vex, kaelim, velari, tauros, lumien, silas, finn, elix, tork, mira, roland, born, mortis, siegfried, sylvana, drake, cormac", ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
                return;
            }

            // Логичное ограничение: проверка на дубликат персонажа
            if (playerProfile.hasCharacter(charId)) {
                logConsole("Ограничение: персонаж '" + it->second->getName() + "' (ID: " + charId + ") уже есть в вашем отряде/резерве!", ImVec4(1.0f, 0.75f, 0.2f, 1.0f));
                return;
            }

            playerProfile.obtainCharacter(charId);
            tempSquad = playerProfile.getActiveSquad();
            logConsole("Успешно: персонаж '" + it->second->getName() + "' (ID: " + charId + ") добавлен в отряд/резерв!", ImVec4(0.3f, 1.0f, 0.4f, 1.0f));
            return;
        }

        // --- КАТЕГОРИИ ПРЕДМЕТОВ ---
        ItemCategory targetCategory;
        std::string catDisplayName;
        bool catFound = false;

        if (catStr == "food" || catStr == "еда") {
            targetCategory = ItemCategory::Food; catDisplayName = "Еда"; catFound = true;
        } else if (catStr == "weapon" || catStr == "weapons" || catStr == "оружие") {
            targetCategory = ItemCategory::Weapon; catDisplayName = "Оружие"; catFound = true;
        } else if (catStr == "material" || catStr == "materials" || catStr == "материал" || catStr == "материалы") {
            targetCategory = ItemCategory::Material; catDisplayName = "Материалы"; catFound = true;
        } else if (catStr == "artifact" || catStr == "artifacts" || catStr == "артефакт" || catStr == "артефакты") {
            targetCategory = ItemCategory::Artifact; catDisplayName = "Артефакты"; catFound = true;
        } else if (catStr == "jewelry" || catStr == "драгоценности" || catStr == "драгоценность") {
            targetCategory = ItemCategory::Jewelry; catDisplayName = "Драгоценности"; catFound = true;
        } else if (catStr == "quest" || catStr == "квест" || catStr == "квестовые") {
            targetCategory = ItemCategory::Quest; catDisplayName = "Квестовые предметы"; catFound = true;
        }

        if (!catFound) {
            logConsole("Ошибка: неизвестная категория '" + tokens[1] + "'!", ImVec4(1.0f, 0.35f, 0.35f, 1.0f));
            logConsole("Допустимые категории: character, food, weapon, material, artifact, jewelry, quest", ImVec4(1.0f, 0.75f, 0.3f, 1.0f));
            return;
        }

        std::string itemId = tokens[2];
        int count = 1;

        if (tokens.size() >= 4) {
            try {
                count = std::stoi(tokens[3]);
            } catch (...) {
                logConsole("Ошибка: количество '" + tokens[3] + "' не является целым числом!", ImVec4(1.0f, 0.35f, 0.35f, 1.0f));
                return;
            }
        }

        // Логичные ограничения на количество
        if (count <= 0) {
            logConsole("Ограничение: количество должно быть положительным числом (> 0)!", ImVec4(1.0f, 0.5f, 0.2f, 1.0f));
            return;
        }
        if (count > 99999) {
            logConsole("Ограничение: количество не может превышать 99999 за раз!", ImVec4(1.0f, 0.5f, 0.2f, 1.0f));
            return;
        }

        auto& dbItems = GameDatabase::getInstance().items;
        auto it = dbItems.find(itemId);
        if (it == dbItems.end()) {
            logConsole("Ошибка: предмет с ID '" + itemId + "' не найден в базе данных!", ImVec4(1.0f, 0.35f, 0.35f, 1.0f));
            return;
        }

        // Логичное ограничение: проверка соответствия категории
        if (it->second->getCategory() != targetCategory) {
            logConsole("Ограничение: несовпадение категорий! Предмет '" + it->second->getName() + 
                       "' (ID: " + itemId + ") принадлежит категории '" + 
                       getItemCategoryDisplayName(it->second->getCategory()) + 
                       "', а не '" + catDisplayName + "'.", ImVec4(1.0f, 0.6f, 0.2f, 1.0f));
            return;
        }

        playerInventory.addItem(itemId, count);
        logConsole("Успешно: выдано " + std::to_string(count) + " шт. '" + it->second->getName() + 
                   "' (ID: " + itemId + ") в инвентарь (" + catDisplayName + ").", ImVec4(0.3f, 1.0f, 0.4f, 1.0f));
        return;
    }

    // Команда: remove
    if (cmd == "remove" || cmd == "удалить") {
        if (tokens.size() < 3) {
            logConsole("Ошибка: недостаточно аргументов!", ImVec4(1.0f, 0.35f, 0.35f, 1.0f));
            logConsole("Синтаксис: remove <категория> <id> [количество]", ImVec4(1.0f, 0.75f, 0.3f, 1.0f));
            logConsole("Для персонажей: remove character <id>", ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
            return;
        }

        std::string catStr = toLowerCmdStr(tokens[1]);

        // --- КАТЕГОРИЯ: ПЕРСОНАЖИ ---
        if (catStr == "character" || catStr == "char" || catStr == "hero" || 
            catStr == "персонаж" || catStr == "герой") {

            // Логичное ограничение: для персонажей количество не указывается
            if (tokens.size() > 3) {
                logConsole("Ограничение: для персонажей нельзя указывать количество!", ImVec4(1.0f, 0.5f, 0.2f, 1.0f));
                return;
            }

            std::string charId = toLowerCmdStr(tokens[2]);

            // Логичное ограничение: у игрока должен быть этот персонаж
            if (!playerProfile.hasCharacter(charId)) {
                logConsole("Ограничение: у вас нет персонажа с ID '" + charId + "'!", ImVec4(1.0f, 0.5f, 0.2f, 1.0f));
                return;
            }

            // Логичное ограничение: нельзя удалить последнего оставшегося персонажа
            if (playerProfile.getCharacterCount() <= 1) {
                logConsole("Ограничение: нельзя удалить последнего оставшегося персонажа в отряде!", ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
                return;
            }

            std::string charName = charId;
            auto& dbChars = GameDatabase::getInstance().characters;
            if (dbChars.find(charId) != dbChars.end()) {
                charName = dbChars[charId]->getName();
            }

            playerProfile.removeCharacter(charId);
            tempSquad = playerProfile.getActiveSquad();
            logConsole("Успешно: персонаж '" + charName + "' (ID: " + charId + ") удален из отряда и резерва.", ImVec4(1.0f, 0.85f, 0.3f, 1.0f));
            return;
        }

        // --- КАТЕГОРИИ ПРЕДМЕТОВ ---
        ItemCategory targetCategory;
        std::string catDisplayName;
        bool catFound = false;

        if (catStr == "food" || catStr == "еда") {
            targetCategory = ItemCategory::Food; catDisplayName = "Еда"; catFound = true;
        } else if (catStr == "weapon" || catStr == "weapons" || catStr == "оружие") {
            targetCategory = ItemCategory::Weapon; catDisplayName = "Оружие"; catFound = true;
        } else if (catStr == "material" || catStr == "materials" || catStr == "материал" || catStr == "материалы") {
            targetCategory = ItemCategory::Material; catDisplayName = "Материалы"; catFound = true;
        } else if (catStr == "artifact" || catStr == "artifacts" || catStr == "артефакт" || catStr == "артефакты") {
            targetCategory = ItemCategory::Artifact; catDisplayName = "Артефакты"; catFound = true;
        } else if (catStr == "jewelry" || catStr == "драгоценности" || catStr == "драгоценность") {
            targetCategory = ItemCategory::Jewelry; catDisplayName = "Драгоценности"; catFound = true;
        } else if (catStr == "quest" || catStr == "квест" || catStr == "квестовые") {
            targetCategory = ItemCategory::Quest; catDisplayName = "Квестовые предметы"; catFound = true;
        }

        if (!catFound) {
            logConsole("Ошибка: неизвестная категория '" + tokens[1] + "'!", ImVec4(1.0f, 0.35f, 0.35f, 1.0f));
            logConsole("Допустимые категории: character, food, weapon, material, artifact, jewelry, quest", ImVec4(1.0f, 0.75f, 0.3f, 1.0f));
            return;
        }

        std::string itemId = tokens[2];
        int count = 1;

        if (tokens.size() >= 4) {
            try {
                count = std::stoi(tokens[3]);
            } catch (...) {
                logConsole("Ошибка: количество '" + tokens[3] + "' не является целым числом!", ImVec4(1.0f, 0.35f, 0.35f, 1.0f));
                return;
            }
        }

        if (count <= 0) {
            logConsole("Ограничение: количество должно быть больше нуля (> 0)!", ImVec4(1.0f, 0.5f, 0.2f, 1.0f));
            return;
        }

        // Логичное ограничение: проверка наличия предмета в инвентаре
        int availableCount = playerInventory.getItemCount(itemId);
        if (availableCount <= 0) {
            logConsole("Ограничение: предмета с ID '" + itemId + "' нет в вашем инвентаре!", ImVec4(1.0f, 0.5f, 0.2f, 1.0f));
            return;
        }

        // Логичное ограничение: проверка категории предмета
        auto& dbItems = GameDatabase::getInstance().items;
        if (dbItems.find(itemId) != dbItems.end()) {
            if (dbItems[itemId]->getCategory() != targetCategory) {
                logConsole(std::string("Ограничение: предмет принадлежит категории '") + 
                           getItemCategoryDisplayName(dbItems[itemId]->getCategory()) + 
                           "', а не '" + catDisplayName + "'.", ImVec4(1.0f, 0.6f, 0.2f, 1.0f));
                return;
            }
        }

        // Логичное ограничение: нельзя удалить больше, чем есть
        if (availableCount < count) {
            logConsole("Ограничение: недостаточно предметов! В наличии: " + 
                       std::to_string(availableCount) + " шт., запрошено к удалению: " + 
                       std::to_string(count) + " шт.", ImVec4(1.0f, 0.5f, 0.2f, 1.0f));
            return;
        }

        playerInventory.removeItem(itemId, count);
        std::string itemName = itemId;
        if (dbItems.find(itemId) != dbItems.end()) {
            itemName = dbItems[itemId]->getName();
        }
        logConsole("Успешно: удалено " + std::to_string(count) + " шт. '" + itemName + 
                   "' (ID: " + itemId + "). Осталось: " + std::to_string(availableCount - count) + " шт.", ImVec4(1.0f, 0.85f, 0.3f, 1.0f));
        return;
    }

    logConsole("Неизвестная команда '" + tokens[0] + "'. Введите 'help' для списка доступных команд.", ImVec4(1.0f, 0.5f, 0.3f, 1.0f));
}

void ScreenManager::renderConsole() {
    ImVec2 displaySize = ImGui::GetIO().DisplaySize;

    ImVec2 consoleSize = ImVec2(displaySize.x * 0.85f, 340.0f);
    ImVec2 consolePos = ImVec2((displaySize.x - consoleSize.x) * 0.5f, 20.0f);

    ImGui::SetNextWindowPos(consolePos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(consoleSize, ImGuiCond_Always);

    ImGuiWindowFlags consoleFlags = ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize;

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.06f, 0.06f, 0.09f, 0.98f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.70f, 0.55f, 0.25f, 1.0f));

    ImGui::Begin("##DevConsole", &isConsoleOpen, consoleFlags);
    ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.3f, 1.0f), "КОНСОЛЬ РАЗРАБОТЧИКА (Клавиша ~ для скрытия)");
    ImGui::SameLine(ImGui::GetWindowWidth() - 110.0f);
    if (ImGui::Button("Очистить##ClearConsole", ImVec2(95.0f, 0.0f))) {
        consoleHistory.clear();
    }
    ImGui::Separator();

    float footerHeight = ImGui::GetFrameHeightWithSpacing() + 8.0f;
    ImGui::BeginChild("ConsoleLog", ImVec2(0, -footerHeight), true, ImGuiWindowFlags_HorizontalScrollbar);
    for (const auto& entry : consoleHistory) {
        ImGui::TextColored(entry.color, "%s", entry.text.c_str());
    }
    if (scrollToBottomConsole) {
        ImGui::SetScrollHereY(1.0f);
        scrollToBottomConsole = false;
    }
    ImGui::EndChild();

    // Поле ввода команд
    ImGui::PushItemWidth(-1);
    bool enterPressed = ImGui::InputText("##ConsoleInput", consoleInputBuf, IM_ARRAYSIZE(consoleInputBuf),
        ImGuiInputTextFlags_EnterReturnsTrue);

    if (enterPressed) {
        if (consoleInputBuf[0] != '\0') {
            executeConsoleCommand(consoleInputBuf);
            consoleInputBuf[0] = '\0';
        }
        ImGui::SetKeyboardFocusHere(-1);
    }
    ImGui::PopItemWidth();

    ImGui::End();
    ImGui::PopStyleColor(2);
}

void ScreenManager::renderInventory() {
    std::string activeTabName = "";
    switch (currentInventoryTab) {
    case ItemTab::Weapons:    activeTabName = "Оружие"; break;
    case ItemTab::Artifacts:  activeTabName = "Артефакты"; break;
    case ItemTab::Food:       activeTabName = "Еда"; break;
    case ItemTab::Materials:  activeTabName = "Материалы"; break;
    case ItemTab::Jewelry:    activeTabName = "Драгоценности"; break;
    case ItemTab::QuestItems: activeTabName = "Квестовые предметы"; break;
    }

    ImGui::Text("=== ИНВЕНТАРЬ | Вкладка: %s ===", activeTabName.c_str());
    ImGui::SameLine(ImGui::GetWindowWidth() - 380.0f);
    ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.0f, 1.0f), "Баланс: %d монет", playerInventory.getBalance());
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.45f, 0.85f, 1.0f));
    if (ImGui::Button("<- В Лобби##InvBack", ImVec2(130.0f, 0.0f))) {
        currentScreen = ScreenState::Lobby;
    }
    ImGui::PopStyleColor();

    ImGui::Separator();
    ImGui::Spacing();

    ImVec2 iconSize = ImVec2(48.0f, 48.0f);
    struct TabButtonData { ItemTab tab; const char* strId; };
    TabButtonData inventoryTabs[] = {
        {ItemTab::Weapons,    "##InvWeapon"},
        {ItemTab::Artifacts,  "##InvArtifact"},
        {ItemTab::Food,       "##InvFood"},
        {ItemTab::Materials,  "##InvMaterials"},
        {ItemTab::Jewelry,    "##InvJewelry"},
        {ItemTab::QuestItems, "##InvQuest"}
    };

    for (int i = 0; i < 6; ++i) {
        if (i > 0) ImGui::SameLine();
        ItemTab tab = inventoryTabs[i].tab;
        ImTextureID textureId = (ImTextureID)(uintptr_t)tabTextures[tab].getNativeHandle();

        bool isActive = (currentInventoryTab == tab);
        if (isActive) ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.5f, 0.8f, 1.0f));

        if (ImGui::ImageButton(inventoryTabs[i].strId, textureId, iconSize)) {
            currentInventoryTab = tab;
        }

        if (isActive) ImGui::PopStyleColor();
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

        ItemCategory selectedCategory = tabToCategory(currentInventoryTab);
    auto itemsList = playerInventory.getItemsByCategory(selectedCategory);
    auto& dbItems = GameDatabase::getInstance().items;

    if (itemsList.empty()) {
        ImGui::Spacing();
        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "В этой категории пока нет предметов.");
    }
    else {
        float availW = ImGui::GetContentRegionAvail().x;
        float cardMinW = 270.0f;
        int columns = std::max(1, static_cast<int>(availW / cardMinW));

        ImGui::Columns(columns, "InventoryFullGrid", false);

        for (size_t i = 0; i < itemsList.size(); ++i) {
            const auto& stack = itemsList[i];
            auto itItem = dbItems.find(stack.itemId);
            if (itItem == dbItems.end()) continue;
            const auto& itemData = itItem->second;

            ImGui::PushID(static_cast<int>(i));

            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.08f, 0.10f, 0.15f, 0.85f));
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.35f, 0.40f, 0.50f, 0.75f));

            ImGui::BeginChild("InvCard", ImVec2(0.0f, 88.0f), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

            // Иконка предмета слева
            ImGui::Image(itemData->getImGuiTextureID(), ImVec2(64.0f, 64.0f));
            ImGui::SameLine(0.0f, 12.0f);

            // Описание предмета справа
            ImGui::BeginGroup();
            ImGui::TextColored(ImVec4(0.95f, 0.90f, 0.60f, 1.0f), "%s", itemData->getName().c_str());
            ImGui::TextColored(ImVec4(0.40f, 0.85f, 1.0f, 1.0f), "Кол-во: %d шт.", stack.count);
            ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.30f, 1.0f), "Цена продажи: %d м", itemData->getSellPrice());
            ImGui::EndGroup();

            ImGui::EndChild();
            ImGui::PopStyleColor(2);

            ImGui::PopID();
            ImGui::NextColumn();
        }
        ImGui::Columns(1);
    }
}

// =============================================================================
// ЭКРАН МАГАЗИНА
// =============================================================================
void ScreenManager::renderShop() {
    std::string activeTabName = "";
    switch (currentShopTab) {
    case ItemTab::Weapons:   activeTabName = "Оружие"; break;
    case ItemTab::Artifacts: activeTabName = "Артефакты"; break;
    case ItemTab::Food:      activeTabName = "Еда"; break;
    case ItemTab::Materials: activeTabName = "Материалы"; break;
    case ItemTab::Jewelry:   activeTabName = "Драгоценности"; break;
    default: break;
    }

    ImGui::Text("=== ТОРГОВЛЯ: %s | Категория: %s ===", currentMerchantName.c_str(), activeTabName.c_str());
    ImGui::SameLine(ImGui::GetWindowWidth() - 220.0f);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.45f, 0.85f, 1.0f));
    if (ImGui::Button("<- На карту##ShopBack", ImVec2(190.0f, 30.0f))) {
        currentScreen = ScreenState::Map;
    }
    ImGui::PopStyleColor();

    ImGui::Separator();
    ImGui::Spacing();

    ImVec2 iconSize = ImVec2(48.0f, 48.0f);
    struct TabButtonData { ItemTab tab; const char* strId; };
    TabButtonData shopTabs[] = {
        {ItemTab::Weapons,   "##ShopWeapon"},
        {ItemTab::Artifacts, "##ShopArtifact"},
        {ItemTab::Food,      "##ShopFood"},
        {ItemTab::Materials, "##ShopMaterials"},
        {ItemTab::Jewelry,   "##ShopJewelry"}
    };

    for (int i = 0; i < 5; ++i) {
        if (i > 0) ImGui::SameLine();
        ItemTab tab = shopTabs[i].tab;
        ImTextureID textureId = (ImTextureID)(uintptr_t)tabTextures[tab].getNativeHandle();

        bool isActive = (currentShopTab == tab);
        if (isActive) ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.5f, 0.2f, 1.0f));

        if (ImGui::ImageButton(shopTabs[i].strId, textureId, iconSize)) {
            currentShopTab = tab;
        }

        if (isActive) ImGui::PopStyleColor();
    }

    ImGui::Spacing();
    ImGui::Separator();

    ItemCategory activeCategory = tabToCategory(currentShopTab);
    auto& dbItems = GameDatabase::getInstance().items;

    ImGui::Columns(2, "ShopTradeColumns", true);

    // ЛЕВАЯ ЧАСТЬ: ИНВЕНТАРЬ ТОРГОВЦА
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "Инвентарь торговца (%s)", currentMerchantName.c_str());
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.0f, 1.0f), "| Баланс: %d монет", activeMerchantInventory->getBalance());
    ImGui::TextDisabled("Нажмите на предмет, чтобы КУПИТЬ его");
    ImGui::Separator();
    ImGui::Spacing();

    auto merchantItems = activeMerchantInventory->getItemsByCategory(activeCategory);
    if (merchantItems.empty()) {
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "У торговца нет товаров в этой категории.");
    }
    else {
        for (size_t i = 0; i < merchantItems.size(); ++i) {
            const auto& stack = merchantItems[i];
            if (dbItems.find(stack.itemId) == dbItems.end()) continue;
            const auto& itemData = dbItems[stack.itemId];

            int buyPrice = itemData->getBuyPrice();
            bool canPlayerAfford = (playerInventory.getBalance() >= buyPrice);

            ImGui::PushID(1000 + static_cast<int>(i));

            if (!canPlayerAfford) {
                ImGui::BeginDisabled();
            }

            if (ImGui::ImageButton("##BuyItemBtn", itemData->getImGuiTextureID(), ImVec2(56, 56))) {
                if (playerInventory.spendFunds(buyPrice)) {
                    activeMerchantInventory->addFunds(buyPrice);
                    activeMerchantInventory->removeItem(stack.itemId, 1);
                    playerInventory.addItem(stack.itemId, 1);
                }
            }

            if (!canPlayerAfford) {
                ImGui::EndDisabled();
            }

            ImGui::SameLine();
            ImGui::BeginGroup();
            ImGui::Text("%s (x%d)", itemData->getName().c_str(), stack.count);
            if (canPlayerAfford) {
                ImGui::TextColored(ImVec4(0.2f, 0.9f, 0.2f, 1.0f), "Цена покупки: %d монет", buyPrice);
            }
            else {
                ImGui::TextColored(ImVec4(0.9f, 0.3f, 0.3f, 1.0f), "Цена покупки: %d монет (Не хватает)", buyPrice);
            }
            ImGui::EndGroup();

            ImGui::PopID();
            ImGui::Spacing();
        }
    }

    ImGui::NextColumn();

    // ПРАВАЯ ЧАСТЬ: ИНВЕНТАРЬ ИГРОКА
    ImGui::TextColored(ImVec4(0.3f, 0.95f, 0.5f, 1.0f), "Инвентарь игрока");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.0f, 1.0f), "| Баланс: %d монет", playerInventory.getBalance());
    ImGui::TextDisabled("Нажмите на предмет, чтобы ПРОДАТЬ его торговцу");
    ImGui::Separator();
    ImGui::Spacing();

    auto playerItems = playerInventory.getItemsByCategory(activeCategory);
    if (playerItems.empty()) {
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "В вашем инвентаре пусто в этой категории.");
    }
    else {
        for (size_t i = 0; i < playerItems.size(); ++i) {
            const auto& stack = playerItems[i];
            if (dbItems.find(stack.itemId) == dbItems.end()) continue;
            const auto& itemData = dbItems[stack.itemId];

            int sellPrice = itemData->getSellPrice();
            bool canMerchantAfford = (activeMerchantInventory->getBalance() >= sellPrice);

            ImGui::PushID(2000 + static_cast<int>(i));

            if (!canMerchantAfford) {
                ImGui::BeginDisabled();
            }

            if (ImGui::ImageButton("##SellItemBtn", itemData->getImGuiTextureID(), ImVec2(56, 56))) {
                if (activeMerchantInventory->spendFunds(sellPrice)) {
                    playerInventory.addFunds(sellPrice);
                    playerInventory.removeItem(stack.itemId, 1);
                    activeMerchantInventory->addItem(stack.itemId, 1);
                }
            }

            if (!canMerchantAfford) {
                ImGui::EndDisabled();
            }

            ImGui::SameLine();
            ImGui::BeginGroup();
            ImGui::Text("%s (x%d)", itemData->getName().c_str(), stack.count);
            if (canMerchantAfford) {
                ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.0f, 1.0f), "Цена продажи: %d монет", sellPrice);
            }
            else {
                ImGui::TextColored(ImVec4(0.9f, 0.3f, 0.3f, 1.0f), "Цена продажи: %d монет (У торговца нет монет)", sellPrice);
            }
            ImGui::EndGroup();

            ImGui::PopID();
            ImGui::Spacing();
        }
    }

    ImGui::Columns(1);
}

// =============================================================================
// ГЛАВНОЕ МЕНЮ
// =============================================================================
void ScreenManager::renderMainMenu() {
    ImVec2 displaySize = ImGui::GetIO().DisplaySize;
    float spacing = 20.0f;
    int buttonCount = 4;

    if (mainMenuFont) {
        ImGui::PushFont(mainMenuFont);
    }

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));

    float textHeight = ImGui::GetFontSize();
    float totalHeight = (textHeight * buttonCount) + (spacing * (buttonCount - 1));
    float startY = (displaySize.y - totalHeight) * 0.5f;

    ImGui::SetCursorPosY(startY);

    struct MenuBtn { const char* label; ScreenState target; bool isQuit; };
    MenuBtn buttons[] = {
        {"Начать игру", ScreenState::Lobby, false},
        {"Загрузить игру", ScreenState::Saves, false},
        {"Настройки", ScreenState::Settings, false},
        {"Выход", ScreenState::MainMenu, true}
    };

    for (int i = 0; i < 4; ++i) {
        float textWidth = ImGui::CalcTextSize(buttons[i].label).x;
        ImGui::SetCursorPosX((displaySize.x - textWidth) * 0.5f);

        bool isHovered = ImGui::IsMouseHoveringRect(ImGui::GetCursorScreenPos(),
            ImVec2(ImGui::GetCursorScreenPos().x + textWidth, ImGui::GetCursorScreenPos().y + textHeight));

        if (isHovered) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.2f, 1.0f));
        }
        else {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        }

        if (ImGui::Button(buttons[i].label)) {
            if (buttons[i].isQuit) shouldQuit = true;
            else currentScreen = buttons[i].target;
        }

        ImGui::PopStyleColor();
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + spacing);
    }

    ImGui::PopStyleColor(3);

    if (mainMenuFont) {
        ImGui::PopFont();
    }
}

// =============================================================================
// ЛОББИ (ХАБ)
// =============================================================================
void ScreenManager::renderLobby() {
    ImVec2 displaySize = ImGui::GetIO().DisplaySize;

    // Стилизованный заголовок по центру сверху
    ImGui::Spacing();
    const char* title = "=== СВЯТИЛИЩЕ ГЕРОЕВ - ЛОББИ ===";
    float titleWidth = ImGui::CalcTextSize(title).x;
    ImGui::SetCursorPosX((displaySize.x - titleWidth) * 0.5f);
    ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.35f, 1.0f), "%s", title);

    const char* subtitle = "Выберите зону для подготовки и путешествия по этажам башни";
    float subWidth = ImGui::CalcTextSize(subtitle).x;
    ImGui::SetCursorPosX((displaySize.x - subWidth) * 0.5f);
    ImGui::TextColored(ImVec4(0.70f, 0.70f, 0.75f, 1.0f), "%s", subtitle);

    ImGui::Separator();
    ImGui::Spacing();

    struct NavTarget {
        const char* label;
        const char* desc;
        ScreenState state;
        ImVec4 accentColor;
    };

    NavTarget targets[] = {
        {"Карта этажей",           "Исследование 10 этажей, битвы, торговцы и святилища", ScreenState::Map,             ImVec4(0.20f, 0.50f, 0.80f, 1.0f)},
        {"Редактор отряда",       "Формирование ударной группы из 3 боевых героев отряда",  ScreenState::SquadEditor,     ImVec4(0.25f, 0.65f, 0.35f, 1.0f)},
        {"Герои и экипировка",    "Смена экипировки, изучение артефактов и сетов", ScreenState::CharacterEditor, ImVec4(0.65f, 0.40f, 0.80f, 1.0f)},
        {"Инвентарь и припасы",  "Снаряжение, драгоценные металлы, минералы и еда", ScreenState::Inventory,       ImVec4(0.85f, 0.60f, 0.20f, 1.0f)},
        {"Зал достижений",        "Список наград, подвигов и бонусов за испытания",     ScreenState::Achievements,    ImVec4(0.90f, 0.75f, 0.25f, 1.0f)},
        {"Задания и квесты",      "Журнал поручений, охота на монстров и награды",       ScreenState::Quests,          ImVec4(0.30f, 0.70f, 0.85f, 1.0f)}
    };

    int count = 6;
    float cardWidth = 320.0f;
    float cardHeight = 72.0f;
    float spacingX = 30.0f;
    float spacingY = 16.0f;
    int cols = 2;
    int rows = 3;

    float totalGridW = (cardWidth * cols) + spacingX;
    float totalGridH = (cardHeight * rows) + (spacingY * (rows - 1));

    float startX = (displaySize.x - totalGridW) * 0.5f;
    float startY = (displaySize.y - totalGridH) * 0.5f - 15.0f;

    for (int i = 0; i < count; ++i) {
        int row = i / cols;
        int col = i % cols;

        float posX = startX + col * (cardWidth + spacingX);
        float posY = startY + row * (cardHeight + spacingY);

        ImGui::SetCursorPos(ImVec2(posX, posY));
        ImGui::PushID(i);

        ImGui::PushStyleColor(ImGuiCol_Border, targets[i].accentColor);
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.09f, 0.10f, 0.15f, 0.82f));
        ImGui::BeginChild("LobbyNavCard", ImVec2(cardWidth, cardHeight), true);

        if (ImGui::Button(targets[i].label, ImVec2(-1, 30.0f))) {
            currentScreen = targets[i].state;
        }
        ImGui::TextColored(ImVec4(0.70f, 0.70f, 0.75f, 1.0f), " %s", targets[i].desc);

        ImGui::EndChild();
        ImGui::PopStyleColor(2);

        ImGui::PopID();
    }

    // Кнопка возврата в главное меню — внизу по центру
    ImVec2 backBtnSize = ImVec2(240.0f, 38.0f);
    ImGui::SetCursorPos(ImVec2((displaySize.x - backBtnSize.x) * 0.5f, displaySize.y - backBtnSize.y - 25.0f));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.70f, 0.18f, 0.18f, 0.95f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.85f, 0.25f, 0.25f, 1.0f));
    if (ImGui::Button("<- Главное меню##LobbyBack", backBtnSize)) {
        currentScreen = ScreenState::MainMenu;
    }
    ImGui::PopStyleColor(2);
}

void ScreenManager::renderCooking() {
    ImGui::Text("=== ПОЛЕВАЯ КУХНЯ ===");
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.45f, 0.85f, 1.0f));
    if (ImGui::Button("<- На карту##CookBack", ImVec2(150, 40))) {
        clearCookingGrid();
        currentScreen = ScreenState::Map;
    }
    ImGui::PopStyleColor();

    ImGui::Spacing();
    ImGui::Separator();

    ImGui::Columns(2, "CookingColumns", false);
    ImGui::SetColumnWidth(0, ImGui::GetWindowWidth() * 0.4f);

    // --- ЛЕВАЯ КОЛОНКА: Доступные ингредиенты ---
    ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Ваши ингредиенты:");
    ImGui::Spacing();

    auto foodItems = playerInventory.getItemsByCategory(ItemCategory::Food);

    std::unordered_map<std::string, int> gridCounts;
    for (const auto& id : cookingGrid) {
        if (!id.empty()) gridCounts[id]++;
    }

    for (size_t i = 0; i < foodItems.size(); ++i) {
        const auto& stack = foodItems[i];
        int availableToUse = stack.count - gridCounts[stack.itemId];

        if (availableToUse > 0) {
            const auto& itemData = GameDatabase::getInstance().items[stack.itemId];

            ImGui::PushID(static_cast<int>(i));

            if (ImGui::ImageButton(itemData->getImGuiTextureID(), ImVec2(48, 48))) {
                for (int slotIndex = 0; slotIndex < 9; ++slotIndex) {
                    if (cookingGrid[slotIndex].empty()) {
                        cookingGrid[slotIndex] = stack.itemId;
                        break;
                    }
                }
            }

            ImGui::SameLine();
            ImGui::Text("%s\nДоступно: %d", itemData->getName().c_str(), availableToUse);
            ImGui::PopID();
        }
    }

    ImGui::NextColumn();

    // --- ПРАВАЯ КОЛОНКА: Сетка 3x3 и Результат ---
    ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Поле рецепта (3x3):");
    ImGui::Spacing();

    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 3; ++col) {
            int index = row * 3 + col;

            ImGui::PushID(100 + index);

            if (cookingGrid[index].empty()) {
                ImGui::Button("##EmptySlot", ImVec2(64, 64));
            }
            else {
                std::string itemId = cookingGrid[index];
                const auto& itemData = GameDatabase::getInstance().items[itemId];

                if (ImGui::ImageButton(itemData->getImGuiTextureID(), ImVec2(64, 64))) {
                    cookingGrid[index] = "";
                }
            }

            if (col < 2) ImGui::SameLine();

            ImGui::PopID();
        }
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Результат:");
    ImGui::Spacing();

    std::vector<std::string> currentIngredients;
    for (const auto& id : cookingGrid) {
        if (!id.empty()) currentIngredients.push_back(id);
    }

    std::sort(currentIngredients.begin(), currentIngredients.end());

    struct SimpleRecipe {
        std::string resultId;
        std::vector<std::string> requiredIngredients;
    };

    static std::vector<SimpleRecipe> recipes = {
        {"baked_apple", {"greenhouse_apple", "goat_milk"}},
        {"baked_apple", {"greenhouse_apple", "cave_wheat"}},
        {"baked_apple", {"wild_berries", "goat_milk"}},
        {"hearty_mushroom_soup", {"rusty_mushroom", "dirty_water", "glowing_moss"}},
        {"hearty_mushroom_soup", {"rusty_mushroom", "wild_berries", "dirty_water"}},
        {"hearty_mushroom_soup", {"shell_meat", "ocean_salt", "dirty_water"}},
        {"meat_stew", {"rat_meat", "dirty_water", "cave_wheat"}},
        {"meat_stew", {"fresh_game", "cave_wheat", "dirty_water"}},
        {"meat_stew", {"gutted_fish", "ocean_salt", "raw_seaweed"}},
        {"meat_stew", {"salamander_meat", "fire_grain", "thermal_water"}},
        {"meat_stew", {"fresh_game", "ocean_salt", "cave_wheat"}}
    };

    std::string matchedResult = "";
    for (auto& recipe : recipes) {
        std::vector<std::string> req = recipe.requiredIngredients;
        std::sort(req.begin(), req.end());

        if (req == currentIngredients) {
            matchedResult = recipe.resultId;
            break;
        }
    }

    if (!matchedResult.empty()) {
        const auto& resultData = GameDatabase::getInstance().items[matchedResult];

        if (ImGui::ImageButton(resultData->getImGuiTextureID(), ImVec2(80, 80))) {
            playerInventory.addItem(matchedResult, 1);

            for (const auto& id : currentIngredients) {
                playerInventory.removeItem(id, 1);
            }

            clearCookingGrid();
        }
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.2f, 0.9f, 0.2f, 1.0f), "Готово: %s!\nКликните по иконке, чтобы забрать.", resultData->getName().c_str());
    }
    else {
        ImGui::BeginDisabled();
        ImGui::Button("?", ImVec2(80, 80));
        ImGui::EndDisabled();
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Скомбинируйте ингредиенты,\nчтобы получить блюдо.");
    }

    ImGui::Columns(1);
}

// =============================================================================
// НАСТРОЙКИ
// =============================================================================
void ScreenManager::renderSettings() {
    ImVec2 displaySize = ImGui::GetIO().DisplaySize;

    float panelWidth = 720.0f;
    float startX = (displaySize.x - panelWidth) * 0.5f;
    if (startX < 20.0f) startX = 20.0f;

    ImGui::SetCursorPosX(startX);
    ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.3f, 1.0f), "=== НАСТРОЙКИ ИГРЫ ===");
    ImGui::SameLine(ImGui::GetWindowWidth() - 200.0f);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.80f, 0.15f, 0.15f, 1.0f));
    if (ImGui::Button("<- Назад##SettingsBack", ImVec2(170.0f, 0.0f))) {
        remappingActionIndex = -1;
        currentScreen = ScreenState::MainMenu;
    }
    ImGui::PopStyleColor();

    ImGui::Separator();
    ImGui::Spacing();

    ImGui::SetCursorPosX(startX);
    float panelHeight = displaySize.y - ImGui::GetCursorPosY() - 40.0f;
    if (panelHeight < 360.0f) panelHeight = 360.0f;

    ImGui::BeginChild("SettingsContainer", ImVec2(panelWidth, panelHeight), true);

    if (ImGui::BeginTabBar("SettingsTabs", ImGuiTabBarFlags_None)) {
        // =========================================================================
        // ВКЛАДКА 1: ОБЩИЕ НАСТРОЙКИ
        // =========================================================================
        if (ImGui::BeginTabItem("  Общие настройки  ")) {
            ImGui::Spacing();

            // 1. ОКОННЫЙ РЕЖИМ (Оконный режим убран во избежание сбоев интерфейса)
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.85f, 0.3f, 1.0f));
            ImGui::Text("РЕЖИМ ОТОБРАЖЕНИЯ: ПОЛНОЭКРАННЫЙ");
            ImGui::PopStyleColor();
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::TextColored(ImVec4(0.35f, 0.85f, 0.45f, 1.0f), "[ ПОЛНОЭКРАННЫЙ РЕЖИМ АКТИВЕН ]");
            ImGui::TextDisabled("Оконный режим отключен для гарантированного сохранения пропорций игрового интерфейса.");
            ImGui::TextColored(ImVec4(0.70f, 0.70f, 0.75f, 1.0f),
                "Текущее разрешение экрана: %.0f x %.0f", displaySize.x, displaySize.y);

            ImGui::Spacing();
            ImGui::Spacing();

            // 2. ГРОМКОСТЬ
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.85f, 0.3f, 1.0f));
            ImGui::Text("НАСТРОЙКИ ЗВУКА");
            ImGui::PopStyleColor();
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::Text("Громкость музыки:");
            ImGui::SetNextItemWidth(360.0f);
            ImGui::SliderFloat("##MusicVolumeSlider", &musicVolume, 0.0f, 100.0f, "%.0f %%");

            ImGui::Spacing();

            ImGui::Text("Громкость эффектов:");
            ImGui::SetNextItemWidth(360.0f);
            ImGui::SliderFloat("##SfxVolumeSlider", &sfxVolume, 0.0f, 100.0f, "%.0f %%");

            ImGui::Spacing();
            if (ImGui::Button("Сбросить звук (100%)", ImVec2(190.0f, 28.0f))) {
                musicVolume = 100.0f;
                sfxVolume = 100.0f;
            }
            ImGui::SameLine();
            if (ImGui::Button("Выключить звук (0%)", ImVec2(180.0f, 28.0f))) {
                musicVolume = 0.0f;
                sfxVolume = 0.0f;
            }

            ImGui::Spacing();
            ImGui::Spacing();

            // 3. ВЫБОР ЯЗЫКА
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.85f, 0.3f, 1.0f));
            ImGui::Text("ЯЗЫК ИНТЕРФЕЙСА");
            ImGui::PopStyleColor();
            ImGui::Separator();
            ImGui::Spacing();

            const char* languages[] = {
                "Русский (Russian)",
                "English (Английский)"
            };

            int langIndex = (currentLanguage == GameLanguage::Russian) ? 0 : 1;
            ImGui::Text("Выберите язык:");
            ImGui::SetNextItemWidth(320.0f);
            if (ImGui::Combo("##LanguageCombo", &langIndex, languages, IM_ARRAYSIZE(languages))) {
                currentLanguage = (langIndex == 0) ? GameLanguage::Russian : GameLanguage::English;
            }

            ImGui::Spacing();
            ImGui::TextColored(ImVec4(0.65f, 0.65f, 0.65f, 1.0f),
                "Выбранный язык: %s [код: %s]", getLanguageName(), getLanguageCode());

            ImGui::EndTabItem();
        }

        // =========================================================================
        // ВКЛАДКА 2: УПРАВЛЕНИЕ (ПЕРЕНАЗНАЧЕНИЕ КЛАВИШ)
        // =========================================================================
        if (ImGui::BeginTabItem("  Управление  ")) {
            ImGui::Spacing();
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.85f, 0.3f, 1.0f));
            ImGui::Text("ГОРЯЧИЕ КЛАВИШИ В БОЮ");
            ImGui::PopStyleColor();
            ImGui::TextColored(ImVec4(0.70f, 0.70f, 0.75f, 1.0f),
                "Нажмите на кнопку с клавишей для переназначения. Для отмены нажмите Esc.");
            ImGui::Separator();
            ImGui::Spacing();

            if (remappingActionIndex >= 0) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.85f, 0.2f, 1.0f));
                ImGui::Text(">> Ожидание нажатия новой клавиши... (Esc для отмены)");
                ImGui::PopStyleColor();
                ImGui::Spacing();
            }

            struct ActionRow {
                CombatAction action;
                const char* title;
                const char* desc;
            };

            ActionRow rows[] = {
                { CombatAction::SelectHero1, "Сменить на бойца 1", "Переключение на слот первого бойца отряда" },
                { CombatAction::SelectHero2, "Сменить на бойца 2", "Переключение на слот второго бойца отряда" },
                { CombatAction::SelectHero3, "Сменить на бойца 3", "Переключение на слот третьего бойца отряда" },
                { CombatAction::BasicAttack,  "Обычная атака",     "Базовый удар без перезарядки (Навык 1)" },
                { CombatAction::Skill,        "Навык",             "Особая тактическая способность (Навык 2)" },
                { CombatAction::Ultimate,     "Ультимейт (Ульта)", "Завершающее сильнейшее умение (Навык 3)" }
            };

            ImGui::Columns(3, "KeybindingsTable", true);
            ImGui::SetColumnWidth(0, 210.0f);
            ImGui::SetColumnWidth(1, 160.0f);
            ImGui::SetColumnWidth(2, 310.0f);

            ImGui::TextColored(ImVec4(0.9f, 0.85f, 0.4f, 1.0f), "Действие");
            ImGui::NextColumn();
            ImGui::TextColored(ImVec4(0.9f, 0.85f, 0.4f, 1.0f), "Клавиша");
            ImGui::NextColumn();
            ImGui::TextColored(ImVec4(0.9f, 0.85f, 0.4f, 1.0f), "Описание");
            ImGui::NextColumn();
            ImGui::Separator();

            for (const auto& r : rows) {
                ImGui::PushID(static_cast<int>(r.action));

                ImGui::AlignTextToFramePadding();
                ImGui::Text("%s", r.title);
                ImGui::NextColumn();

                sf::Keyboard::Key curKey = combatKeybinds.getKey(r.action);
                std::string keyLabel;
                bool isThisRemapping = (remappingActionIndex == static_cast<int>(r.action));

                if (isThisRemapping) {
                    keyLabel = "[ Нажмите... ]";
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.85f, 0.55f, 0.15f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
                } else {
                    keyLabel = "[  " + getKeyDisplayName(curKey) + "  ]";
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.20f, 0.24f, 0.32f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.9f, 0.5f, 1.0f));
                }

                if (ImGui::Button(keyLabel.c_str(), ImVec2(140.0f, 26.0f))) {
                    if (isThisRemapping) {
                        remappingActionIndex = -1;
                    } else {
                        remappingActionIndex = static_cast<int>(r.action);
                    }
                }
                ImGui::PopStyleColor(2);

                ImGui::NextColumn();
                ImGui::AlignTextToFramePadding();
                ImGui::TextDisabled("%s", r.desc);
                ImGui::NextColumn();

                ImGui::PopID();
            }

            ImGui::Columns(1);
            ImGui::Separator();
            ImGui::Spacing();

            if (ImGui::Button("Сбросить управление по умолчанию", ImVec2(280.0f, 32.0f))) {
                combatKeybinds.resetToDefaults();
                remappingActionIndex = -1;
            }
            ImGui::SameLine();
            ImGui::TextDisabled("(По умолчанию: 1-3 герои, E атака, W навык, Q ульта)");

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::EndChild();
}

void ScreenManager::renderSaves() { 
    ImGui::Text("=== ЭКРАН СОХРАНЕНИЙ ==="); 
    ImGui::SameLine(ImGui::GetWindowWidth() - 200.0f);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.80f, 0.15f, 0.15f, 1.0f));
    if (ImGui::Button("<- В меню##SavesBack", ImVec2(170.0f, 0.0f))) {
        currentScreen = ScreenState::MainMenu;
    }
    ImGui::PopStyleColor();
    ImGui::Separator(); 
    ImGui::Spacing();

    ImGui::TextColored(ImVec4(0.9f, 0.8f, 0.4f, 1.0f), "Доступные сохранения:");
    ImGui::Spacing();

    for (int slot = 1; slot <= 3; ++slot) {
        ImGui::PushID(slot);
        ImGui::BeginGroup();
        if (slot == 1) {
            ImGui::Text("Слот %d: Автосохранение (Этаж 1 - Заброшенный лагерь, 250 монет)", slot);
        } else if (slot == 2) {
            ImGui::Text("Слот %d: Сохранение у Оазиса (Этаж 2 - Оазис, 600 монет)", slot);
        } else {
            ImGui::Text("Слот %d: [Пустой слот]", slot);
        }

        ImGui::SameLine(ImGui::GetWindowWidth() - 220.0f);
        if (slot <= 2) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.20f, 0.65f, 0.30f, 1.0f));
            if (ImGui::Button("Загрузить##SlotLoad", ImVec2(180.0f, 28.0f))) {
                currentFloor = slot;
                currentScreen = ScreenState::Lobby;
            }
            ImGui::PopStyleColor();
        } else {
            ImGui::BeginDisabled();
            ImGui::Button("Пусто##SlotEmpty", ImVec2(180.0f, 28.0f));
            ImGui::EndDisabled();
        }
        ImGui::EndGroup();
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::PopID();
    }
}

// =============================================================================
// КАРТА
// =============================================================================
void ScreenManager::renderMap() {
    ImVec2 displaySize = ImGui::GetIO().DisplaySize;
    bool isPopupOpen = (selectedMapPointIndex != -1);

    if (isPopupOpen) {
        ImGui::BeginDisabled(true);
    }

    // --- Верхняя панель: Заголовок текущего этажа и кнопка в Лобби ---
    ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.35f, 1.0f), "=== %s ===", getFloorTitle(currentFloor).c_str());
    ImGui::SameLine(ImGui::GetWindowWidth() - 210.0f);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.18f, 0.45f, 0.85f, 0.95f));
    if (ImGui::Button("<- В Лобби##MapBack", ImVec2(180.0f, 28.0f))) {
        currentScreen = ScreenState::Lobby;
    }
    ImGui::PopStyleColor();

    ImGui::Separator();
    ImGui::Spacing();

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 winPos = ImGui::GetWindowPos();

    // РАСПРОСТРАНЕНИЕ ТОЧЕК НА ВСЮ КАРТУ ПОЛНОЭКРАННОГО РЕЖИМА
    float mapAreaLeft = 90.0f;
    float mapAreaRight = displaySize.x - 170.0f; // Оставляем место под переключатель этажей
    float mapAreaTop = 110.0f;
    float mapAreaBottom = displaySize.y - 70.0f;

    // Собираем центры всех активных точек для отрисовки путей
    std::vector<ImVec2> floorNodeCenters;
    for (size_t i = 0; i < mapPoints.size(); ++i) {
        auto& pt = mapPoints[i];
        if (pt.floor == currentFloor && !pt.isCompleted) {
            float normX = std::clamp((pt.position.x - 180.0f) / 720.0f, 0.0f, 1.0f);
            float normY = std::clamp((pt.position.y - 180.0f) / 380.0f, 0.0f, 1.0f);
            float screenX = mapAreaLeft + normX * (mapAreaRight - mapAreaLeft);
            float screenY = mapAreaTop + normY * (mapAreaBottom - mapAreaTop);
            floorNodeCenters.push_back(ImVec2(winPos.x + screenX, winPos.y + screenY));
        }
    }

    // Линии связующих дорожек удалены по требованию пользователя

    // Отрисовка интерактивных точек
    for (size_t i = 0; i < mapPoints.size(); ++i) {
        auto& point = mapPoints[i];

        if (point.floor != currentFloor || point.isCompleted) {
            continue;
        }

        ImGui::PushID(static_cast<int>(i));

        float r = point.radius;
        // Масштабирование координат на всю площадь полноэкранного окна
        float normX = std::clamp((point.position.x - 180.0f) / 720.0f, 0.0f, 1.0f);
        float normY = std::clamp((point.position.y - 180.0f) / 380.0f, 0.0f, 1.0f);

        float mappedX = mapAreaLeft + normX * (mapAreaRight - mapAreaLeft);
        float mappedY = mapAreaTop + normY * (mapAreaBottom - mapAreaTop);

        ImVec2 center = ImVec2(winPos.x + mappedX, winPos.y + mappedY);
        ImVec2 topLeft = ImVec2(mappedX - r, mappedY - r);

        ImGui::SetCursorPos(topLeft);
        if (ImGui::InvisibleButton("##MapPointBtn", ImVec2(r * 2.0f, r * 2.0f))) {
            selectedMapPointIndex = static_cast<int>(i);
        }

        bool isHovered = ImGui::IsItemHovered();

        ImU32 fillColor = IM_COL32(55, 65, 85, 235);
        ImU32 borderColor = isHovered ? IM_COL32(255, 230, 110, 255) : IM_COL32(190, 195, 210, 255);

        switch (point.type) {
        case MapNodeType::Shop:        fillColor = IM_COL32(45, 110, 75, 240); break;
        case MapNodeType::Combat: {
            if (point.iconKey == "combat_boss") {
                fillColor = IM_COL32(190, 25, 25, 245);
            } else if (point.iconKey == "combat_elite") {
                fillColor = IM_COL32(130, 30, 145, 245);
            } else {
                fillColor = IM_COL32(150, 45, 45, 240);
            }
            break;
        }
        case MapNodeType::Cooking:     fillColor = IM_COL32(155, 95, 35, 240); break;
        case MapNodeType::Crafting:    fillColor = IM_COL32(105, 70, 145, 240); break;
        case MapNodeType::ResourceMine: fillColor = IM_COL32(190, 130, 45, 245); break;
        }

        // Внешнее свечение и пульсация для элитных/босс точек или при наведении
        if (point.iconKey == "combat_boss") {
            float p = sinf(static_cast<float>(ImGui::GetTime()) * 4.5f) * 0.35f + 0.65f;
            drawList->AddCircleFilled(center, r + 8.0f, IM_COL32(255, 30, 30, (int)(110 * p)), 32);
            borderColor = IM_COL32(255, 65, 65, 255);
        } else if (point.iconKey == "combat_elite") {
            float p = sinf(static_cast<float>(ImGui::GetTime()) * 3.5f) * 0.3f + 0.7f;
            drawList->AddCircleFilled(center, r + 6.0f, IM_COL32(215, 60, 255, (int)(85 * p)), 32);
            borderColor = IM_COL32(255, 215, 60, 255);
        } else if (isHovered) {
            drawList->AddCircleFilled(center, r + 6.0f, IM_COL32(255, 220, 90, 70), 32);
        }

        drawList->AddCircleFilled(center, r, fillColor, 32);
        drawList->AddCircle(center, r, borderColor, 32, (isHovered || point.iconKey == "combat_boss") ? 3.5f : 2.0f);

        auto itTex = nodeIconTextures.find(point.iconKey);
        if (itTex != nodeIconTextures.end() && itTex->second.getNativeHandle() != 0) {
            ImTextureID texId = (ImTextureID)(uintptr_t)itTex->second.getNativeHandle();
            float iconHalf = r * 0.65f;
            drawList->AddImage(texId,
                ImVec2(center.x - iconHalf, center.y - iconHalf),
                ImVec2(center.x + iconHalf, center.y + iconHalf));
        }
        else {
            const char* symbol = "?";
            if (point.type == MapNodeType::Shop) symbol = "$";
            else if (point.type == MapNodeType::Combat) symbol = "!";
            else if (point.type == MapNodeType::Cooking) symbol = "+";
            else if (point.type == MapNodeType::Crafting) symbol = "#";

            ImVec2 symSize = ImGui::CalcTextSize(symbol);
            drawList->AddText(ImVec2(center.x - symSize.x * 0.5f, center.y - symSize.y * 0.5f),
                IM_COL32(255, 255, 255, 255), symbol);
        }

        // Текст названия точки под кругом с рамкой
        ImVec2 textSize = ImGui::CalcTextSize(point.title.c_str());
        ImVec2 textPos = ImVec2(center.x - textSize.x * 0.5f, center.y + r + 6.0f);

        drawList->AddRectFilled(ImVec2(textPos.x - 6.0f, textPos.y - 2.0f),
            ImVec2(textPos.x + textSize.x + 6.0f, textPos.y + textSize.y + 2.0f),
            IM_COL32(15, 16, 24, 220), 4.0f);
        drawList->AddRect(ImVec2(textPos.x - 6.0f, textPos.y - 2.0f),
            ImVec2(textPos.x + textSize.x + 6.0f, textPos.y + textSize.y + 2.0f),
            isHovered ? IM_COL32(255, 225, 100, 200) : IM_COL32(100, 90, 70, 160), 4.0f);

        drawList->AddText(textPos, isHovered ? IM_COL32(255, 240, 180, 255) : IM_COL32(240, 240, 240, 255), point.title.c_str());

        ImGui::PopID();
    }

    // --- Правый столбец переключателя этажей ---
    float panelWidth = 110.0f;
    float buttonHeight = 36.0f;
    float spacing = 6.0f;
    int totalFloors = 10;

    float panelHeight = (buttonHeight * totalFloors) + (spacing * (totalFloors - 1));
    float posX = displaySize.x - panelWidth - 25.0f;
    float posY = (displaySize.y - panelHeight) * 0.5f;

    ImGui::SetCursorPos(ImVec2(posX, posY));
    ImGui::BeginGroup();

    ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.3f, 1.0f), "  Этажи Башни");
    ImGui::Separator();
    ImGui::Spacing();

    for (int floor = 10; floor >= 1; --floor) {
        bool isUnlocked = (floor <= maxUnlockedFloor);
        bool isSelected = (currentFloor == floor);

        if (!isUnlocked) {
            ImGui::BeginDisabled();
        }

        if (isSelected) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.85f, 0.60f, 0.20f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.95f, 0.70f, 0.25f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
        }
        else {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.18f, 0.20f, 0.26f, 0.85f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.35f, 0.35f, 0.45f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.85f, 0.85f, 0.85f, 1.0f));
        }

        std::string btnLabel = isUnlocked ? ("Этаж " + std::to_string(floor)) : ("Блок " + std::to_string(floor));
        btnLabel += "##FloorBtn";

        if (ImGui::Button(btnLabel.c_str(), ImVec2(panelWidth, buttonHeight))) {
            currentFloor = floor;
        }

        ImGui::PopStyleColor(3);

        if (!isUnlocked) {
            ImGui::EndDisabled();
        }

        ImGui::Spacing();
    }

    ImGui::EndGroup();
    
    if (isPopupOpen) {
        ImGui::EndDisabled();
    }

    if (isPopupOpen) {
        renderMapPopup();
    }
}

void ScreenManager::renderMapPopup() {
    if (selectedMapPointIndex < 0 || selectedMapPointIndex >= static_cast<int>(mapPoints.size())) {
        selectedMapPointIndex = -1;
        return;
    }

    auto& point = mapPoints[selectedMapPointIndex];
    ImVec2 displaySize = ImGui::GetIO().DisplaySize;

    ImVec2 popupSize = ImVec2(440.0f, 270.0f);
    ImVec2 popupPos = ImVec2((displaySize.x - popupSize.x) * 0.5f, (displaySize.y - popupSize.y) * 0.5f);

    ImGui::SetNextWindowPos(popupPos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(popupSize, ImGuiCond_Always);
    ImGui::SetNextWindowFocus();

    ImGuiWindowFlags popupFlags = ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoSavedSettings;

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.10f, 0.12f, 0.16f, 0.98f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.75f, 0.65f, 0.35f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 2.0f);

    std::string windowTitle = point.title + "##MapPointPopup";
    ImGui::Begin(windowTitle.c_str(), nullptr, popupFlags);

    if (point.isTemporary) {
        ImGui::TextColored(ImVec4(1.0f, 0.65f, 0.25f, 1.0f), "[Временная точка]");
    }
    else {
        ImGui::TextColored(ImVec4(0.5f, 0.85f, 0.5f, 1.0f), "[Постоянная локация]");
    }

    ImGui::Spacing();
    ImGui::TextWrapped("%s", point.description.c_str());
    ImGui::Spacing();
    ImGui::Separator();

    if (point.type == MapNodeType::Shop) {
        ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.2f, 1.0f), "Торговец: %s (Баланс: %d монет)",
            point.merchantName.c_str(), point.merchantInventory.getBalance());
    }
    else if (point.type == MapNodeType::ResourceMine) {
        ImGui::TextColored(ImVec4(0.40f, 0.90f, 1.0f, 1.0f), "[ Добыча полезных ископаемых ]");
        ImGui::Spacing();
        ImGui::TextColored(ImVec4(0.95f, 0.85f, 0.35f, 1.0f), "Богатая жила минералов и ценных самородков.");
        ImGui::TextDisabled("Добывайте руду для ковки редкого оружия и снаряжения в кузнице!");
    }
    else if (point.type == MapNodeType::Combat) {
        ImGui::TextColored(ImVec4(0.95f, 0.4f, 0.4f, 1.0f), "Состав лагеря противников:");
        auto& dbEnemies = GameDatabase::getInstance().enemies;
        for (const auto& enemy : point.enemies) {
            auto it = dbEnemies.find(enemy.enemyId);
            if (it != dbEnemies.end()) {
                ImVec4 rankColor = getEnemyRankColor(it->second->getRank());
                ImGui::TextColored(rankColor, "[%s] ", getEnemyRankName(it->second->getRank()));
                ImGui::SameLine();
                ImGui::Text("%s (Ур. %d) x%d", it->second->getName().c_str(), enemy.level, enemy.count);
            }
            else {
                ImGui::BulletText("%s (Ур. %d) x%d", enemy.enemyName.c_str(), enemy.level, enemy.count);
            }
        }
    }

    float btnWidth = 180.0f;
    float btnHeight = 38.0f;
    ImGui::SetCursorPos(ImVec2(25.0f, popupSize.y - btnHeight - 18.0f));

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.20f, 0.65f, 0.30f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.30f, 0.80f, 0.40f, 1.0f));
    if (ImGui::Button("Перейти", ImVec2(btnWidth, btnHeight))) {
        if (point.isTemporary) {
            point.isCompleted = true;
        }

        switch (point.type) {
        case MapNodeType::Shop:
            openShop(point.merchantName, &point.merchantInventory);
            break;

        case MapNodeType::Combat: {
            std::vector<std::string> currentSquad;
            for (const auto& slot : playerProfile.getActiveSquad()) {
                if (slot) currentSquad.push_back(slot->getId());
            }
            if (currentSquad.empty()) {
                currentSquad = { "adeir", "renwick", "garosh" };
            }
            startCombat(currentSquad, point.enemies);
            break;
        }

        case MapNodeType::Cooking:
            currentScreen = ScreenState::Cooking;
            break;

        case MapNodeType::Crafting:
            currentScreen = ScreenState::Crafting;
            break;

        case MapNodeType::ResourceMine:
            currentMiningFloor = currentFloor;
            miningAttemptsLeft = 5;
            miningTotalYield = 0;
            miningFeedbackMsg = "Сконцентрируйтесь и нанесите удар киркой в зеленую резонансную зону!";
            miningFeedbackColor = ImVec4(0.4f, 0.9f, 1.0f, 1.0f);
            currentScreen = ScreenState::Mining;
            break;
        }

        selectedMapPointIndex = -1;
    }
    ImGui::PopStyleColor(2);

    ImGui::SameLine(0.0f, 25.0f);

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.65f, 0.20f, 0.20f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.80f, 0.30f, 0.30f, 1.0f));
    if (ImGui::Button("Отмена", ImVec2(btnWidth, btnHeight))) {
        selectedMapPointIndex = -1;
    }
    ImGui::PopStyleColor(2);

    ImGui::End();

    ImGui::PopStyleVar();
    ImGui::PopStyleColor(2);
}

// =============================================================================
// ДОСТИЖЕНИЯ
// =============================================================================
void ScreenManager::initAchievements() {
    achievements.clear();

    // ==========================================
    // 1. ГЛОБАЛЬНЫЕ ДОСТИЖЕНИЯ (ОБЩИЕ ДЛЯ ВСЕХ СОХРАНЕНИЙ)
    // ==========================================
    achievements.push_back({
        "global_first_step",
        "Первые шаги",
        "Начать восхождение по таинственному Шпилю в любом сохранении.",
        "*",
        true,
        true
    });
    achievements.push_back({
        "global_floor_5",
        "Покоритель высот",
        "Достичь 5-го этажа Башни (Свалка Забвения).",
        "*",
        true,
        false
    });
    achievements.push_back({
        "global_floor_10",
        "Врата Истока",
        "Добраться до 10-го этажа Башни (Престол Первозданного Истока).",
        "*",
        true,
        false
    });
    achievements.push_back({
        "global_master_blacksmith",
        "Мастер горна",
        "Выковать легендарное оружие в кузнице из собранных редких руд.",
        "*",
        true,
        false
    });
    achievements.push_back({
        "global_hero_collector",
        "Легендарный созыв",
        "Собрать 10 уникальных героев в своем резерве или отряде.",
        "*",
        true,
        false
    });
    achievements.push_back({
        "global_mining_king",
        "Золотая жила",
        "Совершить критический точный удар киркой по рудной жиле.",
        "*",
        true,
        false
    });
    achievements.push_back({
        "global_boss_slayer",
        "Гроза чудовищ",
        "Одержать сокрушительную победу над великим боссом этажа.",
        "*",
        true,
        false
    });

    // ==========================================
    // 2. ПРОФИЛЬНЫЕ ДОСТИЖЕНИЯ (ТЕКУЩЕЕ СОХРАНЕНИЕ)
    // ==========================================
    achievements.push_back({
        "profile_first_blood",
        "Первая кровь",
        "Одержать первую победу в тактическом сражении.",
        "1",
        false,
        false
    });
    achievements.push_back({
        "profile_full_gear",
        "Во всеоружии",
        "Полностью заполнить все 5 слотов экипировки у одного персонажа.",
        "2",
        false,
        false
    });
    achievements.push_back({
        "profile_set_synergy",
        "Сила комплекта",
        "Активировать полный комплектный бонус из 4 предметов артефактов.",
        "3",
        false,
        false
    });
    achievements.push_back({
        "profile_rich",
        "Богатей",
        "Накопить более 1000 золотых монет в кошельке отряда.",
        "4",
        false,
        false
    });
}

void ScreenManager::loadGlobalAchievements() {
    std::ifstream file("saves/global_achievements.txt");
    if (!file.is_open()) return;

    std::unordered_set<std::string> unlockedIds;
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (!line.empty()) unlockedIds.insert(line);
    }
    file.close();

    for (auto& ach : achievements) {
        if (ach.isGlobal && unlockedIds.find(ach.id) != unlockedIds.end()) {
            ach.isUnlocked = true;
        }
    }
}

void ScreenManager::saveGlobalAchievements() {
    std::filesystem::create_directories("saves");
    std::ofstream file("saves/global_achievements.txt");
    if (!file.is_open()) return;

    for (const auto& ach : achievements) {
        if (ach.isGlobal && ach.isUnlocked) {
            file << ach.id << "\n";
        }
    }
    file.close();
}

bool ScreenManager::unlockAchievement(const std::string& idOrTitle) {
    std::string needle = toLowerCmdStr(idOrTitle);
    for (auto& ach : achievements) {
        if (toLowerCmdStr(ach.id) == needle || toLowerCmdStr(ach.title).find(needle) != std::string::npos) {
            ach.isUnlocked = true;
            if (ach.isGlobal) {
                saveGlobalAchievements();
            }
            return true;
        }
    }
    return false;
}

bool ScreenManager::lockAchievement(const std::string& idOrTitle) {
    std::string needle = toLowerCmdStr(idOrTitle);
    for (auto& ach : achievements) {
        if (toLowerCmdStr(ach.id) == needle || toLowerCmdStr(ach.title).find(needle) != std::string::npos) {
            ach.isUnlocked = false;
            if (ach.isGlobal) {
                saveGlobalAchievements();
            }
            return true;
        }
    }
    return false;
}

void ScreenManager::renderAchievements() {
    ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.35f, 1.0f), "=== ЗАЛ ДОСТИЖЕНИЙ И НАГРАД ===");
    ImGui::SameLine(ImGui::GetWindowWidth() - 200.0f);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.18f, 0.45f, 0.85f, 1.0f));
    if (ImGui::Button("<- В лобби##AchBack", ImVec2(170.0f, 26.0f))) {
        currentScreen = ScreenState::Lobby;
    }
    ImGui::PopStyleColor();

    ImGui::Separator();
    ImGui::Spacing();

    // Подсчет статистики достижений
    int totalCount = static_cast<int>(achievements.size());
    int totalUnlocked = 0;
    int globalCount = 0;
    int globalUnlocked = 0;
    int profileCount = 0;
    int profileUnlocked = 0;

    for (const auto& ach : achievements) {
        if (ach.isGlobal) {
            globalCount++;
            if (ach.isUnlocked) globalUnlocked++;
        } else {
            profileCount++;
            if (ach.isUnlocked) profileUnlocked++;
        }
        if (ach.isUnlocked) totalUnlocked++;
    }

    float ratio = totalCount > 0 ? (static_cast<float>(totalUnlocked) / static_cast<float>(totalCount)) : 0.0f;

    ImGui::TextColored(ImVec4(0.40f, 0.85f, 1.0f, 1.0f), "Общий прогресс: %d / %d (%.0f%%)",
        totalUnlocked, totalCount, ratio * 100.0f);
    ImGui::SameLine(ImGui::GetWindowWidth() - 430.0f);
    ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.30f, 1.0f), "Глобальные: %d/%d  |  Профиль: %d/%d",
        globalUnlocked, globalCount, profileUnlocked, profileCount);

    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.85f, 0.70f, 0.20f, 1.0f));
    ImGui::ProgressBar(ratio, ImVec2(-1.0f, 14.0f), "");
    ImGui::PopStyleColor();

    ImGui::Spacing();

    // Фильтр-вкладки
    auto filterBtn = [&](const char* title, int tabIdx) {
        bool isActive = (currentAchievementTab == tabIdx);
        if (isActive) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.20f, 0.50f, 0.85f, 1.0f));
        } else {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.12f, 0.15f, 0.22f, 0.85f));
        }
        if (ImGui::Button(title, ImVec2(180.0f, 28.0f))) {
            currentAchievementTab = tabIdx;
        }
        ImGui::PopStyleColor();
    };

    filterBtn("Все достижения", 0);
    ImGui::SameLine();
    filterBtn("Глобальные (Общие)", 1);
    ImGui::SameLine();
    filterBtn("Профиль (Сохранение)", 2);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Скроллируемый список карточек достижений
    ImGui::BeginChild("AchievementsScrollList", ImVec2(0.0f, -10.0f), false);

    float availW = ImGui::GetContentRegionAvail().x;
    float cardW = std::min(availW, 920.0f);
    float startX = (availW - cardW) * 0.5f;
    if (startX < 0.0f) startX = 0.0f;

    for (size_t i = 0; i < achievements.size(); ++i) {
        const auto& ach = achievements[i];

        if (currentAchievementTab == 1 && !ach.isGlobal) continue;
        if (currentAchievementTab == 2 && ach.isGlobal) continue;

        ImGui::SetCursorPosX(startX);
        ImGui::PushID(static_cast<int>(i));

        ImVec4 bgCol = ach.isUnlocked ? ImVec4(0.10f, 0.16f, 0.14f, 0.90f) : ImVec4(0.07f, 0.08f, 0.12f, 0.85f);
        ImVec4 borderCol = ach.isUnlocked
            ? (ach.isGlobal ? ImVec4(1.0f, 0.85f, 0.25f, 0.95f) : ImVec4(0.35f, 0.90f, 0.50f, 0.90f))
            : ImVec4(0.25f, 0.28f, 0.35f, 0.60f);

        ImGui::PushStyleColor(ImGuiCol_ChildBg, bgCol);
        ImGui::PushStyleColor(ImGuiCol_Border, borderCol);
        ImGui::BeginChild("AchCard", ImVec2(cardW, 76.0f), true, ImGuiWindowFlags_NoScrollbar);

        // Иконка
        ImGui::SetCursorPos(ImVec2(14.0f, 14.0f));
        if (ach.isUnlocked) {
            ImGui::TextColored(ach.isGlobal ? ImVec4(1.0f, 0.85f, 0.25f, 1.0f) : ImVec4(0.35f, 0.95f, 0.50f, 1.0f),
                "%s", ach.icon.c_str());
        } else {
            ImGui::TextColored(ImVec4(0.50f, 0.50f, 0.55f, 0.8f), "?");
        }

        // Заголовок и тип
        ImGui::SetCursorPos(ImVec2(56.0f, 10.0f));
        if (ach.isGlobal) {
            ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.30f, 1.0f), "ГЛОБАЛЬНОЕ");
            ImGui::SameLine();
        } else {
            ImGui::TextColored(ImVec4(0.40f, 0.80f, 1.0f, 1.0f), "ПРОФИЛЬ");
            ImGui::SameLine();
        }

        ImGui::TextColored(ach.isUnlocked ? ImVec4(1.0f, 1.0f, 1.0f, 1.0f) : ImVec4(0.65f, 0.65f, 0.70f, 1.0f),
            "%s", ach.title.c_str());

        // Описание
        ImGui::SetCursorPos(ImVec2(56.0f, 36.0f));
        ImGui::TextColored(ImVec4(0.70f, 0.75f, 0.80f, 1.0f), "%s", ach.description.c_str());

        // Статус справа
        ImGui::SetCursorPos(ImVec2(cardW - 190.0f, 24.0f));
        if (ach.isUnlocked) {
            ImGui::TextColored(ImVec4(0.30f, 1.0f, 0.45f, 1.0f), "РАЗБЛОКИРОВАНО");
        } else {
            ImGui::TextColored(ImVec4(0.50f, 0.50f, 0.55f, 1.0f), "В ПРОЦЕССЕ");
        }

        ImGui::EndChild();
        ImGui::PopStyleColor(2);
        ImGui::PopID();
        ImGui::Spacing();
    }

    ImGui::EndChild();
}

// =============================================================================
// СИСТЕМА КВЕСТОВ И ЗАДАНИЙ
// =============================================================================

void ScreenManager::initQuests() {
    quests.clear();

    // ==========================================
    // СЮЖЕТНЫЕ ЗАДАНИЯ (STORY QUESTS)
    // ==========================================
    quests.push_back({
        "quest_reach_floor3",
        "Вглубь Шпиля",
        "Преодолейте сопротивление монстров и откройте путь на 3 этаж (Подводные глубины).",
        "Исследование",
        true, // Сюжетный!
        0, 1, "3 этаж открыт",
        500, 250, "ruby_ring", 1,
        QuestStatus::InProgress
    });

    quests.push_back({
        "quest_reach_floor5",
        "Путь к Забвению",
        "Доберитесь до 5 этажа Башни (Свалка Забвения) и исследуйте древние механизмы.",
        "Исследование",
        true, // Сюжетный!
        0, 1, "5 этаж достигнут",
        750, 400, "sun_stone", 1,
        QuestStatus::InProgress
    });

    quests.push_back({
        "quest_defeat_boss1",
        "Падение колосса",
        "Сразите Костяного колосса на 1 этаже, чтобы снять печать спуска.",
        "Охота",
        true, // Сюжетный!
        0, 1, "Колосс повержен",
        600, 300, "ancient_core", 1,
        QuestStatus::InProgress
    });

    // ==========================================
    // ПОБОЧНЫЕ ЗАДАНИЯ (SIDE QUESTS)
    // ==========================================
    quests.push_back({
        "quest_rats_skeletons",
        "Зачистка катакомб",
        "Истребите бродячую нежить на 1 этаже Шпиля для защиты караванов.",
        "Охота",
        false, // Побочный!
        0, 5, "уничтожено монстров",
        250, 150, "", 0,
        QuestStatus::InProgress
    });

    quests.push_back({
        "quest_mine_ores",
        "Кузнечные пробы",
        "Добудьте куски железной руды в рудной жиле на карте этажа.",
        "Добыча",
        false, // Побочный!
        0, 5, "железной руды в инвентаре",
        300, 100, "old_cleaver", 1,
        QuestStatus::InProgress
    });

    quests.push_back({
        "quest_forge_weapon",
        "Искусство горна",
        "Используйте ресурсы кузницы для ковки нового клинка, топора или лука.",
        "Кузница",
        false, // Побочный!
        0, 1, "оружие сковано",
        400, 200, "fire_crystal", 2,
        QuestStatus::InProgress
    });

    quests.push_back({
        "quest_slay_elite",
        "Гроза элиты",
        "Сразитесь с любым элитным командиром монстров и одержите верх.",
        "Охота",
        false, // Побочный!
        0, 1, "элитный командир повержен",
        600, 300, "guardian_helm", 1,
        QuestStatus::InProgress
    });

    quests.push_back({
        "quest_cooking_taste",
        "Секреты полевой кухни",
        "Приготовьте любое сытное блюдо в котелке для поддержания сил отряда.",
        "Кулинария",
        false, // Побочный!
        0, 1, "блюдо приготовлено",
        350, 150, "meat_stew", 2,
        QuestStatus::InProgress
    });
}

void ScreenManager::updateQuestsProgress() {
    for (auto& q : quests) {
        if (q.status == QuestStatus::Claimed) continue;

        if (q.id == "quest_mine_ores") {
            q.currentProgress = playerInventory.getItemCount("iron_ore");
        } else if (q.id == "quest_reach_floor3") {
            q.currentProgress = (currentFloor >= 3 || maxUnlockedFloor >= 3) ? 1 : 0;
        } else if (q.id == "quest_reach_floor5") {
            q.currentProgress = (currentFloor >= 5 || maxUnlockedFloor >= 5) ? 1 : 0;
        }

        if (q.currentProgress >= q.targetProgress && q.status == QuestStatus::InProgress) {
            q.status = QuestStatus::Completed;
        }
    }
}

void ScreenManager::renderQuests() {
    updateQuestsProgress();

    ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.35f, 1.0f), "=== ЖУРНАЛ ЗАДАНИЙ И ПОРУЧЕНИЙ ===");
    ImGui::SameLine(ImGui::GetWindowWidth() - 200.0f);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.18f, 0.45f, 0.85f, 1.0f));
    if (ImGui::Button("<- В лобби##QuestsBack", ImVec2(170.0f, 26.0f))) {
        currentScreen = ScreenState::Lobby;
    }
    ImGui::PopStyleColor();

    ImGui::Separator();
    ImGui::Spacing();

    // Подсчет статистики квестов
    int totalCount = static_cast<int>(quests.size());
    int storyCount = 0;
    int sideCount = 0;
    int readyCount = 0;
    int claimedCount = 0;

    for (const auto& q : quests) {
        if (q.isStory) storyCount++;
        else sideCount++;
        if (q.status == QuestStatus::Completed) readyCount++;
        if (q.status == QuestStatus::Claimed) claimedCount++;
    }

    ImGui::TextColored(ImVec4(0.40f, 0.85f, 1.0f, 1.0f), "Всего заданий: %d", totalCount);
    ImGui::SameLine(200.0f);
    if (readyCount > 0) {
        ImGui::TextColored(ImVec4(0.30f, 1.0f, 0.45f, 1.0f), "Готовы к сдаче: %d!", readyCount);
    } else {
        ImGui::TextDisabled("Готовы к сдаче: 0");
    }
    ImGui::SameLine(420.0f);
    ImGui::TextColored(ImVec4(0.85f, 0.80f, 0.40f, 1.0f), "Выполнено: %d", claimedCount);

    ImGui::Spacing();

    // Вкладки выбора категории квестов: Все, Сюжетные, Побочные
    auto questTabBtn = [&](const char* title, int tabIdx) {
        bool isActive = (currentQuestFilterTab == tabIdx);
        if (isActive) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.20f, 0.50f, 0.85f, 1.0f));
        } else {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.12f, 0.15f, 0.22f, 0.85f));
        }
        if (ImGui::Button(title, ImVec2(170.0f, 28.0f))) {
            currentQuestFilterTab = tabIdx;
        }
        ImGui::PopStyleColor();
    };

    std::string allTitle = "Все квесты (" + std::to_string(totalCount) + ")";
    std::string storyTitle = "Сюжетные (" + std::to_string(storyCount) + ")";
    std::string sideTitle = "Побочные (" + std::to_string(sideCount) + ")";

    questTabBtn(allTitle.c_str(), 0);
    ImGui::SameLine();
    questTabBtn(storyTitle.c_str(), 1);
    ImGui::SameLine();
    questTabBtn(sideTitle.c_str(), 2);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    float availW = ImGui::GetContentRegionAvail().x;
    float leftColW = std::clamp(availW * 0.36f, 320.0f, 400.0f);
    float rightColW = availW - leftColW - 15.0f;

    ImGui::Columns(2, "QuestsMasterDetailColumns", false);
    ImGui::SetColumnWidth(0, leftColW);
    ImGui::SetColumnWidth(1, rightColW);

    auto& db = GameDatabase::getInstance();

    // =========================================================================
    // ЛЕВЫЙ СТОЛБЕЦ: СПИСОК КВЕСТОВ (КЛИКАБЕЛЬНЫЕ)
    // =========================================================================
    ImGui::TextColored(ImVec4(0.40f, 0.85f, 1.0f, 1.0f), "Список заданий:");
    ImGui::Spacing();

    ImGui::BeginChild("QuestsMasterList", ImVec2(0.0f, -10.0f), true);

    int displayedCount = 0;
    QuestData* activeSelectedQuest = nullptr;

    for (size_t i = 0; i < quests.size(); ++i) {
        auto& q = quests[i];
        if (currentQuestFilterTab == 1 && !q.isStory) continue;
        if (currentQuestFilterTab == 2 && q.isStory) continue;

        if (selectedQuestId.empty()) {
            selectedQuestId = q.id;
        }
        if (q.id == selectedQuestId) {
            activeSelectedQuest = &q;
        }

        displayedCount++;
        ImGui::PushID(static_cast<int>(i));

        bool isSelected = (q.id == selectedQuestId);

        ImVec4 bgCol = isSelected 
            ? ImVec4(0.16f, 0.22f, 0.32f, 0.95f)
            : ((q.status == QuestStatus::Completed) 
                ? ImVec4(0.10f, 0.18f, 0.14f, 0.85f) 
                : (q.status == QuestStatus::Claimed ? ImVec4(0.07f, 0.08f, 0.11f, 0.70f) : ImVec4(0.09f, 0.11f, 0.16f, 0.85f)));

        ImVec4 borderCol = isSelected 
            ? ImVec4(1.0f, 0.85f, 0.25f, 1.0f)
            : ((q.status == QuestStatus::Completed)
                ? ImVec4(0.35f, 1.0f, 0.50f, 1.0f)
                : (q.status == QuestStatus::Claimed ? ImVec4(0.30f, 0.35f, 0.45f, 0.50f) : (q.isStory ? ImVec4(0.80f, 0.65f, 0.25f, 0.75f) : ImVec4(0.25f, 0.50f, 0.80f, 0.75f))));

        ImGui::PushStyleColor(ImGuiCol_ChildBg, bgCol);
        ImGui::PushStyleColor(ImGuiCol_Border, borderCol);
        ImGui::BeginChild("QuestItemBox", ImVec2(0.0f, 78.0f), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        // Строка 1: тип квеста и статус
        if (q.isStory) {
            ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.30f, 1.0f), "Сюжетное (%s)", q.category.c_str());
        } else {
            ImGui::TextColored(ImVec4(0.40f, 0.85f, 1.0f, 1.0f), "Побочное (%s)", q.category.c_str());
        }
        ImGui::SameLine(ImGui::GetContentRegionAvail().x - 75.0f);
        if (q.status == QuestStatus::Completed) {
            ImGui::TextColored(ImVec4(0.30f, 1.0f, 0.45f, 1.0f), "Готово!");
        } else if (q.status == QuestStatus::Claimed) {
            ImGui::TextDisabled("Сдано");
        } else {
            ImGui::TextColored(ImVec4(0.40f, 0.75f, 1.0f, 0.85f), "В работе");
        }

        // Строка 2: Название квеста
        ImGui::TextColored(isSelected ? ImVec4(1.0f, 0.95f, 0.65f, 1.0f) : ImVec4(0.95f, 0.95f, 0.95f, 1.0f), "%s", q.title.c_str());

        // Строка 3: Индикатор прогресса
        float pRatio = std::clamp(static_cast<float>(q.currentProgress) / static_cast<float>(q.targetProgress), 0.0f, 1.0f);
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, (pRatio >= 1.0f) ? ImVec4(0.25f, 0.85f, 0.40f, 1.0f) : ImVec4(0.20f, 0.55f, 0.90f, 1.0f));
        ImGui::ProgressBar(pRatio, ImVec2(120.0f, 10.0f), "");
        ImGui::PopStyleColor();
        ImGui::SameLine();
        ImGui::TextDisabled("%d/%d %s", std::min(q.currentProgress, q.targetProgress), q.targetProgress, q.progressLabel.c_str());

        ImGui::EndChild();
        ImGui::PopStyleColor(2);

        // Клик по карточке квеста выбирает его для детального просмотра
        if (ImGui::IsItemHovered()) {
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                selectedQuestId = q.id;
            }
        }

        ImGui::PopID();
        ImGui::Spacing();
    }

    if (displayedCount == 0) {
        ImGui::Spacing();
        ImGui::TextDisabled("В этой категории нет доступных заданий.");
    }

    ImGui::EndChild();

    // =========================================================================
    // ПРАВЫЙ СТОЛБЕЦ: ПОДРОБНОЕ ОПИСАНИЕ И НАГРАДЫ КВЕСТА
    // =========================================================================
    ImGui::NextColumn();

    if (!activeSelectedQuest && !quests.empty()) {
        for (auto& q : quests) {
            if (currentQuestFilterTab == 1 && !q.isStory) continue;
            if (currentQuestFilterTab == 2 && q.isStory) continue;
            activeSelectedQuest = &q;
            selectedQuestId = q.id;
            break;
        }
    }

    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.08f, 0.10f, 0.15f, 0.90f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.25f, 0.50f, 0.80f, 0.70f));
    ImGui::BeginChild("QuestDetailPanel", ImVec2(0.0f, -10.0f), true);

    if (activeSelectedQuest) {
        auto& q = *activeSelectedQuest;

        // Заголовок квеста
        ImGui::TextColored(ImVec4(1.0f, 0.88f, 0.35f, 1.0f), "%s", q.title.c_str());
        if (q.isStory) {
            ImGui::TextColored(ImVec4(1.0f, 0.80f, 0.30f, 1.0f), "Сюжетное задание  |  Категория: %s", q.category.c_str());
        } else {
            ImGui::TextColored(ImVec4(0.40f, 0.85f, 1.0f, 1.0f), "Побочное поручение  |  Категория: %s", q.category.c_str());
        }

        ImGui::Separator();
        ImGui::Spacing();

        // Описание
        ImGui::TextColored(ImVec4(0.85f, 0.88f, 0.95f, 1.0f), "Описание задания:");
        ImGui::TextWrapped("%s", q.description.c_str());

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Цель и прогресс
        ImGui::TextColored(ImVec4(0.40f, 0.85f, 1.0f, 1.0f), "Цель задания:");
        ImGui::Text("%s", q.progressLabel.c_str());
        ImGui::Spacing();

        float pRatio = std::clamp(static_cast<float>(q.currentProgress) / static_cast<float>(q.targetProgress), 0.0f, 1.0f);
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, (pRatio >= 1.0f) ? ImVec4(0.25f, 0.85f, 0.40f, 1.0f) : ImVec4(0.20f, 0.55f, 0.90f, 1.0f));
        ImGui::ProgressBar(pRatio, ImVec2(-1.0f, 18.0f), "");
        ImGui::PopStyleColor();
        ImGui::TextDisabled("Текущий прогресс: %d из %d (%d%%)", std::min(q.currentProgress, q.targetProgress), q.targetProgress, static_cast<int>(pRatio * 100.0f));

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Награды
        ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.30f, 1.0f), "Награда за выполнение задания:");
        ImGui::Spacing();

        // Золото
        ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.25f, 1.0f), "Золотые монеты: +%d монет", q.rewardGold);

        // Опыт
        if (q.rewardExp > 0) {
            ImGui::TextColored(ImVec4(0.40f, 0.85f, 1.0f, 1.0f), "Опыт отряда: +%d опыта", q.rewardExp);
        }

        // Предмет
        if (!q.rewardItemId.empty()) {
            ImGui::Spacing();
            auto itItem = db.items.find(q.rewardItemId);
            std::string itemName = (itItem != db.items.end()) ? itItem->second->getName() : q.rewardItemId;

            if (itItem != db.items.end()) {
                ImGui::Image(itItem->second->getImGuiTextureID(), ImVec2(48.0f, 48.0f));
                ImGui::SameLine(0.0f, 12.0f);
            }

            ImGui::BeginGroup();
            ImGui::TextColored(ImVec4(0.90f, 0.70f, 1.0f, 1.0f), "Наградной предмет: %s (x%d)", itemName.c_str(), q.rewardItemCount);
            auto itW = db.weapons.find(q.rewardItemId);
            if (itW != db.weapons.end()) {
                ImGui::TextDisabled("Оружие: %s", itW->second->getEffectDesc().c_str());
            } else {
                auto itA = db.artifacts.find(q.rewardItemId);
                if (itA != db.artifacts.end()) {
                    ImGui::TextDisabled("Артефакт: %s", itA->second->getEffectDesc().c_str());
                } else if (itItem != db.items.end()) {
                    ImGui::TextDisabled("Цена: %d монет", itItem->second->getSellPrice());
                }
            }
            ImGui::EndGroup();
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Кнопка сдачи
        if (q.status == QuestStatus::Completed) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.20f, 0.70f, 0.35f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.28f, 0.85f, 0.45f, 1.0f));
            if (ImGui::Button("Забрать награду##DetailClaim", ImVec2(220.0f, 36.0f))) {
                playerInventory.addFunds(q.rewardGold);
                if (!q.rewardItemId.empty()) {
                    playerInventory.addItem(q.rewardItemId, q.rewardItemCount);
                }
                q.status = QuestStatus::Claimed;
            }
            ImGui::PopStyleColor(2);
        } else if (q.status == QuestStatus::Claimed) {
            ImGui::TextColored(ImVec4(0.40f, 0.85f, 0.50f, 1.0f), "Награда за это задание уже получена.");
        } else {
            ImGui::TextColored(ImVec4(0.50f, 0.70f, 0.90f, 1.0f), "Задание находится в процессе выполнения.");
        }
    } else {
        ImGui::TextDisabled("Выберите задание из списка слева для просмотра деталей.");
    }

    ImGui::EndChild();
    ImGui::PopStyleColor(2);

    ImGui::Columns(1);
}

// =============================================================================
// РЕДАКТОР ОТРЯДА
// =============================================================================
void ScreenManager::renderSquadEditor() {
    ImGui::Text("=== РЕДАКТОР ОТРЯДА ===");
    ImGui::Separator();
    ImGui::Spacing();

    if (!isSquadEditorInitialized) {
        tempSquad = playerProfile.getActiveSquad();
        isSquadEditorInitialized = true;
    }

    ImGui::Columns(3, "SquadSlots", false);
    for (int i = 0; i < 3; ++i) {
        ImGui::PushID(i);

        ImGui::BeginChild("Slot", ImVec2(0, 200), true);
        ImGui::Text("Слот %d", i + 1);
        ImGui::Separator();

        if (tempSquad[i] != nullptr) {
            ImGui::TextColored(ImVec4(0.2f, 0.9f, 0.2f, 1.0f), "%s", tempSquad[i]->getName().c_str());
            ImGui::Text("Уровень: %d", tempSquad[i]->getLevel());
        }
        else {
            ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "[ ПУСТО ]");
        }

        ImGui::SetCursorPosY(160.0f);
        if (ImGui::Button(tempSquad[i] ? "Изменить" : "Добавить", ImVec2(-1, 30))) {
            currentEditingSlot = i;
            showCharacterSelector = true;
        }

        ImGui::EndChild();
        ImGui::PopID();
        ImGui::NextColumn();
    }
    ImGui::Columns(1);
    ImGui::Spacing();
    ImGui::Separator();

    if (ImGui::Button("Сохранить", ImVec2(150, 40))) {
        for (int i = 0; i < 3; ++i) {
            playerProfile.setSquadSlotDirectly(i, tempSquad[i]);
        }
        isSquadEditorInitialized = false;
        currentScreen = ScreenState::Lobby;
    }

    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.45f, 0.85f, 1.0f));
    if (ImGui::Button("<- В Лобби (Без сохранения)", ImVec2(260, 40))) {
        isSquadEditorInitialized = false;
        currentScreen = ScreenState::Lobby;
    }
    ImGui::PopStyleColor();

    if (showCharacterSelector) {
        ImGui::OpenPopup("Выбор персонажа");
    }

    if (ImGui::BeginPopupModal("Выбор персонажа", &showCharacterSelector, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Выберите персонажа для слота %d", currentEditingSlot + 1);
        ImGui::Separator();

        const auto& reserve = playerProfile.getReserve();

        for (size_t i = 0; i < reserve.size(); ++i) {
            auto character = reserve[i];

            bool isAlreadySelected = false;
            for (int j = 0; j < 3; ++j) {
                if (tempSquad[j] == character && j != currentEditingSlot) {
                    isAlreadySelected = true;
                    break;
                }
            }

            if (isAlreadySelected) {
                ImGui::BeginDisabled();
                ImGui::Button((character->getName() + " (Уже в отряде)").c_str(), ImVec2(250, 30));
                ImGui::EndDisabled();
            }
            else {
                if (ImGui::Button(character->getName().c_str(), ImVec2(250, 30))) {
                    tempSquad[currentEditingSlot] = character;
                    showCharacterSelector = false;
                }
            }
        }

        ImGui::Separator();

        if (ImGui::Button("Убрать персонажа", ImVec2(120, 30))) {
            tempSquad[currentEditingSlot] = nullptr;
            showCharacterSelector = false;
        }

        ImGui::SameLine();

        if (ImGui::Button("Закрыть", ImVec2(120, 30))) {
            showCharacterSelector = false;
        }

        ImGui::EndPopup();
    }
}

// =============================================================================
// РЕДАКТОР ПЕРСОНАЖА И СИСТЕМА ЭКИПИРОВКИ
// =============================================================================

// =============================================================================
// РЕДАКТОР ПЕРСОНАЖА И СИСТЕМА ЭКИПИРОВКИ
// =============================================================================

void ScreenManager::renderSlotPickerModal(std::shared_ptr<PlayerCharacter> character) {
    if (!character || activeSlotPicker == ArtifactType::None) return;

    if (requestOpenSlotPicker) {
        ImGui::OpenPopup("##EquipItemModal");
        requestOpenSlotPicker = false;
    }

    bool isModalOpen = true;
    if (ImGui::BeginPopupModal("##EquipItemModal", &isModalOpen, ImGuiWindowFlags_AlwaysAutoResize)) {
        std::string slotName = "ПРЕДМЕТ";
        switch (activeSlotPicker) {
        case ArtifactType::Weapon:   slotName = "ОРУЖИЕ"; break;
        case ArtifactType::Headgear: slotName = "ГОЛОВНОЙ УБОР"; break;
        case ArtifactType::Costume:  slotName = "КОСТЮМ / ДОСПЕХ"; break;
        case ArtifactType::Amulet:   slotName = "АМУЛЕТ"; break;
        case ArtifactType::Ring:     slotName = "КОЛЬЦО"; break;
        default: break;
        }

        ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.30f, 1.0f), "ВЫБЕРИТЕ %s ДЛЯ: %s",
            slotName.c_str(), character->getName().c_str());
        ImGui::TextDisabled("Отображаются только предметы вашего инвентаря, подходящие под этот слот:");
        ImGui::Separator();
        ImGui::Spacing();

        // Опция убрать предмет из этого слота
        std::string equippedId;
        if (activeSlotPicker == ArtifactType::Weapon) {
            equippedId = character->getEquippedWeapon();
        } else if (activeSlotPicker == ArtifactType::Headgear) {
            equippedId = character->getEquippedHeadgear();
        } else if (activeSlotPicker == ArtifactType::Costume) {
            equippedId = character->getEquippedCostume();
        } else if (activeSlotPicker == ArtifactType::Amulet) {
            equippedId = character->getEquippedAmulet();
        } else if (activeSlotPicker == ArtifactType::Ring) {
            equippedId = character->getEquippedRing();
        }

        if (!equippedId.empty()) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.65f, 0.20f, 0.20f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.85f, 0.25f, 0.25f, 1.0f));
            if (ImGui::Button("Убрать предмет из слота (Снять)##UnequipSlotModal", ImVec2(280.0f, 26.0f))) {
                if (activeSlotPicker == ArtifactType::Weapon) {
                    character->unequipWeapon(playerInventory);
                } else {
                    character->unequipArtifact(activeSlotPicker, playerInventory);
                }
                activeSlotPicker = ArtifactType::None;
                ImGui::CloseCurrentPopup();
            }
            ImGui::PopStyleColor(2);
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
        }

        auto& db = GameDatabase::getInstance();
        bool anyFound = false;

        ImGui::BeginChild("EligibleItemsScroll", ImVec2(520.0f, 300.0f), true);

        if (activeSlotPicker == ArtifactType::Weapon) {
            for (const auto& [wId, wTmpl] : db.weapons) {
                int count = playerInventory.getItemCount(wId);
                if (count > 0) {
                    anyFound = true;
                    ImGui::PushID(wId.c_str());

                    // Показываем иконку оружия
                    auto itItem = db.items.find(wId);
                    if (itItem != db.items.end()) {
                        ImGui::Image(itItem->second->getImGuiTextureID(), ImVec2(44.0f, 44.0f));
                        ImGui::SameLine(0.0f, 10.0f);
                    }

                    ImGui::BeginGroup();
                    ImGui::TextColored(ImVec4(0.35f, 0.85f, 1.0f, 1.0f), "%s (В сумке: %d шт.)",
                        wTmpl->getName().c_str(), count);
                    ImGui::TextDisabled("Урон: +%.0f | %s",
                        wTmpl->getDamageAtLevel(character->getLevel()),
                        wTmpl->getEffectDesc().c_str());
                    if (ImGui::Button("Надеть в слот оружия##PickWeapon", ImVec2(180.0f, 24.0f))) {
                        character->equipWeapon(wId, playerInventory);
                        activeSlotPicker = ArtifactType::None;
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndGroup();

                    ImGui::Separator();
                    ImGui::Spacing();
                    ImGui::PopID();
                }
            }
        } else {
            for (const auto& [artId, artTmpl] : db.artifacts) {
                if (artTmpl->getArtifactType() == activeSlotPicker) {
                    int count = playerInventory.getItemCount(artId);
                    if (count > 0) {
                        anyFound = true;
                        ImGui::PushID(artId.c_str());

                        // Показываем иконку артефакта
                        auto itItem = db.items.find(artId);
                        if (itItem != db.items.end()) {
                            ImGui::Image(itItem->second->getImGuiTextureID(), ImVec2(44.0f, 44.0f));
                            ImGui::SameLine(0.0f, 10.0f);
                        }

                        ImGui::BeginGroup();
                        ImGui::TextColored(ImVec4(0.95f, 0.80f, 0.35f, 1.0f), "%s (В сумке: %d шт.)",
                            artTmpl->getName().c_str(), count);
                        if (!artTmpl->getSetId().empty()) {
                            auto itSet = db.artifactSets.find(artTmpl->getSetId());
                            if (itSet != db.artifactSets.end()) {
                                ImGui::TextColored(ImVec4(0.45f, 0.95f, 0.55f, 1.0f), "Комплект: «%s»",
                                    itSet->second.name.c_str());
                            }
                        }
                        ImGui::TextDisabled("Бонусы: %s", artTmpl->getEffectDesc().c_str());
                        if (ImGui::Button("Надеть в слот артефакта##PickArtifact", ImVec2(180.0f, 24.0f))) {
                            character->equipArtifact(activeSlotPicker, artId, playerInventory);
                            activeSlotPicker = ArtifactType::None;
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::EndGroup();

                        ImGui::Separator();
                        ImGui::Spacing();
                        ImGui::PopID();
                    }
                }
            }
        }

        if (!anyFound) {
            ImGui::Spacing();
            ImGui::TextColored(ImVec4(1.0f, 0.45f, 0.45f, 1.0f), "В инвентаре нет доступных предметов для этого слота!");
            ImGui::TextDisabled("Используйте команду 'give all' в консоли для выдачи тестового комплекта.");
            if (ImGui::Button("Выдать начальное снаряжение##QuickGear", ImVec2(240.0f, 26.0f))) {
                playerInventory.addItem("rusty_blade", 1);
                playerInventory.addItem("guardian_helm", 1);
                playerInventory.addItem("guardian_armor", 1);
                playerInventory.addItem("guardian_amulet", 1);
                playerInventory.addItem("guardian_ring", 1);
                playerInventory.addItem("ash_hood", 1);
                playerInventory.addItem("ash_robe", 1);
                playerInventory.addItem("ash_amulet", 1);
                playerInventory.addItem("ash_ring", 1);
                playerInventory.addItem("berserker_mask", 1);
                playerInventory.addItem("berserker_harness", 1);
                playerInventory.addItem("berserker_amulet", 1);
                playerInventory.addItem("berserker_ring", 1);
                playerInventory.addItem("rift_cowl", 1);
                playerInventory.addItem("rift_vestment", 1);
                playerInventory.addItem("rift_talisman", 1);
                playerInventory.addItem("rift_ring", 1);
            }
        }

        ImGui::EndChild();
        ImGui::Spacing();

        if (ImGui::Button("Закрыть окно##ClosePicker", ImVec2(140.0f, 28.0f))) {
            activeSlotPicker = ArtifactType::None;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    if (!isModalOpen || (!requestOpenSlotPicker && !ImGui::IsPopupOpen("##EquipItemModal"))) {
        activeSlotPicker = ArtifactType::None;
    }
}

void ScreenManager::renderCharacterEditor() {
    auto& reserve = playerProfile.getReserve();
    if (reserve.empty()) {
        ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "В вашем отряде пока нет доступных персонажей!");
        if (ImGui::Button("<- В Лобби", ImVec2(160, 30))) currentScreen = ScreenState::Lobby;
        return;
    }

    if (selectedEditorCharIndex >= reserve.size()) {
        selectedEditorCharIndex = 0;
    }
    auto currentChar = reserve[selectedEditorCharIndex];

    // --- ВЕРХНЯЯ ПАНЕЛЬ: ЗАГОЛОВОК И КНОПКА ВЫХОДА ---
    ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.30f, 1.0f), "=== РЕДАКТОР СНАРЯЖЕНИЯ И ХАРАКТЕРИСТИК ГЕРОЕВ ===");
    ImGui::SameLine(ImGui::GetWindowWidth() - 200.0f);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.18f, 0.45f, 0.85f, 1.0f));
    if (ImGui::Button("<- В Лобби##CharBack", ImVec2(170.0f, 26.0f))) {
        currentScreen = ScreenState::Lobby;
    }
    ImGui::PopStyleColor();
    ImGui::Separator();
    ImGui::Spacing();

    // =========================================================================
    // 1. ГАЛЕРЕЯ ПОРТРЕТОВ ПЕРСОНАЖЕЙ (ПОРТРЕТЫ С АВАТАРАМИ)
    // =========================================================================
    ImGui::TextColored(ImVec4(0.40f, 0.85f, 1.0f, 1.0f), "ГЕРОИ ОТРЯДА (Кликните по портрету для выбора):");
    ImGui::Spacing();

    float galleryHeight = 145.0f;
    ImGui::BeginChild("CharacterPortraitGallery", ImVec2(0, galleryHeight), true, ImGuiWindowFlags_HorizontalScrollbar);

    for (size_t i = 0; i < reserve.size(); ++i) {
        auto ch = reserve[i];
        if (!ch) continue;
        ImGui::PushID(static_cast<int>(i));

        bool isSelected = (i == selectedEditorCharIndex);
        bool inSquad = false;
        for (const auto& slot : playerProfile.getActiveSquad()) {
            if (slot == ch) { inSquad = true; break; }
        }

        ImVec4 borderCol = isSelected ? ImVec4(1.0f, 0.85f, 0.25f, 1.0f) : ImVec4(0.35f, 0.40f, 0.50f, 0.6f);
        ImVec4 bgCol = isSelected ? ImVec4(0.18f, 0.24f, 0.34f, 0.95f) : ImVec4(0.08f, 0.10f, 0.15f, 0.75f);

        ImGui::PushStyleColor(ImGuiCol_Border, borderCol);
        ImGui::PushStyleColor(ImGuiCol_ChildBg, bgCol);
        ImGui::BeginChild("PortraitCard", ImVec2(115.0f, 122.0f), true, ImGuiWindowFlags_NoScrollbar);

        auto itPort = characterPortraits.find(ch->getId());
        if (itPort != characterPortraits.end() && itPort->second.getNativeHandle() != 0) {
            ImTextureID tex = (ImTextureID)(uintptr_t)itPort->second.getNativeHandle();
            if (ImGui::ImageButton(tex, ImVec2(95.0f, 62.0f))) {
                selectedEditorCharIndex = i;
            }
        } else {
            if (ImGui::Button(ch->getName().c_str(), ImVec2(95.0f, 62.0f))) {
                selectedEditorCharIndex = i;
            }
        }

        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("%s (Уровень %d)", ch->getName().c_str(), ch->getLevel());
        }

        ImGui::TextColored(isSelected ? ImVec4(1.0f, 0.90f, 0.35f, 1.0f) : ImVec4(0.9f, 0.9f, 0.9f, 1.0f),
                           "%s", ch->getName().c_str());
        if (inSquad) {
            ImGui::TextColored(ImVec4(0.35f, 1.0f, 0.45f, 1.0f), "* В отряде");
        } else {
            ImGui::TextDisabled("Ур. %d", ch->getLevel());
        }

        ImGui::EndChild();
        ImGui::PopStyleColor(2);

        ImGui::SameLine(0.0f, 10.0f);
        ImGui::PopID();
    }
    ImGui::EndChild();

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // =========================================================================
    // 2. СКОМПАКТИРОВАННАЯ ЗОНА: СЛОТЫ ПРИДВИНУТЫ БЛИЖЕ К ЦЕНТРУ, ЦЕНТР УЖЕ
    // =========================================================================
                float availW = ImGui::GetContentRegionAvail().x;

    // Центрирование всего интерфейса редактора по горизонтали
    float totalW = std::clamp(availW, 900.0f, 1220.0f);
    float startX = (availW - totalW) * 0.5f;
    if (startX > 0.0f) {
        ImGui::SetCursorPosX(startX);
    }

    ImGui::BeginGroup();

    // Правая панель характеристик: более узкая (275px)
    float rightPanelW = 275.0f;
    float leftAreaW = totalW - rightPanelW - 15.0f;

    // ГЛАВНЫЙ РАЗДЕЛИТЕЛЬ НА 2 СТОЛБЦА
    ImGui::Columns(2, "CharacterEditorTwoMainColumns", false);
    ImGui::SetColumnWidth(0, leftAreaW);
    ImGui::SetColumnWidth(1, rightPanelW);

    auto& db = GameDatabase::getInstance();

    auto renderSlotCard = [&](const char* slotTitle, ArtifactType slotType, const std::string& currentItemId, const char* iconKey) {
        ImGui::PushID(slotTitle);

        ImVec4 borderCol = currentItemId.empty() ? ImVec4(0.35f, 0.40f, 0.50f, 0.6f) : ImVec4(0.85f, 0.70f, 0.25f, 0.9f);
        ImVec4 bgCol = ImVec4(0.09f, 0.11f, 0.17f, 0.85f);

        ImGui::PushStyleColor(ImGuiCol_ChildBg, bgCol);
        ImGui::PushStyleColor(ImGuiCol_Border, borderCol);
        ImGui::BeginChild("SlotBox", ImVec2(0, 96.0f), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.35f, 1.0f), "%s", slotTitle);
        ImGui::Separator();

        // Иконка слота
        auto itSlotTex = slotIconTextures.find(iconKey);
        if (itSlotTex != slotIconTextures.end() && itSlotTex->second.getNativeHandle() != 0) {
            ImTextureID slotTexId = (ImTextureID)(uintptr_t)itSlotTex->second.getNativeHandle();
            ImGui::Image(slotTexId, ImVec2(44.0f, 44.0f));
            ImGui::SameLine(0.0f, 10.0f);
        }

        ImGui::BeginGroup();
        if (currentItemId.empty()) {
            ImGui::TextDisabled("Пустой слот");
            ImGui::TextColored(ImVec4(0.40f, 0.80f, 1.0f, 0.85f), "Нажмите, чтобы надеть");
        } else {
            if (slotType == ArtifactType::Weapon) {
                auto itW = db.weapons.find(currentItemId);
                if (itW != db.weapons.end()) {
                    ImGui::TextColored(ImVec4(0.35f, 0.85f, 1.0f, 1.0f), "%s", itW->second->getName().c_str());
                    ImGui::TextDisabled("Урон: +%.0f", itW->second->getDamageAtLevel(currentChar->getLevel()));
                }
            } else {
                auto itA = db.artifacts.find(currentItemId);
                if (itA != db.artifacts.end()) {
                    ImGui::TextColored(ImVec4(0.95f, 0.80f, 0.35f, 1.0f), "%s", itA->second->getName().c_str());
                    if (!itA->second->getSetId().empty()) {
                        auto itSet = db.artifactSets.find(itA->second->getSetId());
                        if (itSet != db.artifactSets.end()) {
                            ImGui::TextColored(ImVec4(0.50f, 0.95f, 0.55f, 1.0f), "Сет: %s", itSet->second.name.c_str());
                        }
                    }
                }
            }
            ImGui::TextColored(ImVec4(0.65f, 0.70f, 0.80f, 0.8f), "Нажмите для смены");
        }
        ImGui::EndGroup();

        ImGui::EndChild();
        ImGui::PopStyleColor(2);

        // Вся область слота является кликабельной кнопкой
        if (ImGui::IsItemHovered()) {
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                activeSlotPicker = slotType;
                requestOpenSlotPicker = true;
            }
        }

        ImGui::Spacing();
        ImGui::PopID();
    };

    // =========================================================================
    // ЛЕВЫЙ СТОЛБЕЦ: СНАРЯЖЕНИЕ И ВИЗУАЛ ГЕРОЯ (РАЗБИТ НА 3 СУБКОЛОНКИ)
    // =========================================================================
    float colW1 = 205.0f;
    float colW3 = 205.0f;
    float colW2 = leftAreaW - colW1 - colW3 - 25.0f;

    ImGui::Columns(3, "EditorLeftSlotsModelGrid", false);
    ImGui::SetColumnWidth(0, colW1);
    ImGui::SetColumnWidth(1, colW2);
    ImGui::SetColumnWidth(2, colW3);

    // Подколонка 0: Оружие, Шлем, Костюм
    ImGui::TextColored(ImVec4(0.40f, 0.85f, 1.0f, 1.0f), "Оружие и броня:");
    ImGui::Spacing();
    renderSlotCard("Слот оружия", ArtifactType::Weapon, currentChar->getEquippedWeapon(), "slot_weapon");
    renderSlotCard("Головной убор", ArtifactType::Headgear, currentChar->getEquippedHeadgear(), "slot_headgear");
    renderSlotCard("Костюм / Доспех", ArtifactType::Costume, currentChar->getEquippedCostume(), "slot_costume");

    ImGui::NextColumn();

    // Подколонка 1: Витрина героя с моделью, уровнем, отрядом и навыками
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.08f, 0.09f, 0.14f, 0.85f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.85f, 0.70f, 0.25f, 0.85f));
    ImGui::BeginChild("CenterCharacterShowcase", ImVec2(0, 520.0f), true, ImGuiWindowFlags_NoScrollbar);

    ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.25f, 1.0f), "Герой: %s", currentChar->getName().c_str());
    ImGui::TextDisabled("Уровень: %d", currentChar->getLevel());
    ImGui::Separator();
    ImGui::Spacing();

    // Процедурная модель героя
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 previewCenter = ImGui::GetCursorScreenPos();
    previewCenter.x += (colW2 * 0.5f) - 10.0f;
    previewCenter.y += 65.0f;

    float animTime = static_cast<float>(ImGui::GetTime());
    renderAnimatedHeroModel(drawList, previewCenter, currentChar->getId(), true, animTime, 1.35f);

    ImGui::Dummy(ImVec2(0, 130.0f));

    // Управление участием в отряде
    bool inActiveSquad = false;
    size_t activeSquadSlot = 0;
    for (size_t s = 0; s < 3; ++s) {
        if (playerProfile.getActiveSquad()[s] == currentChar) {
            inActiveSquad = true;
            activeSquadSlot = s;
            break;
        }
    }

    if (inActiveSquad) {
        ImGui::TextColored(ImVec4(0.35f, 1.0f, 0.45f, 1.0f), "В боевом отряде (Слот %d)", static_cast<int>(activeSquadSlot + 1));
        if (ImGui::Button("Убрать из отряда##RemoveSq", ImVec2(220.0f, 26.0f))) {
            playerProfile.removeFromSquad(activeSquadSlot);
        }
    } else {
        ImGui::TextDisabled("Назначить в отряд:");
        if (ImGui::Button("Поставить в отряд##AddSq", ImVec2(220.0f, 26.0f))) {
            bool placed = false;
            for (size_t s = 0; s < 3; ++s) {
                if (playerProfile.getActiveSquad()[s] == nullptr) {
                    playerProfile.setSquadSlotDirectly(s, currentChar);
                    placed = true;
                    break;
                }
            }
            if (!placed) {
                playerProfile.setSquadSlotDirectly(0, currentChar);
            }
        }
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Способности героя (без текстовых тегов)
    auto charTmplIt = db.characters.find(currentChar->getId());
    if (charTmplIt != db.characters.end()) {
        ImGui::TextColored(ImVec4(0.40f, 0.85f, 1.0f, 1.0f), "Способности героя:");
        const auto& skills = charTmplIt->second->getSkills();
        for (size_t sk = 0; sk < skills.size(); ++sk) {
            std::string skTag = (sk == 0) ? "Базовая атака: " : (sk == 1) ? "Навык: " : "Ультимейт: ";
            ImGui::TextColored(ImVec4(0.95f, 0.90f, 0.70f, 1.0f), "%s%s", skTag.c_str(), skills[sk]->getName().c_str());
            if (!skills[sk]->getDescription().empty()) {
                ImGui::TextDisabled("  %s", skills[sk]->getDescription().c_str());
            }
        }
    }

    ImGui::EndChild();
    ImGui::PopStyleColor(2);

    ImGui::NextColumn();

    // Подколонка 2: Амулет, Кольцо, и Сеты артефактов
    ImGui::TextColored(ImVec4(0.40f, 0.85f, 1.0f, 1.0f), "Бижутерия и сеты:");
    ImGui::Spacing();
    renderSlotCard("Талисман", ArtifactType::Amulet, currentChar->getEquippedAmulet(), "slot_amulet");
    renderSlotCard("Перстень", ArtifactType::Ring, currentChar->getEquippedRing(), "slot_ring");

    // Панель комплектов артефактов
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.08f, 0.10f, 0.15f, 0.85f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.40f, 0.70f, 0.50f, 0.80f));
    ImGui::BeginChild("ArtifactSetsPanel", ImVec2(0, 275.0f), true, ImGuiWindowFlags_NoScrollbar);

    ImGui::TextColored(ImVec4(0.45f, 0.95f, 0.55f, 1.0f), "Бонусы комплектов:");
    ImGui::Separator();
    ImGui::Spacing();

    auto setCounts = currentChar->getActiveSetCounts();
    if (setCounts.empty()) {
        ImGui::TextDisabled("Сеты артефактов не активированы.");
        ImGui::Spacing();
        ImGui::TextDisabled("Экипируйте 2 или 4 предмета одного сета для получения пассивных бонусов.");
    } else {
        for (const auto& [setId, count] : setCounts) {
            auto itSet = db.artifactSets.find(setId);
            if (itSet == db.artifactSets.end()) continue;

            ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.35f, 1.0f), "«%s» (%d/4):", itSet->second.name.c_str(), count);
            if (count >= 2) {
                ImGui::TextColored(ImVec4(0.35f, 1.0f, 0.45f, 1.0f), " (2/4) Активно: %s", itSet->second.twoPieceDesc.c_str());
            } else {
                ImGui::TextDisabled(" (2/4) Не активно: %s (требуется еще %d)", itSet->second.twoPieceDesc.c_str(), 2 - count);
            }
            if (count >= 4) {
                ImGui::TextColored(ImVec4(0.35f, 1.0f, 0.45f, 1.0f), " (4/4) Активно: %s", itSet->second.fourPieceDesc.c_str());
            } else {
                ImGui::TextDisabled(" (4/4) Не активно: %s (требуется еще %d)", itSet->second.fourPieceDesc.c_str(), 4 - count);
            }
            ImGui::Spacing();
        }
    }

    ImGui::EndChild();
    ImGui::PopStyleColor(2);

    ImGui::Columns(1); // Завершаем подколонки слотов

    // =========================================================================
    // ПРАВЫЙ СТОЛБЕЦ: ИТОГОВЫЕ ХАРАКТЕРИСТИКИ (УЗКАЯ ПАНЕЛЬ БЕЗ СПИСКА АРТЕФАКТОВ И ТЕГОВ)
    // =========================================================================
    ImGui::NextColumn();

    StatArray totalStats = currentChar->getTotalStats();

    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.08f, 0.10f, 0.16f, 0.95f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.35f, 0.70f, 0.95f, 0.95f));
    ImGui::BeginChild("CharacterTotalStatsPanelLarge", ImVec2(0, 520.0f), true, ImGuiWindowFlags_NoScrollbar);

    ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.30f, 1.0f), "=== ХАРАКТЕРИСТИКИ ===");
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::TextColored(ImVec4(0.95f, 0.90f, 0.60f, 1.0f), "Герой: %s", currentChar->getName().c_str());
    ImGui::TextColored(ImVec4(0.40f, 0.85f, 1.0f, 1.0f), "Уровень героя: %d", currentChar->getLevel());
    ImGui::Spacing();

    // Здоровье и Мана
    ImGui::TextColored(ImVec4(0.35f, 1.0f, 0.45f, 1.0f), "Здоровье (HP): %.0f", totalStats[StatIndex::HP]);
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.25f, 0.85f, 0.40f, 1.0f));
    ImGui::ProgressBar(1.0f, ImVec2(-1.0f, 14.0f), "");
    ImGui::PopStyleColor();

    ImGui::TextColored(ImVec4(0.35f, 0.80f, 1.0f, 1.0f), "Запас маны: %.0f", totalStats[StatIndex::Mana]);
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.20f, 0.55f, 0.95f, 1.0f));
    ImGui::ProgressBar(1.0f, ImVec2(-1.0f, 14.0f), "");
    ImGui::PopStyleColor();

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Боевые параметры (без текстовых тегов в скобках)
    ImGui::TextColored(ImVec4(1.0f, 0.65f, 0.30f, 1.0f), "Сила атаки: %.1f", totalStats[StatIndex::Attack]);

    double mitigation = (totalStats[StatIndex::Defense] / (totalStats[StatIndex::Defense] + 100.0)) * 100.0;
    ImGui::TextColored(ImVec4(0.40f, 0.85f, 1.0f, 1.0f), "Защита: %.1f (-%.1f%%)", totalStats[StatIndex::Defense], mitigation);

    ImGui::Text("Крит. шанс: %.1f%%", totalStats[StatIndex::CritChance] * 100.0);
    ImGui::Text("Крит. урон: %.0f%%", totalStats[StatIndex::CritDamage] * 100.0);
    ImGui::Text("Вампиризм: %.1f%%", totalStats[StatIndex::Vampirism] * 100.0);
    ImGui::Text("Бонус умений: +%.1f%%", totalStats[StatIndex::DamageBonus] * 100.0);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Расчетный боевой рейтинг
    int powerScore = static_cast<int>(
        totalStats[StatIndex::HP] * 0.2 +
        totalStats[StatIndex::Mana] * 0.15 +
        totalStats[StatIndex::Attack] * 2.5 +
        totalStats[StatIndex::Defense] * 2.0 +
        totalStats[StatIndex::CritChance] * 250.0 +
        totalStats[StatIndex::DamageBonus] * 120.0
    );
    ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.25f, 1.0f), "Боевая мощь героя: %d", powerScore);

    ImGui::EndChild();
    ImGui::PopStyleColor(2);

    ImGui::Columns(1); // Завершаем главный сплит

    ImGui::EndGroup(); // Завершаем центрированную группу

    renderSlotPickerModal(currentChar);
}

void ScreenManager::logCombat(const std::string& msg, const ImVec4& color) {
    combatLog.push_back({ msg, color });
    if (combatLog.size() > 120) {
        combatLog.erase(combatLog.begin());
    }
    scrollToBottomCombatLog = true;
}

void ScreenManager::initCombatState() {
    combatSquad.clear();
    combatEnemies.clear();
    combatLog.clear();
    combatTurnNumber = 1;
    activeCharacterIndex = 0;
    selectedEnemyTarget = 0;
    combatPhase = CombatPhase::PlayerTurn;

    auto& dbChars = GameDatabase::getInstance().characters;

    // Всегда берем текущий актуальный активный отряд игрока
    activeCombatPlayerSquad.clear();
    for (const auto& slot : playerProfile.getActiveSquad()) {
        if (slot) activeCombatPlayerSquad.push_back(slot->getId());
    }
    if (activeCombatPlayerSquad.empty()) {
        activeCombatPlayerSquad = { "adeir", "renwick", "garosh" };
    }

    for (const auto& charId : activeCombatPlayerSquad) {
        auto it = dbChars.find(charId);
        if (it == dbChars.end()) continue;

        CombatUnit unit;
        unit.id = charId;
        unit.name = it->second->getName();
        unit.level = 1;
        unit.stats = it->second->getBaseStats();

        // Проверяем характеристики из профиля игрока
        for (const auto& slot : playerProfile.getActiveSquad()) {
            if (slot && slot->getId() == charId) {
                unit.level = slot->getLevel();
                unit.stats = slot->getTotalStats();
                break;
            }
        }

        unit.maxHp = unit.stats[StatIndex::HP];
        unit.currentHp = unit.maxHp;
        unit.maxMana = unit.stats[StatIndex::Mana];
        unit.currentMana = unit.maxMana;
        unit.shield = 0.0;
        unit.skillStacks = 0;
        unit.skillCooldowns = { 0, 0, 0 };

        combatSquad.push_back(unit);
    }

    if (combatSquad.empty()) {
        CombatUnit fallback;
        fallback.id = "adeir";
        fallback.name = "Адеир";
        fallback.stats = { 110.0, 90.0, 18.0, 14.0, 0.05, 1.50, 0.0, 0.0 };
        fallback.maxHp = 110.0;
        fallback.currentHp = 110.0;
        fallback.maxMana = 90.0;
        fallback.currentMana = 90.0;
        combatSquad.push_back(fallback);
    }

    // Инициализация противников
    auto& dbEnemies = GameDatabase::getInstance().enemies;
    if (activeCombatEnemies.empty()) {
        activeCombatEnemies.push_back({ "f1_necropolis_rat", "Чумная крыса", 1, 1 });
    }

    int totalEnemyHp = 0;
    for (const auto& spawn : activeCombatEnemies) {
        auto it = dbEnemies.find(spawn.enemyId);
        if (it == dbEnemies.end()) continue;
        const auto& t = it->second;

        for (int c = 0; c < spawn.count; ++c) {
            CombatEnemy e;
            e.enemyId = spawn.enemyId;
            e.name = t->getName() + (spawn.count > 1 ? (" #" + std::to_string(c + 1)) : "");
            e.level = spawn.level;
            e.rank = t->getRank();
            e.maxHp = t->getHpAtLevel(spawn.level);
            e.currentHp = e.maxHp;
            e.attack = t->getAttackAtLevel(spawn.level);
            e.defense = t->getDefenseAtLevel(spawn.level);
            e.shield = 0.0;
            e.attacks = t->getAttacks();
            e.attackCooldowns.assign(e.attacks.size(), 0);

            totalEnemyHp += static_cast<int>(e.maxHp);
            combatEnemies.push_back(e);
        }
    }

    if (combatEnemies.empty()) {
        CombatEnemy rat;
        rat.enemyId = "f1_necropolis_rat";
        rat.name = "Чумная крыса";
        rat.level = 1;
        rat.rank = EnemyRank::Normal;
        rat.maxHp = 60.0;
        rat.currentHp = 60.0;
        rat.attack = 10.0;
        rat.defense = 2.0;
        rat.attacks = {
            { "Быстрый укус", "Быстрая атака без перезарядки.", 1.0, 0, 0.0, 0.0, false },
            { "Бешеный укус", "Периодический урон ядом.", 1.35, 2, 0.0, 0.0, false },
            { "Яростный натиск", "Мощный отчаянный выпад.", 1.65, 3, 0.0, 0.0, false }
        };
        rat.attackCooldowns = { 0, 0, 0 };
        combatEnemies.push_back(rat);
    }

    combatRewardGold = std::max(50, (currentFloor * 30) + (totalEnemyHp / 8));

    logCombat("=== СРАЖЕНИЕ НАЧАЛОСЬ ===", ImVec4(1.0f, 0.85f, 0.2f, 1.0f));
    logCombat("Раунд 1. Ваш ход! Выберите умение активного бойца внизу экрана.", ImVec4(0.4f, 1.0f, 0.4f, 1.0f));

    isCombatInitialized = true;
}

void ScreenManager::processPlayerSkill(size_t skillIndex) {
    if (combatPhase != CombatPhase::PlayerTurn) return;
    if (activeCharacterIndex >= combatSquad.size()) return;

    auto& hero = combatSquad[activeCharacterIndex];
    if (!hero.isAlive()) return;
    if (skillIndex >= 3) return;
    if (hero.skillCooldowns[skillIndex] > 0) return;

    auto& dbChars = GameDatabase::getInstance().characters;
    auto it = dbChars.find(hero.id);
    if (it == dbChars.end()) return;

    const auto& skills = it->second->getSkills();
    if (skillIndex >= skills.size()) return;
    const auto& skill = skills[skillIndex];

    // Проверяем живую цель
    if (selectedEnemyTarget >= static_cast<int>(combatEnemies.size()) || !combatEnemies[selectedEnemyTarget].isAlive()) {
        selectedEnemyTarget = -1;
        for (size_t i = 0; i < combatEnemies.size(); ++i) {
            if (combatEnemies[i].isAlive()) {
                selectedEnemyTarget = static_cast<int>(i);
                break;
            }
        }
        if (selectedEnemyTarget == -1) {
            combatPhase = CombatPhase::Victory;
            return;
        }
    }

    double rawValue = skill->calculateValue(hero.stats, hero.skillStacks);
    const std::string& skillName = skill->getName();

    bool isShield = (skillName.find("щит") != std::string::npos || skillName.find("Щит") != std::string::npos ||
                     skillName.find("кожа") != std::string::npos || skillName.find("Кожа") != std::string::npos);
    bool isHeal = (skillName.find("дыхание") != std::string::npos || skillName.find("Дыхание") != std::string::npos ||
                   skillName.find("свет") != std::string::npos || skillName.find("Свет") != std::string::npos ||
                   skillName.find("исцел") != std::string::npos || skillName.find("Исцел") != std::string::npos);

    // Управление стаками и КД
    if (skill->getConsumesAllStacks()) {
        hero.skillStacks = 0;
    } else if (skill->getStacksGranted() > 0) {
        hero.skillStacks += skill->getStacksGranted();
    }
    hero.skillCooldowns[skillIndex] = skill->getCooldown();

    if (isShield) {
        hero.shield += rawValue;
        logCombat(hero.name + " активирует '" + skillName + "' и накладывает щит: +" + std::to_string(static_cast<int>(rawValue)) + "!", ImVec4(0.3f, 0.8f, 1.0f, 1.0f));
        executeEnemyTurn();
    } else if (isHeal) {
        double healed = std::min(hero.maxHp - hero.currentHp, rawValue);
        hero.currentHp = std::min(hero.maxHp, hero.currentHp + rawValue);
        logCombat(hero.name + " применяет '" + skillName + "' и восстанавливает +" + std::to_string(static_cast<int>(healed)) + " HP!", ImVec4(0.3f, 1.0f, 0.4f, 1.0f));
        executeEnemyTurn();
    } else {
        auto& target = combatEnemies[selectedEnemyTarget];
        bool isCrit = ((rand() % 1000) / 1000.0) < hero.stats[StatIndex::CritChance];
        double critMult = isCrit ? hero.stats[StatIndex::CritDamage] : 1.0;
        double dmgBonus = 1.0 + hero.stats[StatIndex::DamageBonus];
        double totalDmg = rawValue * critMult * dmgBonus;
        double netDmg = std::max(1.0, totalDmg - (target.defense * 0.4));

        // Запуск визуальной анимации атаки и применения эффектов
        combatAnim.isPlaying = true;
        combatAnim.isHero = true;
        combatAnim.attackerId = hero.id;
        combatAnim.heroIndex = activeCharacterIndex;
        combatAnim.targetIndex = static_cast<size_t>(selectedEnemyTarget);
        combatAnim.skillIndex = skillIndex;
        combatAnim.timer = 0.0f;
        combatAnim.duration = 0.85f;
        combatAnim.damage = netDmg;
        combatAnim.isCrit = isCrit;
        combatAnim.damageApplied = false;
        combatAnim.skillName = skillName;
    }
}

void ScreenManager::executeEnemyTurn() {
    combatPhase = CombatPhase::EnemyTurn;

    std::vector<size_t> livingHeroIndices;
    for (size_t i = 0; i < combatSquad.size(); ++i) {
        if (combatSquad[i].isAlive()) livingHeroIndices.push_back(i);
    }

    if (livingHeroIndices.empty()) {
        combatPhase = CombatPhase::Defeat;
        logCombat("Весь отряд разгромлен... Поражение!", ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
        return;
    }

    // Собираем всех живых врагов
    std::vector<size_t> livingEnemyIndices;
    for (size_t i = 0; i < combatEnemies.size(); ++i) {
        if (combatEnemies[i].isAlive()) livingEnemyIndices.push_back(i);
    }

    if (livingEnemyIndices.empty()) {
        combatPhase = CombatPhase::Victory;
        logCombat("=== ПОБЕДА! Все противники повержены! ===", ImVec4(0.3f, 1.0f, 0.3f, 1.0f));
        return;
    }

    // =========================================================================
    // ПРАВИЛО: ЗА ОДИН ХОД АТАКУЕТ ТОЛЬКО 1 ПРОТИВНИК!
    // =========================================================================
    // Выбираем 1 атакующего врага (приоритет: Босс -> Элита -> циклический выбор среди обычных)
    size_t chosenEnemyIdx = livingEnemyIndices.front();
    for (size_t eIdx : livingEnemyIndices) {
        if (combatEnemies[eIdx].rank == EnemyRank::Boss) {
            chosenEnemyIdx = eIdx;
            break;
        } else if (combatEnemies[eIdx].rank == EnemyRank::Elite) {
            chosenEnemyIdx = eIdx;
        }
    }
    if (combatEnemies[chosenEnemyIdx].rank == EnemyRank::Normal && livingEnemyIndices.size() > 1) {
        static size_t attackerCycle = 0;
        attackerCycle = (attackerCycle + 1) % livingEnemyIndices.size();
        chosenEnemyIdx = livingEnemyIndices[attackerCycle];
    }

    auto& enemy = combatEnemies[chosenEnemyIdx];

    // Выбор атаки врага (3 -> 2 -> 1)
    size_t chosenAtk = 0;
    if (enemy.attacks.size() >= 3 && enemy.attackCooldowns[2] == 0) {
        bool hpLow = (enemy.currentHp / enemy.maxHp) < 0.5;
        if (enemy.rank == EnemyRank::Boss || enemy.rank == EnemyRank::Elite || hpLow || (rand() % 100 < 65)) {
            chosenAtk = 2;
        }
    }
    if (chosenAtk == 0 && enemy.attacks.size() >= 2 && enemy.attackCooldowns[1] == 0) {
        chosenAtk = 1;
    }

    const auto& attack = enemy.attacks[chosenAtk];
    enemy.attackCooldowns[chosenAtk] = attack.cooldown;

    // Выполнение атаки выбранного единственного противника
    if (attack.isAoe) {
        logCombat("[МАССОВАЯ АТАКА] " + enemy.name + " обрушивает '" + attack.name + "' на всех бойцов!", ImVec4(1.0f, 0.25f, 0.25f, 1.0f));
        for (size_t hIdx : livingHeroIndices) {
            auto& hero = combatSquad[hIdx];
            double rawDmg = enemy.attack * attack.damageMultiplier;
            double heroDef = hero.stats[StatIndex::Defense] * (1.0 - attack.defensePierce);
            double netDmg = std::max(1.0, rawDmg - heroDef * 0.4);
            if (hero.shield > 0.0) {
                double absShield = std::min(hero.shield, netDmg);
                hero.shield -= absShield;
                netDmg -= absShield;
            }
            hero.currentHp = std::max(0.0, hero.currentHp - netDmg);
            logCombat("  -> " + hero.name + " получает " + std::to_string(static_cast<int>(netDmg)) + " урона!", ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
        }
    } else {
        size_t targetIdx = activeCharacterIndex;
        if (!combatSquad[targetIdx].isAlive()) {
            targetIdx = livingHeroIndices.front();
        }

        if (enemy.rank == EnemyRank::Elite) {
            size_t lowestHpIdx = livingHeroIndices.front();
            for (size_t idx : livingHeroIndices) {
                if (combatSquad[idx].currentHp < combatSquad[lowestHpIdx].currentHp) {
                    lowestHpIdx = idx;
                }
            }
            if ((combatSquad[lowestHpIdx].currentHp / combatSquad[lowestHpIdx].maxHp < 0.35) && (rand() % 100 < 70)) {
                targetIdx = lowestHpIdx;
            }
        } else if (enemy.rank == EnemyRank::Normal) {
            if (rand() % 100 >= 85 && livingHeroIndices.size() > 1) {
                targetIdx = livingHeroIndices[rand() % livingHeroIndices.size()];
            }
        }

        auto& targetHero = combatSquad[targetIdx];
        double rawDmg = enemy.attack * attack.damageMultiplier;
        double heroDef = targetHero.stats[StatIndex::Defense] * (1.0 - attack.defensePierce);
        double netDmg = std::max(1.0, rawDmg - heroDef * 0.4);

        if (targetHero.shield > 0.0) {
            double absShield = std::min(targetHero.shield, netDmg);
            targetHero.shield -= absShield;
            netDmg -= absShield;
        }
        targetHero.currentHp = std::max(0.0, targetHero.currentHp - netDmg);

        if (attack.lifesteal > 0.0) {
            double heal = netDmg * attack.lifesteal;
            enemy.currentHp = std::min(enemy.maxHp, enemy.currentHp + heal);
        }

        logCombat(enemy.name + " атакует [" + attack.name + "] по " + targetHero.name + " (-" + std::to_string(static_cast<int>(netDmg)) + " HP)!", ImVec4(1.0f, 0.45f, 0.45f, 1.0f));

        if (!targetHero.isAlive()) {
            logCombat("[ПАЛ В БОЮ] " + targetHero.name + " потерял сознание!", ImVec4(1.0f, 0.15f, 0.15f, 1.0f));
        }
    }

    if (!combatSquad[activeCharacterIndex].isAlive()) {
        for (size_t i = 0; i < combatSquad.size(); ++i) {
            if (combatSquad[i].isAlive()) {
                activeCharacterIndex = i;
                logCombat("В бой вступает резервный боец: " + combatSquad[activeCharacterIndex].name + "!", ImVec4(0.4f, 1.0f, 0.4f, 1.0f));
                break;
            }
        }
    }

    bool anyHeroAlive = false;
    for (const auto& h : combatSquad) {
        if (h.isAlive()) { anyHeroAlive = true; break; }
    }
    if (!anyHeroAlive) {
        combatPhase = CombatPhase::Defeat;
        logCombat("=== ВЕСЬ ОТРЯД ПОВЕРЖЕН... ПОРАЖЕНИЕ ===", ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
        return;
    }

    endCombatTurn();
}

void ScreenManager::endCombatTurn() {
    for (auto& hero : combatSquad) {
        for (auto& cd : hero.skillCooldowns) {
            if (cd > 0) cd--;
        }
    }
    for (auto& enemy : combatEnemies) {
        for (auto& cd : enemy.attackCooldowns) {
            if (cd > 0) cd--;
        }
    }

    combatTurnNumber++;
    combatPhase = CombatPhase::PlayerTurn;
    logCombat("--- Раунд " + std::to_string(combatTurnNumber) + ": Ваш ход ---", ImVec4(0.35f, 0.85f, 1.0f, 1.0f));
}


// =============================================================================

static void drawEllipseFilled(ImDrawList* drawList, const ImVec2& center, float rx, float ry, ImU32 col, int num_segments = 16) {
    std::vector<ImVec2> pts;
    pts.reserve(num_segments);
    for (int i = 0; i < num_segments; ++i) {
        float a = (static_cast<float>(i) / static_cast<float>(num_segments)) * 6.2831853f;
        pts.push_back(ImVec2(center.x + cosf(a) * rx, center.y + sinf(a) * ry));
    }
    drawList->AddConvexPolyFilled(pts.data(), static_cast<int>(pts.size()), col);
}

static void drawEllipse(ImDrawList* drawList, const ImVec2& center, float rx, float ry, ImU32 col, int num_segments = 16, float thickness = 1.0f) {
    std::vector<ImVec2> pts;
    pts.reserve(num_segments);
    for (int i = 0; i < num_segments; ++i) {
        float a = (static_cast<float>(i) / static_cast<float>(num_segments)) * 6.2831853f;
        pts.push_back(ImVec2(center.x + cosf(a) * rx, center.y + sinf(a) * ry));
    }
    drawList->AddPolyline(pts.data(), static_cast<int>(pts.size()), col, ImDrawFlags_Closed, thickness);
}

static void drawArc(ImDrawList* drawList, const ImVec2& center, float radius, float a_min, float a_max, ImU32 col, int num_segments = 16, float thickness = 1.0f) {
    std::vector<ImVec2> pts;
    pts.reserve(num_segments + 1);
    for (int i = 0; i <= num_segments; ++i) {
        float a = a_min + (static_cast<float>(i) / static_cast<float>(num_segments)) * (a_max - a_min);
        pts.push_back(ImVec2(center.x + cosf(a) * radius, center.y + sinf(a) * radius));
    }
    drawList->AddPolyline(pts.data(), static_cast<int>(pts.size()), col, ImDrawFlags_None, thickness);
}

// ПРОЦЕДУРНЫЕ АНИМИРОВАННЫЕ МОДЕЛИ БОЯ (ГЕРОИ И ПРОТИВНИКИ)
// =============================================================================

void ScreenManager::renderAnimatedHeroModel(ImDrawList* drawList, const ImVec2& center, const std::string& charId, bool isActive, float animTime, float scale) {
    float bob = sinf(animTime * 3.5f + (isActive ? 0.0f : 1.2f)) * 4.5f * scale;
    float breath = sinf(animTime * 2.5f) * 1.5f * scale;
    ImVec2 pos = ImVec2(center.x, center.y + bob);

    // 1. Тень под ногами
    drawEllipseFilled(drawList, ImVec2(center.x, center.y + 40.0f * scale), 26.0f * scale, 9.0f * scale, IM_COL32(0, 0, 0, 115));

    // 2. Свечение активного героя
    if (isActive) {
        float p = sinf(animTime * 4.0f) * 0.25f + 0.75f;
        drawList->AddCircle(ImVec2(center.x, center.y + 40.0f * scale), 30.0f * scale, IM_COL32(255, 215, 60, (int)(160 * p)), 32, 2.5f);
        drawList->AddCircleFilled(ImVec2(pos.x, pos.y), 34.0f * scale, IM_COL32(255, 220, 80, (int)(35 * p)), 24);
    }

    // 3. Индивидуальные цвета и детали для ВСЕХ 20 ПЕРСОНАЖЕЙ
    ImU32 bodyCol = IM_COL32(65, 75, 95, 255);
    ImU32 trimCol = IM_COL32(180, 195, 225, 255);
    ImU32 eyeCol  = IM_COL32(255, 225, 100, 255);

    if (charId == "adeir") {
        bodyCol = IM_COL32(150, 40, 25, 255); trimCol = IM_COL32(255, 140, 40, 255); eyeCol = IM_COL32(255, 210, 50, 255);
    } else if (charId == "renwick") {
        bodyCol = IM_COL32(45, 65, 110, 255); trimCol = IM_COL32(160, 210, 255, 255); eyeCol = IM_COL32(120, 220, 255, 255);
    } else if (charId == "garosh") {
        bodyCol = IM_COL32(50, 50, 56, 255); trimCol = IM_COL32(195, 195, 205, 255); eyeCol = IM_COL32(255, 110, 45, 255);
    } else if (charId == "vex") {
        bodyCol = IM_COL32(28, 75, 40, 255); trimCol = IM_COL32(100, 230, 130, 255); eyeCol = IM_COL32(140, 255, 180, 255);
    } else if (charId == "kaelim") {
        bodyCol = IM_COL32(40, 18, 52, 255); trimCol = IM_COL32(210, 90, 255, 255); eyeCol = IM_COL32(230, 130, 255, 255);
    } else if (charId == "velari") {
        bodyCol = IM_COL32(30, 70, 75, 255); trimCol = IM_COL32(100, 245, 225, 255); eyeCol = IM_COL32(160, 255, 245, 255);
    } else if (charId == "tauros") {
        bodyCol = IM_COL32(75, 20, 25, 255); trimCol = IM_COL32(255, 80, 80, 255); eyeCol = IM_COL32(255, 230, 80, 255);
    } else if (charId == "lumien") {
        bodyCol = IM_COL32(70, 62, 25, 255); trimCol = IM_COL32(255, 225, 80, 255); eyeCol = IM_COL32(255, 245, 150, 255);
    } else if (charId == "silas") {
        bodyCol = IM_COL32(32, 18, 42, 255); trimCol = IM_COL32(160, 80, 240, 255); eyeCol = IM_COL32(70, 245, 130, 255);
    } else if (charId == "finn") {
        bodyCol = IM_COL32(60, 45, 25, 255); trimCol = IM_COL32(240, 180, 75, 255); eyeCol = IM_COL32(255, 220, 120, 255);
    } else if (charId == "elix") {
        bodyCol = IM_COL32(25, 60, 50, 255); trimCol = IM_COL32(85, 240, 170, 255); eyeCol = IM_COL32(120, 255, 200, 255);
    } else if (charId == "tork") {
        bodyCol = IM_COL32(60, 42, 28, 255); trimCol = IM_COL32(240, 145, 55, 255); eyeCol = IM_COL32(255, 180, 70, 255);
    } else if (charId == "mira") {
        bodyCol = IM_COL32(25, 45, 75, 255); trimCol = IM_COL32(120, 220, 255, 255); eyeCol = IM_COL32(180, 240, 255, 255);
    } else if (charId == "roland") {
        bodyCol = IM_COL32(50, 32, 60, 255); trimCol = IM_COL32(225, 155, 250, 255); eyeCol = IM_COL32(245, 180, 255, 255);
    } else if (charId == "born") {
        bodyCol = IM_COL32(55, 45, 32, 255); trimCol = IM_COL32(210, 170, 100, 255); eyeCol = IM_COL32(255, 200, 80, 255);
    } else if (charId == "mortis") {
        bodyCol = IM_COL32(26, 22, 34, 255); trimCol = IM_COL32(150, 65, 195, 255); eyeCol = IM_COL32(70, 220, 255, 255);
    } else if (charId == "siegfried") {
        bodyCol = IM_COL32(65, 30, 22, 255); trimCol = IM_COL32(255, 145, 60, 255); eyeCol = IM_COL32(255, 215, 80, 255);
    } else if (charId == "sylvana") {
        bodyCol = IM_COL32(25, 55, 45, 255); trimCol = IM_COL32(115, 240, 190, 255); eyeCol = IM_COL32(160, 255, 220, 255);
    } else if (charId == "drake") {
        bodyCol = IM_COL32(32, 45, 60, 255); trimCol = IM_COL32(105, 190, 250, 255); eyeCol = IM_COL32(255, 215, 90, 255);
    } else if (charId == "cormac") {
        bodyCol = IM_COL32(32, 52, 28, 255); trimCol = IM_COL32(150, 230, 90, 255); eyeCol = IM_COL32(190, 255, 120, 255);
    }

    // 4. Силуэт тела (Мантия / Кираса / Ловкий костюм)
    bool isRobe = (charId == "adeir" || charId == "mira" || charId == "silas" || charId == "velari" || charId == "elix" || charId == "cormac");
    bool isHeavy = (charId == "garosh" || charId == "tauros" || charId == "born" || charId == "mortis" || charId == "lumien" || charId == "siegfried");

    if (isRobe) {
        drawList->AddTriangleFilled(
            ImVec2(pos.x, pos.y - 18.0f * scale),
            ImVec2(pos.x - (19.0f + breath) * scale, pos.y + 36.0f * scale),
            ImVec2(pos.x + (19.0f + breath) * scale, pos.y + 36.0f * scale),
            bodyCol);
        drawList->AddTriangle(
            ImVec2(pos.x, pos.y - 18.0f * scale),
            ImVec2(pos.x - (19.0f + breath) * scale, pos.y + 36.0f * scale),
            ImVec2(pos.x + (19.0f + breath) * scale, pos.y + 36.0f * scale),
            trimCol, 2.0f);
    } else if (isHeavy) {
        drawList->AddRectFilled(
            ImVec2(pos.x - (23.0f + breath) * scale, pos.y - 16.0f * scale),
            ImVec2(pos.x + (23.0f + breath) * scale, pos.y + 34.0f * scale),
            bodyCol, 6.0f * scale);
        drawList->AddRect(
            ImVec2(pos.x - (23.0f + breath) * scale, pos.y - 16.0f * scale),
            ImVec2(pos.x + (23.0f + breath) * scale, pos.y + 34.0f * scale),
            trimCol, 6.0f * scale, 0, 2.5f);
    } else {
        drawList->AddQuadFilled(
            ImVec2(pos.x - 16.0f * scale, pos.y - 16.0f * scale),
            ImVec2(pos.x + 16.0f * scale, pos.y - 16.0f * scale),
            ImVec2(pos.x + (18.0f + breath) * scale, pos.y + 32.0f * scale),
            ImVec2(pos.x - (18.0f + breath) * scale, pos.y + 32.0f * scale),
            bodyCol);
        drawList->AddQuad(
            ImVec2(pos.x - 16.0f * scale, pos.y - 16.0f * scale),
            ImVec2(pos.x + 16.0f * scale, pos.y - 16.0f * scale),
            ImVec2(pos.x + (18.0f + breath) * scale, pos.y + 32.0f * scale),
            ImVec2(pos.x - (18.0f + breath) * scale, pos.y + 32.0f * scale),
            trimCol, 2.0f);
    }

    // 5. Голова
    drawList->AddCircleFilled(ImVec2(pos.x, pos.y - 26.0f * scale), 12.0f * scale, bodyCol);
    drawList->AddCircle(ImVec2(pos.x, pos.y - 26.0f * scale), 12.0f * scale, trimCol, 24, 2.0f);

    // Глаза
    drawEllipseFilled(drawList, ImVec2(pos.x + 4.0f * scale, pos.y - 27.0f * scale), 3.0f * scale, 2.0f * scale, eyeCol);
    drawEllipseFilled(drawList, ImVec2(pos.x + 9.0f * scale, pos.y - 27.0f * scale), 3.0f * scale, 2.0f * scale, eyeCol);

    // 6. УНИКАЛЬНЫЕ АКСЕССУАРЫ ДЛЯ КАЖДОГО ПЕРСОНАЖА
    if (charId == "adeir") {
        // Посох огня со сферой и искрами
        float sBob = sinf(animTime * 4.0f) * 3.0f * scale;
        ImVec2 sTop = ImVec2(pos.x + 24.0f * scale, pos.y - (28.0f + sBob) * scale);
        drawList->AddLine(sTop, ImVec2(pos.x + 18.0f * scale, pos.y + 36.0f * scale), IM_COL32(140, 95, 55, 255), 3.0f * scale);
        drawList->AddCircleFilled(sTop, 7.5f * scale, IM_COL32(255, 120, 30, 255));
        drawList->AddCircle(sTop, 9.5f * scale, IM_COL32(255, 230, 100, 200), 16, 1.5f);
    } else if (charId == "renwick") {
        // Плюмаж на шлеме и широкий меч
        drawList->AddLine(ImVec2(pos.x, pos.y - 38.0f * scale), ImVec2(pos.x - 8.0f * scale, pos.y - 48.0f * scale), IM_COL32(70, 140, 255, 255), 4.0f * scale);
        drawList->AddLine(ImVec2(pos.x + 18.0f * scale, pos.y + 10.0f * scale), ImVec2(pos.x + 28.0f * scale, pos.y - 26.0f * scale), IM_COL32(220, 235, 255, 255), 3.5f * scale);
    } else if (charId == "garosh") {
        // Огромный ростовой щит с шипами
        ImVec2 sC = ImVec2(pos.x - 24.0f * scale, pos.y + 8.0f * scale);
        drawList->AddRectFilled(ImVec2(sC.x - 10.0f * scale, sC.y - 24.0f * scale), ImVec2(sC.x + 10.0f * scale, sC.y + 24.0f * scale), IM_COL32(40, 45, 55, 255), 4.0f * scale);
        drawList->AddRect(ImVec2(sC.x - 10.0f * scale, sC.y - 24.0f * scale), ImVec2(sC.x + 10.0f * scale, sC.y + 24.0f * scale), trimCol, 4.0f * scale, 0, 2.0f);
        drawList->AddTriangleFilled(ImVec2(sC.x - 10.0f * scale, sC.y), ImVec2(sC.x - 18.0f * scale, sC.y - 4.0f * scale), ImVec2(sC.x - 10.0f * scale, sC.y - 8.0f * scale), trimCol);
    } else if (charId == "vex") {
        // Изогнутый лук и колчан
        drawList->AddLine(ImVec2(pos.x - 16.0f * scale, pos.y - 12.0f * scale), ImVec2(pos.x - 24.0f * scale, pos.y - 32.0f * scale), IM_COL32(140, 100, 60, 255), 4.0f * scale);
        drawList->AddLine(ImVec2(pos.x + 20.0f * scale, pos.y - 24.0f * scale), ImVec2(pos.x + 28.0f * scale, pos.y + 24.0f * scale), IM_COL32(160, 110, 50, 255), 3.0f * scale);
    } else if (charId == "kaelim") {
        // Парные кинжалы с фиолетовым ядом
        drawList->AddLine(ImVec2(pos.x + 14.0f * scale, pos.y + 4.0f * scale), ImVec2(pos.x + 26.0f * scale, pos.y + 26.0f * scale), IM_COL32(200, 90, 255, 255), 3.0f * scale);
        drawList->AddLine(ImVec2(pos.x - 14.0f * scale, pos.y + 4.0f * scale), ImVec2(pos.x - 26.0f * scale, pos.y + 26.0f * scale), IM_COL32(200, 90, 255, 255), 3.0f * scale);
    } else if (charId == "velari") {
        // Золотой парящий нимб и кадуцей
        drawEllipse(drawList, ImVec2(pos.x, pos.y - 42.0f * scale), 12.0f * scale, 4.0f * scale, IM_COL32(255, 235, 100, 255), 16, 2.0f);
        drawList->AddLine(ImVec2(pos.x + 22.0f * scale, pos.y - 24.0f * scale), ImVec2(pos.x + 18.0f * scale, pos.y + 36.0f * scale), IM_COL32(220, 200, 100, 255), 3.0f * scale);
        drawList->AddCircleFilled(ImVec2(pos.x + 22.0f * scale, pos.y - 24.0f * scale), 5.0f * scale, IM_COL32(100, 245, 225, 255));
    } else if (charId == "tauros") {
        // Рога минотавра и тяжелые боевые секиры
        drawList->AddLine(ImVec2(pos.x - 12.0f * scale, pos.y - 30.0f * scale), ImVec2(pos.x - 26.0f * scale, pos.y - 44.0f * scale), IM_COL32(240, 230, 200, 255), 4.5f * scale);
        drawList->AddLine(ImVec2(pos.x + 12.0f * scale, pos.y - 30.0f * scale), ImVec2(pos.x + 26.0f * scale, pos.y - 44.0f * scale), IM_COL32(240, 230, 200, 255), 4.5f * scale);
        drawList->AddLine(ImVec2(pos.x + 20.0f * scale, pos.y + 12.0f * scale), ImVec2(pos.x + 30.0f * scale, pos.y - 22.0f * scale), IM_COL32(230, 70, 70, 255), 4.0f * scale);
    } else if (charId == "lumien") {
        // Солнечный молот света
        ImVec2 hHead = ImVec2(pos.x + 26.0f * scale, pos.y - 22.0f * scale);
        drawList->AddLine(ImVec2(pos.x + 16.0f * scale, pos.y + 24.0f * scale), hHead, IM_COL32(200, 160, 50, 255), 3.5f * scale);
        drawList->AddRectFilled(ImVec2(hHead.x - 8.0f * scale, hHead.y - 8.0f * scale), ImVec2(hHead.x + 8.0f * scale, hHead.y + 8.0f * scale), IM_COL32(255, 225, 80, 255), 3.0f * scale);
    } else if (charId == "silas") {
        // Парящий зеленый череп-фамильяр
        float sF = sinf(animTime * 4.5f) * 4.0f * scale;
        ImVec2 skP = ImVec2(pos.x - 24.0f * scale, pos.y - (34.0f + sF) * scale);
        drawList->AddCircleFilled(skP, 6.0f * scale, IM_COL32(60, 240, 130, 230));
        drawList->AddCircle(skP, 8.0f * scale, IM_COL32(180, 255, 200, 180), 16, 1.5f);
    } else if (charId == "finn") {
        // Шляпа с золотым пером и шпага
        drawList->AddLine(ImVec2(pos.x - 6.0f * scale, pos.y - 36.0f * scale), ImVec2(pos.x - 18.0f * scale, pos.y - 50.0f * scale), IM_COL32(255, 215, 60, 255), 3.5f * scale);
        drawList->AddLine(ImVec2(pos.x + 14.0f * scale, pos.y + 8.0f * scale), ImVec2(pos.x + 32.0f * scale, pos.y - 12.0f * scale), IM_COL32(220, 230, 240, 255), 2.5f * scale);
    } else if (charId == "elix") {
        // Очки на лбу и колба с зельем
        drawList->AddCircleFilled(ImVec2(pos.x + 22.0f * scale, pos.y + 8.0f * scale), 6.5f * scale, IM_COL32(85, 240, 170, 255));
        drawList->AddCircle(ImVec2(pos.x + 22.0f * scale, pos.y + 8.0f * scale), 8.5f * scale, IM_COL32(240, 255, 250, 200), 16, 1.5f);
    } else if (charId == "tork") {
        // Паровой ранец и латунный гаечный ключ
        drawList->AddRectFilled(ImVec2(pos.x - 26.0f * scale, pos.y - 12.0f * scale), ImVec2(pos.x - 16.0f * scale, pos.y + 16.0f * scale), IM_COL32(180, 110, 40, 255), 2.0f * scale);
        drawList->AddLine(ImVec2(pos.x + 16.0f * scale, pos.y + 12.0f * scale), ImVec2(pos.x + 28.0f * scale, pos.y - 18.0f * scale), IM_COL32(220, 160, 50, 255), 4.0f * scale);
    } else if (charId == "mira") {
        // Ледяная тиара и парящие ледяные осколки
        drawList->AddTriangleFilled(ImVec2(pos.x, pos.y - 42.0f * scale), ImVec2(pos.x - 8.0f * scale, pos.y - 34.0f * scale), ImVec2(pos.x + 8.0f * scale, pos.y - 34.0f * scale), IM_COL32(160, 235, 255, 255));
        float iO = sinf(animTime * 3.0f) * 6.0f * scale;
        drawList->AddCircleFilled(ImVec2(pos.x + 26.0f * scale, pos.y - (10.0f + iO) * scale), 4.0f * scale, IM_COL32(140, 225, 255, 255));
    } else if (charId == "roland") {
        // Красная роза на лацкане и рапира
        drawList->AddCircleFilled(ImVec2(pos.x - 6.0f * scale, pos.y + 4.0f * scale), 3.5f * scale, IM_COL32(220, 30, 60, 255));
        drawList->AddLine(ImVec2(pos.x + 14.0f * scale, pos.y + 8.0f * scale), ImVec2(pos.x + 30.0f * scale, pos.y - 18.0f * scale), IM_COL32(230, 235, 245, 255), 2.5f * scale);
    } else if (charId == "born") {
        // Парящие каменные глыбы над плечами
        drawList->AddRectFilled(ImVec2(pos.x - 28.0f * scale, pos.y - 14.0f * scale), ImVec2(pos.x - 18.0f * scale, pos.y - 4.0f * scale), IM_COL32(140, 120, 95, 255), 2.0f * scale);
        drawList->AddRectFilled(ImVec2(pos.x + 18.0f * scale, pos.y - 14.0f * scale), ImVec2(pos.x + 28.0f * scale, pos.y - 4.0f * scale), IM_COL32(140, 120, 95, 255), 2.0f * scale);
    } else if (charId == "mortis") {
        // Рунический темный двуручник со светящимися рунами
        drawList->AddLine(ImVec2(pos.x + 18.0f * scale, pos.y + 14.0f * scale), ImVec2(pos.x + 30.0f * scale, pos.y - 28.0f * scale), IM_COL32(40, 30, 50, 255), 4.5f * scale);
        drawList->AddLine(ImVec2(pos.x + 20.0f * scale, pos.y + 8.0f * scale), ImVec2(pos.x + 28.0f * scale, pos.y - 22.0f * scale), IM_COL32(80, 220, 255, 255), 2.0f * scale);
    } else if (charId == "siegfried") {
        // Драконьи крылья-наплечники
        drawList->AddTriangleFilled(ImVec2(pos.x - 22.0f * scale, pos.y - 10.0f * scale), ImVec2(pos.x - 34.0f * scale, pos.y - 24.0f * scale), ImVec2(pos.x - 18.0f * scale, pos.y - 2.0f * scale), IM_COL32(220, 60, 40, 255));
        drawList->AddLine(ImVec2(pos.x + 18.0f * scale, pos.y + 12.0f * scale), ImVec2(pos.x + 30.0f * scale, pos.y - 26.0f * scale), IM_COL32(255, 180, 70, 255), 4.0f * scale);
    } else if (charId == "sylvana") {
        // Ветряные серпы
        drawArc(drawList, ImVec2(pos.x + 22.0f * scale, pos.y + 8.0f * scale), 10.0f * scale, 3.14f, 6.28f, IM_COL32(115, 240, 190, 255), 12, 3.0f * scale);
    } else if (charId == "drake") {
        // Повязка на глазу и абордажный палаш
        drawList->AddLine(ImVec2(pos.x + 16.0f * scale, pos.y + 8.0f * scale), ImVec2(pos.x + 30.0f * scale, pos.y - 16.0f * scale), IM_COL32(210, 225, 240, 255), 3.5f * scale);
    } else if (charId == "cormac") {
        // Оленьи рога и живой посох с листвой
        drawList->AddLine(ImVec2(pos.x - 10.0f * scale, pos.y - 32.0f * scale), ImVec2(pos.x - 24.0f * scale, pos.y - 48.0f * scale), IM_COL32(160, 110, 50, 255), 3.5f * scale);
        drawList->AddLine(ImVec2(pos.x + 10.0f * scale, pos.y - 32.0f * scale), ImVec2(pos.x + 24.0f * scale, pos.y - 48.0f * scale), IM_COL32(160, 110, 50, 255), 3.5f * scale);
        drawList->AddLine(ImVec2(pos.x + 22.0f * scale, pos.y - 26.0f * scale), ImVec2(pos.x + 18.0f * scale, pos.y + 36.0f * scale), IM_COL32(130, 90, 45, 255), 3.5f * scale);
        drawList->AddCircleFilled(ImVec2(pos.x + 22.0f * scale, pos.y - 26.0f * scale), 4.5f * scale, IM_COL32(120, 240, 80, 255));
    }
}

void ScreenManager::renderAnimatedEnemyModel(ImDrawList* drawList, const ImVec2& center, const CombatEnemy& enemy, bool isTarget, float animTime, float scale) {
    float bob = sinf(animTime * 2.8f + 1.2f) * 5.0f * scale;
    float breath = sinf(animTime * 2.0f) * 2.0f * scale;
    ImVec2 pos = ImVec2(center.x, center.y + bob);

    // 1. Тень монстра
    float rShadow = (enemy.rank == EnemyRank::Boss ? 42.0f : enemy.rank == EnemyRank::Elite ? 32.0f : 24.0f) * scale;
    drawEllipseFilled(drawList, ImVec2(center.x, center.y + 40.0f * scale), rShadow, 10.0f * scale, IM_COL32(0, 0, 0, 120));

    // 2. Индикатор цели атаки (Targeting reticle)
    if (isTarget) {
        float p = sinf(animTime * 5.0f) * 0.3f + 0.7f;
        drawList->AddCircle(ImVec2(center.x, center.y + 40.0f * scale), rShadow + 6.0f, IM_COL32(255, 45, 45, (int)(220 * p)), 32, 2.5f);
        drawList->AddCircle(ImVec2(pos.x, pos.y), 34.0f * scale, IM_COL32(255, 50, 50, (int)(40 * p)), 24);
    }

    // 3. Аура ранга (Элита / Босс)
    if (enemy.rank == EnemyRank::Elite) {
        float p = sinf(animTime * 4.0f) * 0.3f + 0.7f;
        drawList->AddCircle(ImVec2(pos.x, pos.y), 38.0f * scale, IM_COL32(215, 60, 255, (int)(110 * p)), 24, 2.0f);
    } else if (enemy.rank == EnemyRank::Boss) {
        float p = sinf(animTime * 5.0f) * 0.4f + 0.6f;
        drawList->AddCircle(ImVec2(pos.x, pos.y), 48.0f * scale, IM_COL32(255, 40, 40, (int)(140 * p)), 24, 3.0f);
    }

    // 4. Тело монстра
    ImU32 monsterCol = (enemy.rank == EnemyRank::Boss) ? IM_COL32(80, 20, 25, 255) :
                       (enemy.rank == EnemyRank::Elite) ? IM_COL32(70, 25, 90, 255) :
                                                          IM_COL32(40, 42, 48, 255);
    ImU32 outlineCol = (enemy.rank == EnemyRank::Boss) ? IM_COL32(255, 60, 50, 255) :
                       (enemy.rank == EnemyRank::Elite) ? IM_COL32(255, 215, 60, 255) :
                                                          IM_COL32(160, 165, 180, 255);

    // Torso / Silhouette
    float bodyW = (enemy.rank == EnemyRank::Boss ? 30.0f : enemy.rank == EnemyRank::Elite ? 24.0f : 18.0f) + breath;
    drawList->AddRectFilled(ImVec2(pos.x - bodyW * scale, pos.y - 18.0f * scale),
                           ImVec2(pos.x + bodyW * scale, pos.y + 34.0f * scale),
                           monsterCol, 8.0f * scale);
    drawList->AddRect(ImVec2(pos.x - bodyW * scale, pos.y - 18.0f * scale),
                      ImVec2(pos.x + bodyW * scale, pos.y + 34.0f * scale),
                      outlineCol, 8.0f * scale, 0, 2.0f);

    // Голова
    float headR = (enemy.rank == EnemyRank::Boss ? 16.0f : enemy.rank == EnemyRank::Elite ? 13.0f : 11.0f) * scale;
    drawList->AddCircleFilled(ImVec2(pos.x, pos.y - 28.0f * scale), headR, monsterCol);
    drawList->AddCircle(ImVec2(pos.x, pos.y - 28.0f * scale), headR, outlineCol, 24, 2.0f);

    // Рога / Шипы для элиты и босса
    if (enemy.rank == EnemyRank::Boss || enemy.rank == EnemyRank::Elite) {
        ImVec2 hLeft = ImVec2(pos.x - (headR + 10.0f * scale), pos.y - (38.0f * scale));
        ImVec2 hRight = ImVec2(pos.x + (headR + 10.0f * scale), pos.y - (38.0f * scale));
        drawList->AddTriangleFilled(ImVec2(pos.x - headR * 0.6f, pos.y - 32.0f * scale),
                                   hLeft,
                                   ImVec2(pos.x - headR * 0.2f, pos.y - 24.0f * scale),
                                   outlineCol);
        drawList->AddTriangleFilled(ImVec2(pos.x + headR * 0.6f, pos.y - 32.0f * scale),
                                   hRight,
                                   ImVec2(pos.x + headR * 0.2f, pos.y - 24.0f * scale),
                                   outlineCol);
    }

    // Зловещие светящиеся глаза
    ImU32 eyeCol = (enemy.rank == EnemyRank::Boss) ? IM_COL32(255, 30, 30, 255) :
                   (enemy.rank == EnemyRank::Elite) ? IM_COL32(230, 70, 255, 255) :
                                                      IM_COL32(255, 200, 50, 255);
    drawEllipseFilled(drawList, ImVec2(pos.x - 5.0f * scale, pos.y - 29.0f * scale), 3.0f * scale, 2.0f * scale, eyeCol);
    drawEllipseFilled(drawList, ImVec2(pos.x - 10.0f * scale, pos.y - 29.0f * scale), 3.0f * scale, 2.0f * scale, eyeCol);
}

void ScreenManager::renderCombatVisualEffects(ImDrawList* drawList, const ImVec2& arenaPos, float arenaW, float arenaH) {
    if (!combatAnim.isPlaying) return;

    combatAnim.timer += ImGui::GetIO().DeltaTime;
    float progress = std::clamp(combatAnim.timer / combatAnim.duration, 0.0f, 1.0f);

    float heroStartX = arenaPos.x + 80.0f;
    float heroStepX = 110.0f;
    float heroBaseY = arenaPos.y + arenaH * 0.50f;

    float enemyStartX = arenaPos.x + arenaW - 90.0f;
    float enemyStepX = 120.0f;
    float enemyBaseY = arenaPos.y + arenaH * 0.50f;

    size_t hIdx = std::min(combatAnim.heroIndex, combatSquad.size() - 1);
    size_t eIdx = std::min(combatAnim.targetIndex, combatEnemies.size() - 1);

    ImVec2 heroPos = ImVec2(heroStartX + hIdx * heroStepX, heroBaseY);
    ImVec2 enemyPos = ImVec2(enemyStartX - eIdx * enemyStepX, enemyBaseY);

    // 1. АНИМАЦИЯ АТАКИ ГЕРОЯ ПО ПЕРСОНАЖУ
    const std::string& cid = combatAnim.attackerId;
    size_t sIdx = combatAnim.skillIndex;

    if (cid == "adeir") {
        // Огненный шар или огненный смерч
        float projX = heroPos.x + (enemyPos.x - heroPos.x) * progress;
        float projY = heroPos.y + (enemyPos.y - heroPos.y) * progress - sinf(progress * 3.1415f) * 40.0f;
        drawList->AddCircleFilled(ImVec2(projX, projY), 14.0f * (1.0f + (sIdx == 2 ? 1.0f : 0.0f)), IM_COL32(255, 110, 30, 240));
        drawList->AddCircleFilled(ImVec2(projX, projY), 8.0f, IM_COL32(255, 230, 90, 255));
        if (progress > 0.45f) {
            drawList->AddCircle(enemyPos, 45.0f * progress, IM_COL32(255, 120, 30, (int)(255 * (1.0f - progress))), 24, 4.0f);
        }
    } else if (cid == "renwick" || cid == "roland") {
        // Сияющий серебряный рассекающий удар мечом
        if (progress >= 0.35f && progress <= 0.85f) {
            float aProg = (progress - 0.35f) / 0.5f;
            ImVec2 p1 = ImVec2(enemyPos.x - 30.0f, enemyPos.y - 45.0f + aProg * 80.0f);
            ImVec2 p2 = ImVec2(enemyPos.x + 30.0f, enemyPos.y + 45.0f - aProg * 80.0f);
            drawList->AddLine(p1, p2, IM_COL32(180, 230, 255, 255), 5.0f);
            drawList->AddLine(ImVec2(p1.x - 4, p1.y), ImVec2(p2.x + 4, p2.y), IM_COL32(255, 255, 255, 255), 2.5f);
        }
    } else if (cid == "garosh" || cid == "tauros") {
        // Земляной удар и кровавый раскол
        if (progress >= 0.4f) {
            drawList->AddCircle(enemyPos, 50.0f * (progress - 0.4f) * 1.8f, IM_COL32(255, 60, 40, (int)(255 * (1.0f - progress))), 16, 4.0f);
            drawList->AddLine(ImVec2(enemyPos.x - 35.0f, enemyPos.y + 35.0f), ImVec2(enemyPos.x + 35.0f, enemyPos.y + 35.0f), IM_COL32(240, 160, 60, 220), 4.0f);
        }
    } else if (cid == "vex" || cid == "sylvana") {
        // Свистящая стрела с ветряным шлейфом
        float aX = heroPos.x + (enemyPos.x - heroPos.x) * progress;
        float aY = heroPos.y + (enemyPos.y - heroPos.y) * progress;
        drawList->AddLine(ImVec2(aX - 25.0f, aY), ImVec2(aX, aY), IM_COL32(120, 240, 150, 255), 3.5f);
        drawList->AddCircleFilled(ImVec2(aX, aY), 4.0f, IM_COL32(255, 255, 255, 255));
    } else if (cid == "kaelim") {
        // Фиолетовый крестообразный разрез из тени (X-Slash)
        if (progress >= 0.35f) {
            drawList->AddLine(ImVec2(enemyPos.x - 28.0f, enemyPos.y - 28.0f), ImVec2(enemyPos.x + 28.0f, enemyPos.y + 28.0f), IM_COL32(220, 90, 255, 255), 4.0f);
            drawList->AddLine(ImVec2(enemyPos.x + 28.0f, enemyPos.y - 28.0f), ImVec2(enemyPos.x - 28.0f, enemyPos.y + 28.0f), IM_COL32(220, 90, 255, 255), 4.0f);
        }
    } else if (cid == "velari" || cid == "lumien") {
        // Нисходящий столб небесного света
        if (progress >= 0.3f) {
            float bW = 28.0f * (1.0f - fabsf(progress - 0.6f) * 2.0f);
            drawList->AddRectFilled(ImVec2(enemyPos.x - bW, arenaPos.y + 10.0f), ImVec2(enemyPos.x + bW, enemyPos.y + 40.0f), IM_COL32(255, 240, 120, 180));
            drawList->AddCircleFilled(enemyPos, 35.0f, IM_COL32(255, 255, 200, 200));
        }
    } else if (cid == "silas" || cid == "mortis") {
        // Свистящие зеленые черепа пустоты
        float skX = heroPos.x + (enemyPos.x - heroPos.x) * progress;
        float skY = heroPos.y + (enemyPos.y - heroPos.y) * progress + sinf(progress * 12.0f) * 15.0f;
        drawList->AddCircleFilled(ImVec2(skX, skY), 10.0f, IM_COL32(60, 240, 130, 240));
        drawList->AddCircle(ImVec2(skX, skY), 14.0f, IM_COL32(180, 255, 200, 180), 16, 2.0f);
    } else if (cid == "mira") {
        // Ледяные копья и замораживающий взрыв
        float icX = heroPos.x + (enemyPos.x - heroPos.x) * progress;
        float icY = heroPos.y + (enemyPos.y - heroPos.y) * progress;
        drawList->AddLine(ImVec2(icX - 20.0f, icY), ImVec2(icX, icY), IM_COL32(140, 230, 255, 255), 4.5f);
        if (progress > 0.5f) {
            drawList->AddCircle(enemyPos, 40.0f * (progress - 0.5f) * 2.0f, IM_COL32(160, 240, 255, 220), 24, 3.0f);
        }
    } else {
        // Стандартный эффект удара
        if (progress >= 0.4f && progress <= 0.8f) {
            drawList->AddCircleFilled(enemyPos, 30.0f, IM_COL32(255, 220, 100, 180));
        }
    }

    // 2. ВСПЛЫВАЮЩИЙ ТЕКСТ УРОНА ПРИ ПОПАДАНИИ
    if (progress >= 0.45f) {
        float floatY = (progress - 0.45f) * 50.0f;
        std::string dmgText = (combatAnim.isCrit ? "[*] КРИТ! -" : "-") + std::to_string(static_cast<int>(combatAnim.damage)) + " HP";
        ImVec4 tColor = combatAnim.isCrit ? ImVec4(1.0f, 0.25f, 0.25f, 1.0f) : ImVec4(1.0f, 0.85f, 0.20f, 1.0f);
        drawList->AddText(ImVec2(enemyPos.x - 28.0f, enemyPos.y - 65.0f - floatY),
                          ImGui::ColorConvertFloat4ToU32(tColor), dmgText.c_str());
    }

    // 3. ПРИМЕНЕНИЕ УРОНА И ЗАВЕРШЕНИЕ
    if (progress >= 0.50f && !combatAnim.damageApplied) {
        combatAnim.damageApplied = true;
        if (combatAnim.targetIndex < combatEnemies.size()) {
            auto& target = combatEnemies[combatAnim.targetIndex];
            double netDmg = combatAnim.damage;
            if (target.shield > 0.0) {
                double absS = std::min(target.shield, netDmg);
                target.shield -= absS;
                netDmg -= absS;
            }
            target.currentHp = std::max(0.0, target.currentHp - netDmg);

            // Вампиризм атакующего
            if (combatAnim.heroIndex < combatSquad.size()) {
                auto& hero = combatSquad[combatAnim.heroIndex];
                if (hero.stats[StatIndex::Vampirism] > 0.0 && netDmg > 0.0) {
                    double healV = netDmg * hero.stats[StatIndex::Vampirism];
                    hero.currentHp = std::min(hero.maxHp, hero.currentHp + healV);
                }
            }

            std::string critPrefix = combatAnim.isCrit ? "[КРИТ!] " : "";
            ImVec4 logColor = combatAnim.isCrit ? ImVec4(1.0f, 0.45f, 0.1f, 1.0f) : ImVec4(0.95f, 0.95f, 0.35f, 1.0f);
            logCombat(critPrefix + combatSquad[hIdx].name + " наносит '" + combatAnim.skillName + "' по " + target.name + " (-" + std::to_string(static_cast<int>(netDmg)) + " HP)!", logColor);

            if (!target.isAlive()) {
                logCombat("[ПОБЕЖДЕН] " + target.name + " повержен!", ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
            }
        }
    }

    if (progress >= 1.0f) {
        combatAnim.isPlaying = false;

        // Проверка победы
        bool anyEnemyAlive = false;
        for (const auto& e : combatEnemies) {
            if (e.isAlive()) { anyEnemyAlive = true; break; }
        }

        if (!anyEnemyAlive) {
            combatPhase = CombatPhase::Victory;
            logCombat("=== ПОБЕДА! Все противники повержены! ===", ImVec4(0.3f, 1.0f, 0.3f, 1.0f));
            return;
        }

        // Если текущая цель пала, переключаем на следующего живого
        if (selectedEnemyTarget >= 0 && !combatEnemies[selectedEnemyTarget].isAlive()) {
            for (size_t i = 0; i < combatEnemies.size(); ++i) {
                if (combatEnemies[i].isAlive()) {
                    selectedEnemyTarget = static_cast<int>(i);
                    break;
                }
            }
        }

        executeEnemyTurn();
    }
}

void ScreenManager::renderCombatBattlefield(float width, float height) {
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.06f, 0.07f, 0.11f, 0.85f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.50f, 0.40f, 0.25f, 0.70f));
    ImGui::BeginChild("CombatArenaCanvas", ImVec2(width, height), true, ImGuiWindowFlags_NoScrollbar);

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 canvasPos = ImGui::GetCursorScreenPos();
    float animTime = static_cast<float>(ImGui::GetTime());

    // Разделительная черта центра поля боя
    float midX = canvasPos.x + width * 0.5f;
    drawList->AddLine(ImVec2(midX, canvasPos.y + 10.0f), ImVec2(midX, canvasPos.y + height - 10.0f),
                      IM_COL32(200, 160, 70, 70), 1.5f);

    // Заголовки сторон
    drawList->AddText(ImVec2(canvasPos.x + 30.0f, canvasPos.y + 8.0f), IM_COL32(100, 220, 140, 220), "[Атака] ОТРЯД ГЕРОЕВ");
    drawList->AddText(ImVec2(canvasPos.x + width - 190.0f, canvasPos.y + 8.0f), IM_COL32(255, 90, 90, 220), "[Босс] ВРАЖЕСКИЙ ОТРЯД");

    // Отрисовка отряда героев СЛЕВА (с учетом рывка атакующего)
    float heroStartX = canvasPos.x + 80.0f;
    float heroStepX = 110.0f;
    float heroBaseY = canvasPos.y + height * 0.50f;

    for (size_t i = 0; i < combatSquad.size(); ++i) {
        auto& hero = combatSquad[i];
        if (!hero.isAlive()) continue;
        bool isActive = (i == activeCharacterIndex);
        float hX = heroStartX + i * heroStepX + (isActive ? 25.0f : 0.0f);
        float hY = heroBaseY + (isActive ? 12.0f : -6.0f);

        // Рывок вперед во время анимации атаки
        if (combatAnim.isPlaying && combatAnim.isHero && combatAnim.heroIndex == i) {
            float prog = combatAnim.timer / combatAnim.duration;
            hX += sinf(prog * 3.1415f) * 120.0f;
        }

        renderAnimatedHeroModel(drawList, ImVec2(hX, hY), hero.id, isActive, animTime, isActive ? 1.25f : 1.0f);
    }

    // Отрисовка отряда врагов СПРАВА (с учетом вздрагивания при ударе)
    float enemyStartX = canvasPos.x + width - 90.0f;
    float enemyStepX = 120.0f;
    float enemyBaseY = canvasPos.y + height * 0.50f;

    for (size_t i = 0; i < combatEnemies.size(); ++i) {
        auto& enemy = combatEnemies[i];
        if (!enemy.isAlive()) continue;
        bool isTarget = (static_cast<int>(i) == selectedEnemyTarget);
        float eX = enemyStartX - i * enemyStepX - (isTarget ? 25.0f : 0.0f);
        float eY = enemyBaseY + (isTarget ? 12.0f : -6.0f);

        // Отдача/вздрагивание назад при получении урона
        if (combatAnim.isPlaying && combatAnim.targetIndex == i && combatAnim.timer >= combatAnim.duration * 0.45f) {
            float fProg = (combatAnim.timer - combatAnim.duration * 0.45f) / (combatAnim.duration * 0.55f);
            eX += sinf(fProg * 3.1415f) * 22.0f;
        }

        renderAnimatedEnemyModel(drawList, ImVec2(eX, eY), enemy, isTarget, animTime,
                                (enemy.rank == EnemyRank::Boss) ? 1.45f :
                                (enemy.rank == EnemyRank::Elite) ? 1.20f : 1.0f);
    }

    // Отрисовка летящих снарядов, магических волн и всплывающего урона
    renderCombatVisualEffects(drawList, canvasPos, width, height);

    ImGui::EndChild();
    ImGui::PopStyleColor(2);
}

void ScreenManager::renderCombat() {
    if (!isCombatInitialized) {
        initCombatState();
    }

    // --- ВЕРХНЯЯ СТРОКА: ЗАГОЛОВОК, РАУНД И СТАТУС ХОДА ---
    ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.25f, 1.0f), "=== ПОЛЕ БОЯ: %s ===", getFloorTitle(currentFloor).c_str());
    ImGui::SameLine(0.0f, 25.0f);
    ImGui::Text("Раунд: %d", combatTurnNumber);
    ImGui::SameLine(0.0f, 25.0f);

    switch (combatPhase) {
    case CombatPhase::PlayerTurn:
        ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.35f, 1.0f), "ВАШ ХОД (ИГРОК)");
        break;
    case CombatPhase::EnemyTurn:
        ImGui::TextColored(ImVec4(1.0f, 0.35f, 0.35f, 1.0f), "ХОД ПРОТИВНИКА");
        break;
    case CombatPhase::Victory:
        ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.4f, 1.0f), "ПОБЕДА!");
        break;
    case CombatPhase::Defeat:
        ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "ПОРАЖЕНИЕ");
        break;
    }

    float winWidth = ImGui::GetWindowWidth();
    if (winWidth > 300.0f) {
        ImGui::SameLine(winWidth - 210.0f);
    }
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.70f, 0.15f, 0.15f, 1.0f));
    if (ImGui::Button("<- Отступить на карту##CombatBack", ImVec2(190, 26))) {
        showCombatExitWarning = true;
    }
    ImGui::PopStyleColor();

    ImGui::Separator();
    ImGui::Spacing();

    // =========================================================================
    // АНИМИРОВАННАЯ БОЕВАЯ АРЕНА: МОДЕЛИ ГЕРОЕВ СЛЕВА И ПРОТИВНИКОВ СПРАВА
    // =========================================================================
    renderCombatBattlefield(ImGui::GetContentRegionAvail().x, 160.0f);
    ImGui::Spacing();

    // =========================================================================
    // ТРЕХКОЛОНОЧНЫЙ БЛОК:
    // КОЛОНКА 1 (СЛЕВА): ОТРЯД ИГРОКА
    // КОЛОНКА 2 (ПО ЦЕНТРУ): БЛОК С КНОПКАМИ УМЕНИЙ И ХАРАКТЕРИСТИКАМИ
    // КОЛОНКА 3 (СПРАВА): СПИСОК ПРОТИВНИКОВ
    // =========================================================================
    float totalWidth = ImGui::GetContentRegionAvail().x;
    float leftColW = 320.0f;
    float rightColW = 360.0f;
    float centerColW = totalWidth - leftColW - rightColW - 30.0f;

    ImGui::Columns(3, "CombatThreeColumns", false);
    ImGui::SetColumnWidth(0, leftColW);
    ImGui::SetColumnWidth(1, centerColW);
    ImGui::SetColumnWidth(2, rightColW);

    // =========================================================================
    // КОЛОНКА 1 (СЛЕВА): ОТРЯД ИГРОКА
    // =========================================================================
    ImGui::TextColored(ImVec4(0.4f, 0.9f, 0.5f, 1.0f), "ОТРЯД ИГРОКА:");
    ImGui::TextDisabled("Клавиши [1-3] для смены активного:");
    ImGui::Spacing();

    for (size_t i = 0; i < combatSquad.size(); ++i) {
        auto& hero = combatSquad[i];
        ImGui::PushID(static_cast<int>(i));

        bool isActive = (i == activeCharacterIndex);
        sf::Keyboard::Key heroKey = (i == 0) ? combatKeybinds.selectHero1 :
                                    (i == 1) ? combatKeybinds.selectHero2 : combatKeybinds.selectHero3;
        std::string keyBadge = "[" + getKeyDisplayName(heroKey) + "] ";

        // Карточка героя
        ImVec4 cardBorderColor = isActive ? ImVec4(0.95f, 0.80f, 0.25f, 0.95f) : ImVec4(0.40f, 0.45f, 0.55f, 0.50f);
        ImVec4 cardBgColor = isActive ? ImVec4(0.12f, 0.16f, 0.24f, 0.85f) : ImVec4(0.08f, 0.09f, 0.14f, 0.65f);

        ImGui::PushStyleColor(ImGuiCol_Border, cardBorderColor);
        ImGui::PushStyleColor(ImGuiCol_ChildBg, cardBgColor);
        ImGui::BeginChild("HeroCardBox", ImVec2(0, 115.0f), true, ImGuiWindowFlags_NoScrollbar);

        if (hero.isAlive()) {
            if (isActive) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.18f, 0.55f, 0.25f, 1.0f));
                ImGui::Button(("* " + keyBadge + hero.name + " (АКТИВЕН) *").c_str(), ImVec2(-1, 24));
                ImGui::PopStyleColor();
            } else {
                if (ImGui::Button((keyBadge + hero.name).c_str(), ImVec2(-1, 24))) {
                    if (combatPhase == CombatPhase::PlayerTurn) {
                        activeCharacterIndex = i;
                        logCombat("Вы переключились на бойца: " + hero.name, ImVec4(0.4f, 1.0f, 0.4f, 1.0f));
                    }
                }
            }
        } else {
            ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.2f, 1.0f), "%s (ПАЛ В БОЮ)", hero.name.c_str());
        }

        // Полоса здоровья
        float hpFrac = (hero.maxHp > 0.0) ? static_cast<float>(hero.currentHp / hero.maxHp) : 0.0f;
        hpFrac = std::clamp(hpFrac, 0.0f, 1.0f);

        ImVec4 barColor = ImVec4(0.2f, 0.8f, 0.25f, 1.0f);
        if (hpFrac < 0.25f) barColor = ImVec4(0.9f, 0.2f, 0.2f, 1.0f);
        else if (hpFrac < 0.5f) barColor = ImVec4(0.9f, 0.75f, 0.2f, 1.0f);

        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, barColor);
        ImGui::ProgressBar(hpFrac, ImVec2(-1, 14), "");
        ImGui::PopStyleColor();

        // HP и щит
        ImGui::Text("HP: %.0f / %.0f", hero.currentHp, hero.maxHp);
        if (hero.shield > 0.0) {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.3f, 0.85f, 1.0f, 1.0f), "(Щит: +%.0f)", hero.shield);
        }

        // Стаки героя
        ImVec4 stColor = (hero.skillStacks > 0) ? ImVec4(1.0f, 0.85f, 0.25f, 1.0f) : ImVec4(0.55f, 0.55f, 0.60f, 1.0f);
        ImGui::TextColored(stColor, "Стаки: %d", hero.skillStacks);
        if (hero.skillStacks > 0) {
            ImGui::SameLine();
            std::string pips = "";
            for (int p = 0; p < std::min(hero.skillStacks, 5); ++p) pips += "*";
            ImGui::TextColored(ImVec4(1.0f, 0.75f, 0.2f, 1.0f), "%s", pips.c_str());
        }

        ImGui::EndChild();
        ImGui::PopStyleColor(2); // Border, ChildBg

        ImGui::Spacing();
        ImGui::PopID();
    }

    ImGui::NextColumn();

    // =========================================================================
    // КОЛОНКА 2 (ПО ЦЕНТРУ): БЛОК С КНОПКАМИ УМЕНИЙ И ХАРАКТЕРИСТИКАМИ
    // =========================================================================
    if (activeCharacterIndex < combatSquad.size()) {
        auto& activeHero = combatSquad[activeCharacterIndex];
        auto& dbChars = GameDatabase::getInstance().characters;
        auto charIt = dbChars.find(activeHero.id);

        // Карточка пульта управления способностями
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.08f, 0.09f, 0.14f, 0.80f));
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.70f, 0.55f, 0.25f, 0.80f));
        ImGui::BeginChild("CenterCombatDeck", ImVec2(0, 360.0f), true);

        // Заголовок с именем активного героя и крупным бейджем стаков
        ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.3f, 1.0f), "АКТИВНЫЙ БОЕЦ: %s (Ур. %d)",
            activeHero.name.c_str(), activeHero.level);

        ImGui::SameLine(0.0f, 20.0f);
        ImVec4 stackBadgeColor = (activeHero.skillStacks > 0) ? ImVec4(1.0f, 0.85f, 0.20f, 1.0f) : ImVec4(0.65f, 0.65f, 0.70f, 1.0f);
        ImGui::TextColored(stackBadgeColor, "[ * КОЛИЧЕСТВО СТАКОВ: %d ]", activeHero.skillStacks);

        if (activeHero.skillStacks > 0) {
            ImGui::SameLine(0.0f, 8.0f);
            std::string pips = "";
            for (int p = 0; p < std::min(activeHero.skillStacks, 8); ++p) pips += "*";
            ImGui::TextColored(ImVec4(1.0f, 0.75f, 0.20f, 1.0f), "%s", pips.c_str());
        }

        ImGui::Separator();
        ImGui::Spacing();

        // КНОПКИ 3 УМЕНИЙ С ТУЛТИПАМИ ПРИ НАВЕДЕНИИ
        if (charIt != dbChars.end()) {
            const auto& skills = charIt->second->getSkills();
            float btnWidth = (ImGui::GetContentRegionAvail().x - 24.0f) / 3.0f;
            if (btnWidth < 140.0f) btnWidth = 140.0f;

            for (size_t sIdx = 0; sIdx < 3 && sIdx < skills.size(); ++sIdx) {
                const auto& skill = skills[sIdx];
                int cd = activeHero.skillCooldowns[sIdx];
                bool canUse = (combatPhase == CombatPhase::PlayerTurn) && (cd == 0) && activeHero.isAlive();

                sf::Keyboard::Key boundKey = (sIdx == 0) ? combatKeybinds.basicAttack :
                                             (sIdx == 1) ? combatKeybinds.skill : combatKeybinds.ultimate;
                std::string keyStr = "[" + getKeyDisplayName(boundKey) + "] ";

                std::string btnLabel = keyStr + skill->getName() + "\n";
                if (sIdx == 0) {
                    btnLabel += "(Атака | Готово)";
                } else if (sIdx == 1) {
                    if (cd > 0) btnLabel += "(Навык | КД: " + std::to_string(cd) + ")";
                    else btnLabel += "(Навык | Готово)";
                } else {
                    if (cd > 0) btnLabel += "(УЛЬТА | КД: " + std::to_string(cd) + ")";
                    else btnLabel += "(УЛЬТА | Готово)";
                }

                ImVec4 btnColor = (sIdx == 0) ? ImVec4(0.20f, 0.42f, 0.70f, 1.0f) :
                                  (sIdx == 1) ? ImVec4(0.42f, 0.22f, 0.70f, 1.0f) :
                                                ImVec4(0.72f, 0.22f, 0.22f, 1.0f);

                if (!canUse) ImGui::BeginDisabled(true);

                ImGui::PushStyleColor(ImGuiCol_Button, btnColor);
                if (ImGui::Button(btnLabel.c_str(), ImVec2(btnWidth, 54))) {
                    processPlayerSkill(sIdx);
                }
                ImGui::PopStyleColor();

                if (!canUse) ImGui::EndDisabled();

                // ПОКАЗ ОПИСАНИЯ УМЕНИЯ ПРИ НАВЕДЕНИИ МЫШИ (ДАЖЕ ЕСЛИ КНОПКА ОТКЛЮЧЕНА)
                if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                    ImGui::BeginTooltip();
                    ImGui::PushTextWrapPos(320.0f);
                    ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.3f, 1.0f), "%s", skill->getName().c_str());
                    ImGui::Separator();
                    std::string fullDesc = getSkillDetailedDescription(skill, activeHero, sIdx);
                    ImGui::TextUnformatted(fullDesc.c_str());
                    ImGui::PopTextWrapPos();
                    ImGui::EndTooltip();
                }

                if (sIdx < 2) {
                    ImGui::SameLine(0.0f, 10.0f);
                }
            }

            ImGui::Spacing();
            double previewVal0 = (skills.size() > 0) ? skills[0]->calculateValue(activeHero.stats, activeHero.skillStacks) : 0;
            double previewVal1 = (skills.size() > 1) ? skills[1]->calculateValue(activeHero.stats, activeHero.skillStacks) : 0;
            double previewVal2 = (skills.size() > 2) ? skills[2]->calculateValue(activeHero.stats, activeHero.skillStacks) : 0;

            std::string k0 = getKeyDisplayName(combatKeybinds.basicAttack);
            std::string k1 = getKeyDisplayName(combatKeybinds.skill);
            std::string k2 = getKeyDisplayName(combatKeybinds.ultimate);

            ImGui::TextColored(ImVec4(0.85f, 0.85f, 0.85f, 1.0f),
                "Сила умений: [%s] %.0f  |  [%s] %.0f  |  [%s] %.0f",
                k0.c_str(), previewVal0, k1.c_str(), previewVal1, k2.c_str(), previewVal2);

            ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.25f, 1.0f),
                "[*] Эффект стаков: %d (усиливает урон и защитные параметры)", activeHero.skillStacks);
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // БЛОК ХАРАКТЕРИСТИК АКТИВНОГО БОЙЦА
        ImGui::TextColored(ImVec4(0.4f, 0.85f, 1.0f, 1.0f), "ХАРАКТЕРИСТИКИ (%s):", activeHero.name.c_str());
        ImGui::Spacing();

        ImGui::Columns(2, "ActiveStatsSubCols", false);
        ImGui::Text("Здоровье: %.0f / %.0f", activeHero.currentHp, activeHero.maxHp);
        if (activeHero.shield > 0.0) {
            ImGui::TextColored(ImVec4(0.3f, 0.8f, 1.0f, 1.0f), "Щит: +%.0f", activeHero.shield);
        }
        ImGui::Text("Мана: %.0f / %.0f", activeHero.currentMana, activeHero.maxMana);
        ImGui::Text("Атака: %.1f", activeHero.stats[StatIndex::Attack]);
        ImGui::Text("Защита: %.1f", activeHero.stats[StatIndex::Defense]);

        ImGui::NextColumn();
        ImGui::Text("Крит. шанс: %.1f%%", activeHero.stats[StatIndex::CritChance] * 100.0);
        ImGui::Text("Крит. урон: %.0f%%", activeHero.stats[StatIndex::CritDamage] * 100.0);
        ImGui::Text("Вампиризм: %.1f%%", activeHero.stats[StatIndex::Vampirism] * 100.0);
        ImGui::Text("Бонус урона: +%.1f%%", activeHero.stats[StatIndex::DamageBonus] * 100.0);
        ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.25f, 1.0f), "* Стаки героя: %d", activeHero.skillStacks);
        ImGui::Columns(1);

        ImGui::EndChild();
        ImGui::PopStyleColor(2);
    }

    ImGui::NextColumn();

    // =========================================================================
    // КОЛОНКА 3 (СПРАВА): СПИСОК ПРОТИВНИКОВ
    // =========================================================================
    ImGui::TextColored(ImVec4(0.95f, 0.35f, 0.35f, 1.0f), "ПРОТИВНИКИ НА ПОЛЕ:");
    ImGui::TextDisabled("Кликните для выбора цели атаки:");
    ImGui::Spacing();

    for (size_t i = 0; i < combatEnemies.size(); ++i) {
        auto& enemy = combatEnemies[i];
        ImGui::PushID(static_cast<int>(100 + i));

        bool isTarget = (static_cast<int>(i) == selectedEnemyTarget);
        ImVec4 cardBorder = isTarget ? ImVec4(0.95f, 0.25f, 0.25f, 1.0f) : ImVec4(0.40f, 0.35f, 0.35f, 0.50f);
        ImVec4 cardBg = isTarget ? ImVec4(0.18f, 0.09f, 0.10f, 0.85f) : ImVec4(0.10f, 0.09f, 0.12f, 0.65f);

        ImGui::PushStyleColor(ImGuiCol_Border, cardBorder);
        ImGui::PushStyleColor(ImGuiCol_ChildBg, cardBg);
        ImGui::BeginChild("EnemyCardBox", ImVec2(0, 95.0f), true, ImGuiWindowFlags_NoScrollbar);

        ImVec4 rankColor = getEnemyRankColor(enemy.rank);
        ImGui::TextColored(rankColor, "[%s]", getEnemyRankName(enemy.rank));
        ImGui::SameLine();
        ImGui::Text("%s (Ур. %d)", enemy.name.c_str(), enemy.level);

        if (enemy.isAlive()) {
            ImGui::SameLine(rightColW - 145.0f);
            if (isTarget) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.85f, 0.2f, 0.2f, 1.0f));
                ImGui::Button("ТЕКУЩАЯ ЦЕЛЬ##TargetActive", ImVec2(120, 22));
                ImGui::PopStyleColor();
            } else {
                if (ImGui::Button("Выбрать цель##SetTarget", ImVec2(120, 22))) {
                    selectedEnemyTarget = static_cast<int>(i);
                }
            }

            float eHpFrac = (enemy.maxHp > 0.0) ? static_cast<float>(enemy.currentHp / enemy.maxHp) : 0.0f;
            eHpFrac = std::clamp(eHpFrac, 0.0f, 1.0f);
            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.85f, 0.25f, 0.25f, 1.0f));
            ImGui::ProgressBar(eHpFrac, ImVec2(-1, 13), "");
            ImGui::PopStyleColor();

            ImGui::TextColored(ImVec4(0.85f, 0.85f, 0.85f, 1.0f), "HP: %.0f/%.0f | АТК: %.0f | ЗАЩ: %.0f",
                enemy.currentHp, enemy.maxHp, enemy.attack, enemy.defense);

            // Описание навыков противников убрано по требованию пользователя
        } else {
            ImGui::SameLine(rightColW - 145.0f);
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "ПОБЕЖДЕН");
        }

        ImGui::EndChild();
        ImGui::PopStyleColor(2); // Border, ChildBg

        ImGui::Spacing();
        ImGui::PopID();
    }

    ImGui::Columns(1);

    // =========================================================================
    // НИЖНЯЯ ЗОНА: ЖУРНАЛ БОЯ В САМОМ НИЗУ ЭКРАНА
    // =========================================================================
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::TextColored(ImVec4(0.95f, 0.85f, 0.40f, 1.0f), "ЖУРНАЛ БОЯ:");
    ImGui::Spacing();

    float logHeight = 140.0f;
    ImGui::BeginChild("CombatLogScrollArea", ImVec2(0, logHeight), true);
    for (const auto& entry : combatLog) {
        ImGui::TextColored(entry.color, "%s", entry.message.c_str());
    }
    if (scrollToBottomCombatLog) {
        ImGui::SetScrollHereY(1.0f);
        scrollToBottomCombatLog = false;
    }
    ImGui::EndChild();

    // --- МОДАЛЬНОЕ ОКНО ПОБЕДЫ ---
    if (combatPhase == CombatPhase::Victory) {
        ImGui::OpenPopup("##CombatVictoryModal");
    }
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(360, 200));

    if (ImGui::BeginPopupModal("##CombatVictoryModal", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar)) {
        ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.4f, 1.0f), "=== ПОБЕДА НА ПОЛЕ БОЯ! ===");
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::Text("Все противники успешно повержены!");
        ImGui::Text("Полученное золото: +%d монет", combatRewardGold);
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("Забрать награду и вернуться на карту##VictoryOk", ImVec2(-1, 32))) {
            playerInventory.addFunds(combatRewardGold);
            isCombatInitialized = false;
            currentScreen = ScreenState::Map;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    // --- МОДАЛЬНОЕ ОКНО ПОРАЖЕНИЯ ---
    if (combatPhase == CombatPhase::Defeat) {
        ImGui::OpenPopup("##CombatDefeatModal");
    }
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(360, 180));

    if (ImGui::BeginPopupModal("##CombatDefeatModal", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar)) {
        ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "=== ВАШ ОТРЯД ПОВЕРЖЕН ===");
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::Text("Все бойцы пали в бою...");
        ImGui::TextDisabled("Попробуйте изменить тактику или улучшить снаряжение.");
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("Отступить в лагерь (Лобби)##DefeatOk", ImVec2(-1, 32))) {
            isCombatInitialized = false;
            currentScreen = ScreenState::Lobby;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    // --- ПРЕДУПРЕЖДЕНИЕ ПРИ ВЫХОДЕ ИЗ БОЯ ---
    if (showCombatExitWarning) {
        ImGui::OpenPopup("##CombatExitWarningModal");
    }
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("##CombatExitWarningModal", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar)) {
        ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "Внимание: побег из боя!");
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::Text("Вы уверены, что хотите отступить?\nПрогресс текущего сражения будет сброшен.");
        ImGui::Spacing();

        if (ImGui::Button("Да, отступить##ConfirmCombatExit", ImVec2(140, 28))) {
            showCombatExitWarning = false;
            isCombatInitialized = false;
            currentScreen = ScreenState::Map;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Продолжить бой##CancelCombatExit", ImVec2(140, 28))) {
            showCombatExitWarning = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void ScreenManager::renderCrafting() {
    std::string activeTabName = (currentCraftingTab == ItemTab::Materials) ? "Материалы" : "Оружие";

    ImGui::Text("=== МАСТЕРСКАЯ (СОЗДАНИЕ) | Вкладка: %s ===", activeTabName.c_str());
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.45f, 0.85f, 1.0f));
    if (ImGui::Button("<- На карту##CraftBack", ImVec2(150, 40))) {
        clearCraftingGrid();
        currentScreen = ScreenState::Map;
    }
    ImGui::PopStyleColor();

    ImGui::Spacing();
    ImGui::Separator();

    ImGui::Columns(2, "CraftingColumns", false);
    ImGui::SetColumnWidth(0, ImGui::GetWindowWidth() * 0.4f);

    ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Категория компонентов:");
    ImGui::Spacing();

    ImVec2 tabIconSize = ImVec2(48.0f, 48.0f);
    struct CraftTabButton { ItemTab tab; const char* strId; const char* fallbackText; };
    CraftTabButton craftTabs[] = {
        {ItemTab::Materials, "##CraftTabMaterials", "Материалы"},
        {ItemTab::Weapons,   "##CraftTabWeapons",   "Оружие"}
    };

    for (int i = 0; i < 2; ++i) {
        if (i > 0) ImGui::SameLine();

        ItemTab tab = craftTabs[i].tab;
        bool isActive = (currentCraftingTab == tab);

        if (isActive) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.5f, 0.8f, 1.0f));
        }

        if (tabTextures.count(tab) && tabTextures[tab].getNativeHandle() != 0) {
            ImTextureID texId = (ImTextureID)(uintptr_t)tabTextures[tab].getNativeHandle();
            if (ImGui::ImageButton(craftTabs[i].strId, texId, tabIconSize)) {
                currentCraftingTab = tab;
            }
        }
        else {
            if (ImGui::Button(craftTabs[i].fallbackText, ImVec2(120.0f, 40.0f))) {
                currentCraftingTab = tab;
            }
        }

        if (isActive) {
            ImGui::PopStyleColor();
        }
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Доступно в инвентаре (%s):", activeTabName.c_str());
    ImGui::Spacing();

    ItemCategory selectedCategory = (currentCraftingTab == ItemTab::Materials)
        ? ItemCategory::Material
        : ItemCategory::Weapon;

    auto availableItems = playerInventory.getItemsByCategory(selectedCategory);

    std::unordered_map<std::string, int> gridCounts;
    for (const auto& id : craftingGrid) {
        if (!id.empty()) gridCounts[id]++;
    }

    for (size_t i = 0; i < availableItems.size(); ++i) {
        const auto& stack = availableItems[i];
        int availableToUse = stack.count - gridCounts[stack.itemId];

        if (availableToUse > 0) {
            auto& dbItems = GameDatabase::getInstance().items;
            if (dbItems.find(stack.itemId) == dbItems.end()) continue;

            const auto& itemData = dbItems[stack.itemId];

            ImGui::PushID(static_cast<int>(i));

            if (ImGui::ImageButton("##CraftItemBtn", itemData->getImGuiTextureID(), ImVec2(48, 48))) {
                for (int slotIndex = 0; slotIndex < 9; ++slotIndex) {
                    if (craftingGrid[slotIndex].empty()) {
                        craftingGrid[slotIndex] = stack.itemId;
                        break;
                    }
                }
            }

            ImGui::SameLine();
            ImGui::Text("%s\nДоступно: %d", itemData->getName().c_str(), availableToUse);

            ImGui::PopID();
        }
    }

    ImGui::NextColumn();

    ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Сетка создания (3x3):");
    ImGui::Spacing();

    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 3; ++col) {
            int index = row * 3 + col;

            ImGui::PushID(200 + index);

            if (craftingGrid[index].empty()) {
                ImGui::Button("##EmptyCraftSlot", ImVec2(64, 64));
            }
            else {
                std::string itemId = craftingGrid[index];
                const auto& itemData = GameDatabase::getInstance().items[itemId];

                if (ImGui::ImageButton("##FilledCraftSlot", itemData->getImGuiTextureID(), ImVec2(64, 64))) {
                    craftingGrid[index] = "";
                }
            }

            if (col < 2) ImGui::SameLine();

            ImGui::PopID();
        }
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Результат создания:");
    ImGui::Spacing();

    std::vector<std::string> currentComponents;
    for (const auto& id : craftingGrid) {
        if (!id.empty()) currentComponents.push_back(id);
    }

    std::sort(currentComponents.begin(), currentComponents.end());

    struct CraftRecipe {
        std::string resultId;
        std::vector<std::string> requiredComponents;
    };

    static std::vector<CraftRecipe> craftRecipes = {
        {"old_cleaver", {"rusty_blade", "scrap_metal"}},
        {"old_cleaver", {"rusty_blade", "iron_ore", "iron_ore"}},
        {"rusty_blade", {"scrap_metal", "scrap_metal"}},
        {"bastion_blade", {"old_cleaver", "iron_ore", "scrap_metal"}},
        {"bastion_blade", {"iron_ore", "mithril_ore", "scrap_metal"}},
        {"berserker_axe", {"old_cleaver", "iron_ore", "iron_ore"}},
        {"berserker_axe", {"old_cleaver", "iron_ore", "coal_dust"}},
        {"shadow_dagger", {"scrap_metal", "iron_ore"}},
        {"shadow_dagger", {"scrap_metal", "void_shard", "mithril_ore"}},
        {"hunter_bow", {"scrap_metal", "iron_ore", "scrap_metal"}},
        {"hunter_bow", {"scrap_metal", "scrap_metal", "mithril_ore"}},
        {"vampire_blade", {"old_cleaver", "fire_crystal", "iron_ore"}},
        {"vampire_blade", {"old_cleaver", "fire_crystal", "mithril_ore"}},
        {"apprentice_staff", {"iron_ore", "glowing_moss", "fire_crystal"}},
        {"apprentice_staff", {"fire_crystal", "void_shard", "celestial_ingot"}},
        {"guardian_helm", {"iron_ore", "iron_ore", "scrap_metal"}},
        {"guardian_armor", {"iron_ore", "iron_ore", "iron_ore"}},
        {"guardian_ring", {"iron_ore", "mithril_ore"}},
        {"guardian_amulet", {"iron_ore", "fire_crystal"}}
    };

    std::string matchedResult = "";
    for (const auto& recipe : craftRecipes) {
        std::vector<std::string> req = recipe.requiredComponents;
        std::sort(req.begin(), req.end());

        if (req == currentComponents) {
            matchedResult = recipe.resultId;
            break;
        }
    }

    auto& dbItems = GameDatabase::getInstance().items;
    if (!matchedResult.empty() && dbItems.find(matchedResult) != dbItems.end()) {
        const auto& resultData = dbItems[matchedResult];

        if (ImGui::ImageButton("##CraftResultBtn", resultData->getImGuiTextureID(), ImVec2(80, 80))) {
            playerInventory.addItem(matchedResult, 1);

            for (const auto& id : currentComponents) {
                playerInventory.removeItem(id, 1);
            }

            clearCraftingGrid();
        }
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.2f, 0.9f, 0.2f, 1.0f),
            "Готово: %s!\nНажмите на иконку, чтобы создать.",
            resultData->getName().c_str());
    }
    else {
        ImGui::BeginDisabled();
        ImGui::Button("?", ImVec2(80, 80));
        ImGui::EndDisabled();
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 0.5f),
            "Разместите материалы и оружие,\nчтобы создать новый предмет.");
    }

    ImGui::Columns(1);
}


// =============================================================================
// ИНТЕРАКТИВНАЯ ДОБЫЧА РУДЫ И МИНЕРАЛОВ (МИНИ-ИГРА)
// =============================================================================

void ScreenManager::renderMining() {
    ImVec2 displaySize = ImGui::GetIO().DisplaySize;
    float panelW = std::min(960.0f, displaySize.x - 60.0f);
    float panelH = std::min(680.0f, displaySize.y - 60.0f);
    ImVec2 panelPos = ImVec2((displaySize.x - panelW) * 0.5f, (displaySize.y - panelH) * 0.5f);

    ImGui::SetNextWindowPos(panelPos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(panelW, panelH), ImGuiCond_Always);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.08f, 0.09f, 0.13f, 0.96f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.85f, 0.70f, 0.30f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 2.0f);

    std::string title = "Добыча Руды и Минералов — Этаж " + std::to_string(currentMiningFloor) + "##MiningWindow";
    if (ImGui::Begin(title.c_str(), nullptr, flags)) {
        // Заголовок и кнопка назад
        ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.35f, 1.0f), "=== ЖИЛА РЕДКИХ МИНЕРАЛОВ (ЭТАЖ %d) ===", currentMiningFloor);
        ImGui::SameLine(panelW - 200.0f);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.20f, 0.45f, 0.80f, 1.0f));
        if (ImGui::Button("<- На карту##ExitMining", ImVec2(160.0f, 26.0f))) {
            currentScreen = ScreenState::Map;
        }
        ImGui::PopStyleColor();

        ImGui::Separator();
        ImGui::Spacing();

        // Верхний информационный блок
        ImGui::TextWrapped("Перед вами обнажилась богатая кристаллическая жила. Сконцентрируйтесь и нанесите удар киркой в резонансную зеленую зону, чтобы добыть редкие самородки и кристаллы для ковки!");
        ImGui::Spacing();

        // Статистика попыток
        ImGui::TextColored(ImVec4(0.40f, 0.85f, 1.0f, 1.0f), "Осталось ударов киркой: %d / 5", miningAttemptsLeft);
        ImGui::SameLine(300.0f);
        ImGui::TextColored(ImVec4(0.50f, 0.95f, 0.55f, 1.0f), "Всего добыто ресурсов за подход: %d ед.", miningTotalYield);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Интерактивная анимационная шкала
        miningTimer += ImGui::GetIO().DeltaTime * (2.8f + currentMiningFloor * 0.12f);
        float strikePos = (sinf(miningTimer * 3.2f) + 1.0f) * 0.5f; // 0.0 .. 1.0

        float barW = panelW - 80.0f;
        float barH = 50.0f;
        ImVec2 barMin = ImGui::GetCursorScreenPos();
        ImVec2 barMax = ImVec2(barMin.x + barW, barMin.y + barH);

        ImDrawList* drawList = ImGui::GetWindowDrawList();

        // Фоновый контур шкалы
        drawList->AddRectFilled(barMin, barMax, IM_COL32(25, 28, 38, 255), 8.0f);
        drawList->AddRect(barMin, barMax, IM_COL32(140, 150, 175, 200), 8.0f, 0, 2.0f);

        // Зоны:
        // 1. Обычная зона (края): серый
        // 2. Хорошая зона (0.24 .. 0.76): янтарный
        float goodMinX = barMin.x + barW * 0.24f;
        float goodMaxX = barMin.x + barW * 0.76f;
        drawList->AddRectFilled(ImVec2(goodMinX, barMin.y + 3.0f), ImVec2(goodMaxX, barMax.y - 3.0f), IM_COL32(200, 140, 30, 90));

        // 3. Идеальная / Критическая зона (0.43 .. 0.57): светящийся изумруд с пульсацией
        float critMinX = barMin.x + barW * 0.43f;
        float critMaxX = barMin.x + barW * 0.57f;
        float pulse = sinf(miningTimer * 6.0f) * 0.25f + 0.75f;
        drawList->AddRectFilled(ImVec2(critMinX, barMin.y + 2.0f), ImVec2(critMaxX, barMax.y - 2.0f),
            IM_COL32((int)(40 * pulse), (int)(230 * pulse), (int)(140 * pulse), 180));
        drawList->AddRect(ImVec2(critMinX, barMin.y + 2.0f), ImVec2(critMaxX, barMax.y - 2.0f),
            IM_COL32(180, 255, 200, 230), 0.0f, 0, 1.8f);

        // Разделительные риски и надписи
        drawList->AddText(ImVec2(barMin.x + 14.0f, barMin.y + 16.0f), IM_COL32(160, 170, 190, 220), "Скол (Базовая руда)");
        drawList->AddText(ImVec2(barMax.x - 180.0f, barMin.y + 16.0f), IM_COL32(160, 170, 190, 220), "Скол (Базовая руда)");
        drawList->AddText(ImVec2(barMin.x + barW * 0.27f, barMin.y + 16.0f), IM_COL32(245, 190, 80, 230), "Хороший удар");
        drawList->AddText(ImVec2(barMin.x + barW * 0.60f, barMin.y + 16.0f), IM_COL32(245, 190, 80, 230), "Хороший удар");
        drawList->AddText(ImVec2(barMin.x + barW * 0.44f, barMin.y + 16.0f), IM_COL32(255, 255, 255, 255), "ТОЧНЫЙ РЕЗОНАНС!");

        // Бегунок-указатель удара
        float needleX = barMin.x + strikePos * barW;
        drawList->AddLine(ImVec2(needleX, barMin.y - 6.0f), ImVec2(needleX, barMax.y + 6.0f), IM_COL32(255, 45, 45, 255), 3.5f);
        // Треугольники указателя сверху и снизу
        drawList->AddTriangleFilled(
            ImVec2(needleX, barMin.y + 2.0f),
            ImVec2(needleX - 7.0f, barMin.y - 8.0f),
            ImVec2(needleX + 7.0f, barMin.y - 8.0f),
            IM_COL32(255, 230, 60, 255)
        );
        drawList->AddTriangleFilled(
            ImVec2(needleX, barMax.y - 2.0f),
            ImVec2(needleX - 7.0f, barMax.y + 8.0f),
            ImVec2(needleX + 7.0f, barMax.y + 8.0f),
            IM_COL32(255, 230, 60, 255)
        );

        ImGui::Dummy(ImVec2(barW, barH + 16.0f));
        ImGui::Spacing();

        // Кнопка удара киркой
        bool canStrike = (miningAttemptsLeft > 0);
        float btnW = 340.0f;
        float btnH = 46.0f;
        ImGui::SetCursorPosX((panelW - btnW) * 0.5f);

        if (!canStrike) {
            ImGui::BeginDisabled();
            ImGui::Button("Жила истощена (0 ударов)", ImVec2(btnW, btnH));
            ImGui::EndDisabled();
        } else {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.20f, 0.65f, 0.35f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.28f, 0.80f, 0.42f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.15f, 0.55f, 0.28f, 1.0f));

            if (ImGui::Button("[УДАР]  НАНЕСТИ ТОЧНЫЙ УДАР КИРКОЙ!##Strike", ImVec2(btnW, btnH))) {
                miningAttemptsLeft--;
                float dist = std::fabs(strikePos - 0.5f);

                if (dist <= 0.08f) {
                    // Критический точный удар
                    if (currentMiningFloor >= 8) {
                        playerInventory.addItem("celestial_ingot", 1);
                        playerInventory.addItem("void_shard", 2);
                        playerInventory.addItem("mithril_ore", 3);
                        miningTotalYield += 6;
                        miningFeedbackMsg = "[*] КРИТИЧЕСКИЙ УДАР! Жила раскололась: получен [Небесный слиток x1], [Осколок пустоты x2], [Мифриловая руда x3]!";
                    } else if (currentMiningFloor >= 4) {
                        playerInventory.addItem("fire_crystal", 2);
                        playerInventory.addItem("mithril_ore", 2);
                        playerInventory.addItem("iron_ore", 4);
                        miningTotalYield += 8;
                        miningFeedbackMsg = "[*] КРИТИЧЕСКИЙ УДАР! Жила озарилась жаром: получен [Огненный кристалл x2], [Мифриловая руда x2], [Железная руда x4]!";
                    } else {
                        playerInventory.addItem("mithril_ore", 2);
                        playerInventory.addItem("iron_ore", 4);
                        playerInventory.addItem("scrap_metal", 3);
                        miningTotalYield += 9;
                        miningFeedbackMsg = "[*] КРИТИЧЕСКИЙ УДАР! Высечена богатая порода: получен [Мифриловая руда x2], [Железная руда x4], [Обломки металла x3]!";
                    }
                    miningFeedbackColor = ImVec4(0.25f, 1.0f, 0.45f, 1.0f);
                } else if (dist <= 0.26f) {
                    // Отличный удар
                    if (currentMiningFloor >= 6) {
                        playerInventory.addItem("mithril_ore", 1);
                        playerInventory.addItem("scrap_metal", 3);
                        playerInventory.addItem("iron_ore", 3);
                        miningTotalYield += 7;
                        miningFeedbackMsg = "Отличный удар! Вы откололи увесистые куски: [Мифриловая руда x1], [Железная руда x3], [Обломки металла x3].";
                    } else {
                        playerInventory.addItem("iron_ore", 3);
                        playerInventory.addItem("scrap_metal", 2);
                        playerInventory.addItem("coal_dust", 2);
                        miningTotalYield += 7;
                        miningFeedbackMsg = "Отличный удар! Добыта качественная руда: [Железная руда x3], [Обломки металла x2], [Угольная пыль x2].";
                    }
                    miningFeedbackColor = ImVec4(1.0f, 0.85f, 0.35f, 1.0f);
                } else {
                    // Базовый скол
                    playerInventory.addItem("scrap_metal", 2);
                    playerInventory.addItem("coal_dust", 1);
                    miningTotalYield += 3;
                    miningFeedbackMsg = "Скол по краю: задета пустая порода. Добыто: [Обломки металла x2], [Угольная пыль x1].";
                    miningFeedbackColor = ImVec4(0.75f, 0.80f, 0.85f, 1.0f);
                }
            }
            ImGui::PopStyleColor(3);
        }

        ImGui::Spacing();
        if (!miningFeedbackMsg.empty()) {
            ImGui::SetCursorPosX((panelW - ImGui::CalcTextSize(miningFeedbackMsg.c_str()).x) * 0.5f);
            ImGui::TextColored(miningFeedbackColor, "%s", miningFeedbackMsg.c_str());
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Панель запасов ресурсов игрока для ковки
        ImGui::TextColored(ImVec4(0.40f, 0.85f, 1.0f, 1.0f), "Ваши запасы ресурсов для ковки оружия:");
        ImGui::Spacing();

        auto& db = GameDatabase::getInstance();
        std::vector<std::pair<std::string, std::string>> oreResources = {
            {"iron_ore", "Железная руда"},
            {"scrap_metal", "Обломки металла"},
            {"coal_dust", "Угольная пыль"},
            {"mithril_ore", "Мифриловая руда"},
            {"fire_crystal", "Огненный кристалл"},
            {"void_shard", "Осколок пустоты"},
            {"celestial_ingot", "Небесный слиток"}
        };

        ImGui::Columns(static_cast<int>(oreResources.size()), "MiningResCols", false);
        for (const auto& [resId, resName] : oreResources) {
            int count = playerInventory.getItemCount(resId);
            auto itItem = db.items.find(resId);
            if (itItem != db.items.end()) {
                ImGui::Image(itItem->second->getImGuiTextureID(), ImVec2(48.0f, 48.0f));
            }
            ImGui::TextColored(count > 0 ? ImVec4(1.0f, 1.0f, 1.0f, 1.0f) : ImVec4(0.5f, 0.5f, 0.5f, 0.5f), "%s", resName.c_str());
            ImGui::TextColored(count > 0 ? ImVec4(0.35f, 0.95f, 0.45f, 1.0f) : ImVec4(0.5f, 0.5f, 0.5f, 0.5f), "x%d", count);
            ImGui::NextColumn();
        }
        ImGui::Columns(1);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::TextDisabled("Подсказка: Собранные ресурсы можно использовать на столе создания (Крафтинг) для ковки мечей, топоров, луков и посохов.");

        ImGui::End();
    }
    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar();
}
