import os
import re

cpp_path = r"src\ScreenManager.cpp"
with open(cpp_path, "r", encoding="utf-8") as f:
    content = f.read()

# 1. Top includes
old_includes = """#include <iostream>
#include <imgui.h>
#include <algorithm>
#include "PlayerData.hpp\""""

new_includes = """#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <unordered_set>
#include <imgui.h>
#include <algorithm>
#include "PlayerData.hpp\""""

if "\r\n" in content:
    old_includes = old_includes.replace("\n", "\r\n")
    new_includes = new_includes.replace("\n", "\r\n")

if old_includes in content:
    content = content.replace(old_includes, new_includes, 1)
    print("Top includes updated!")

# 2. Constructor init
old_ctor = """    // Инициализация стартовой боевой группы (если она не заполнена)
    if (playerProfile.getCharacterCount() == 0) {
        playerProfile.obtainCharacter("adeir");
        playerProfile.obtainCharacter("renwick");
        playerProfile.obtainCharacter("garosh");
    }"""

new_ctor = """    // Инициализация стартовой боевой группы (если она не заполнена)
    if (playerProfile.getCharacterCount() == 0) {
        playerProfile.obtainCharacter("adeir");
        playerProfile.obtainCharacter("renwick");
        playerProfile.obtainCharacter("garosh");
    }

    initAchievements();
    loadGlobalAchievements();
    initQuests();"""

if "\r\n" in content:
    old_ctor = old_ctor.replace("\n", "\r\n")
    new_ctor = new_ctor.replace("\n", "\r\n")

if old_ctor in content:
    content = content.replace(old_ctor, new_ctor, 1)
    print("Constructor updated!")

# 3. renderCurrentScreen switch
old_switch = """    case ScreenState::Mining: renderMining(); break;
    }"""

new_switch = """    case ScreenState::Mining: renderMining(); break;
    case ScreenState::Quests: renderQuests(); break;
    }"""

if "\r\n" in content:
    old_switch = old_switch.replace("\n", "\r\n")
    new_switch = new_switch.replace("\n", "\r\n")

if old_switch in content:
    content = content.replace(old_switch, new_switch, 1)
    print("renderCurrentScreen switch updated!")

# 4. renderLobby targets (add 6th target: Quests)
old_lobby_nav = """    NavTarget targets[] = {
        {"[ 🗺 ] Карта этажей",           "Исследование 10 этажей, битвы, торговцы и святилища", ScreenState::Map,             ImVec4(0.20f, 0.50f, 0.80f, 1.0f)},
        {"[ 👥 ] Редактор отряда",       "Формирование ударной группы из 3 боевых героев отряда",  ScreenState::SquadEditor,     ImVec4(0.25f, 0.65f, 0.35f, 1.0f)},
        {"[ 👤 ] Герои и слоты",        "Смена экипировки, изучение артефактов и сетов", ScreenState::CharacterEditor, ImVec4(0.65f, 0.40f, 0.80f, 1.0f)},
        {"[ 🎒 ] Инвентарь и припасы",  "Снаряжение, драгоценные металлы, минералы и еда", ScreenState::Inventory,       ImVec4(0.85f, 0.60f, 0.20f, 1.0f)},
        {"[ 🏆 ] Зал достижений",        "Список наград, подвигов и бонусов за испытания",     ScreenState::Achievements,    ImVec4(0.90f, 0.75f, 0.25f, 1.0f)}
    };

    int count = 5;
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

        // Если это последняя (5-я) кнопка, центрируем ее по ширине экрана
        float posX = startX + col * (cardWidth + spacingX);
        float posY = startY + row * (cardHeight + spacingY);
        if (i == 4) {
            posX = (displaySize.x - cardWidth) * 0.5f;
        }

        ImGui::SetCursorPos(ImVec2(posX, posY));"""

new_lobby_nav = """    NavTarget targets[] = {
        {"[ 🗺 ] Карта этажей",           "Исследование 10 этажей, битвы, торговцы и святилища", ScreenState::Map,             ImVec4(0.20f, 0.50f, 0.80f, 1.0f)},
        {"[ 👥 ] Редактор отряда",       "Формирование ударной группы из 3 боевых героев отряда",  ScreenState::SquadEditor,     ImVec4(0.25f, 0.65f, 0.35f, 1.0f)},
        {"[ 👤 ] Герои и слоты",        "Смена экипировки, изучение артефактов и сетов", ScreenState::CharacterEditor, ImVec4(0.65f, 0.40f, 0.80f, 1.0f)},
        {"[ 🎒 ] Инвентарь и припасы",  "Снаряжение, драгоценные металлы, минералы и еда", ScreenState::Inventory,       ImVec4(0.85f, 0.60f, 0.20f, 1.0f)},
        {"[ 🏆 ] Зал достижений",        "Список наград, подвигов и бонусов за испытания",     ScreenState::Achievements,    ImVec4(0.90f, 0.75f, 0.25f, 1.0f)},
        {"[ 📜 ] Задания и квесты",      "Журнал поручений, охота на монстров и награды",       ScreenState::Quests,          ImVec4(0.30f, 0.70f, 0.85f, 1.0f)}
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

        ImGui::SetCursorPos(ImVec2(posX, posY));"""

if "\r\n" in content:
    old_lobby_nav = old_lobby_nav.replace("\n", "\r\n")
    new_lobby_nav = new_lobby_nav.replace("\n", "\r\n")

if old_lobby_nav in content:
    content = content.replace(old_lobby_nav, new_lobby_nav, 1)
    print("renderLobby targets updated!")

# 5. renderInventory full-width items grid
old_inv_grid = """    ItemCategory selectedCategory = tabToCategory(currentInventoryTab);
    auto itemsList = playerInventory.getItemsByCategory(selectedCategory);
    auto& dbItems = GameDatabase::getInstance().items;

    if (itemsList.empty()) {
        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "В этой категории пока нет предметов.");
    }
    else {
        int columns = 4;
        for (size_t i = 0; i < itemsList.size(); ++i) {
            const auto& stack = itemsList[i];
            if (dbItems.find(stack.itemId) == dbItems.end()) continue;
            const auto& itemData = dbItems[stack.itemId];

            ImGui::PushID(static_cast<int>(i));
            ImGui::BeginGroup();

            ImGui::ImageButton("##InvItemBtn", itemData->getImGuiTextureID(), ImVec2(64, 64));
            ImGui::SameLine();
            ImGui::Text("%s\\nК-во: %d\\nЦена: %d м",
                itemData->getName().c_str(), stack.count, itemData->getSellPrice());

            ImGui::EndGroup();

            if ((i + 1) % columns != 0 && i + 1 < itemsList.size()) {
                ImGui::SameLine(0.0f, 30.0f);
            }
            ImGui::PopID();
        }
    }"""

new_inv_grid = """    ItemCategory selectedCategory = tabToCategory(currentInventoryTab);
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

            ImGui::BeginChild("InvCard", ImVec2(0.0f, 82.0f), true, ImGuiWindowFlags_NoScrollbar);

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
    }"""

# Use regex replace for inv grid because of encoding in old text
inv_pattern = r'ItemCategory selectedCategory = tabToCategory\(currentInventoryTab\);[\s\S]*?ImGui::PopID\(\);\s*\}\s*\}'
m_inv = re.search(inv_pattern, content)
if m_inv:
    replacement_inv = new_inv_grid
    if "\r\n" in m_inv.group(0):
        replacement_inv = replacement_inv.replace("\n", "\r\n")
    content = content[:m_inv.start()] + replacement_inv + content[m_inv.end():]
    print("renderInventory full-width grid updated!")
else:
    print("Could not match inv_pattern!")

# 6. renderConsole: Remove hints and autocomplete
console_pattern = r'float footerHeight = ImGui::GetFrameHeightWithSpacing\(\) \+ 52\.0f;[\s\S]*?ImGui::End\(\);'
m_console = re.search(console_pattern, content)
if m_console:
    new_console_code = """float footerHeight = ImGui::GetFrameHeightWithSpacing() + 8.0f;
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
        if (consoleInputBuf[0] != '\\0') {
            executeConsoleCommand(consoleInputBuf);
            consoleInputBuf[0] = '\\0';
        }
        ImGui::SetKeyboardFocusHere(-1);
    }
    ImGui::PopItemWidth();

    ImGui::End();"""

    if "\r\n" in m_console.group(0):
        new_console_code = new_console_code.replace("\n", "\r\n")
    content = content[:m_console.start()] + new_console_code + content[m_console.end():]
    print("renderConsole hints successfully removed!")
else:
    print("Could not match console_pattern!")

# 7. executeConsoleCommand: add achievement command
old_help_block = """    // Команда: help
    if (cmd == "help" || cmd == "помощь" || cmd == "?") {
        logConsole("=== СПИСОК ДОСТУПНЫХ КОМАНД ===", ImVec4(1.0f, 0.85f, 0.3f, 1.0f));
        logConsole("1. Выдача: give <тип> <id> [кол-во]", ImVec4(0.4f, 0.9f, 1.0f, 1.0f));
        logConsole("   Пример: give character vex", ImVec4(0.85f, 0.85f, 0.85f, 1.0f));
        logConsole("   Чит: give all [кол-во] — выдает всех героев и все предметы в игре", ImVec4(1.0f, 0.85f, 0.25f, 1.0f));
        logConsole("            give food cave_wheat 5", ImVec4(0.85f, 0.85f, 0.85f, 1.0f));
        logConsole("            give weapon rusty_blade 1", ImVec4(0.85f, 0.85f, 0.85f, 1.0f));
        logConsole("2. Удаление: remove <тип> <id> [кол-во]", ImVec4(0.4f, 0.9f, 1.0f, 1.0f));
        logConsole("   Пример: remove character adeir", ImVec4(0.85f, 0.85f, 0.85f, 1.0f));
        logConsole("            remove food cave_wheat 2", ImVec4(0.85f, 0.85f, 0.85f, 1.0f));
        logConsole("3. Категории предметов: food, weapon, material, artifact, jewelry, quest", ImVec4(0.9f, 0.8f, 0.5f, 1.0f));
        logConsole("   Категория персонажей: character (или char, герой)", ImVec4(0.9f, 0.8f, 0.5f, 1.0f));
        logConsole("4. Очистка: clear (очистить историю консоли)", ImVec4(0.8f, 0.8f, 0.8f, 1.0f));
        return;
    }"""

help_pattern = r'// Команда: help[\s\S]*?return;\s*\}'
m_help = re.search(help_pattern, content)
if m_help:
    new_help_code = """// Команда: help
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
    }"""

    if "\r\n" in m_help.group(0):
        new_help_code = new_help_code.replace("\n", "\r\n")
    content = content[:m_help.start()] + new_help_code + content[m_help.end():]
    print("executeConsoleCommand updated with achievement commands!")
else:
    print("Could not match help_pattern!")

with open(cpp_path, "w", encoding="utf-8") as f:
    f.write(content)

print("Part 1 of apply_new_features completed!")
