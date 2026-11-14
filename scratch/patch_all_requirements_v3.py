import os
import re

cpp_path = r"src\ScreenManager.cpp"
with open(cpp_path, "r", encoding="utf-8") as f:
    text = f.read()

# -------------------------------------------------------------
# 1. REMOVE ALL EMOJIS AND SPECIAL UNICODE CHARACTERS
# -------------------------------------------------------------
emoji_replacements = [
    ("\U0001f5fa", "[Карта]"),
    ("\U0001f465", "[Отряд]"),
    ("\U0001f464", "[Герои]"),
    ("\U0001f392", "[Сумка]"),
    ("\U0001f3c6", "[Награды]"),
    ("\U0001f4dc", "[Задания]"),
    ("\u2605", "[*]"),
    ("\u2694", "[Атака]"),
    ("\U0001f6e1", "[Защита]"),
    ("\u26a1", "[Крит]"),
    ("\U0001f4b0", "[Золото]"),
    ("\u2713", "[+]"),
    ("\U0001f512", "[x]"),
    ("\u25cf", "*"),
    ("\U0001f451", "[Шлем]"),
    ("\U0001f94b", "[Доспех]"),
    ("\U0001f4ff", "[Амулет]"),
    ("\U0001f48d", "[Кольцо]"),
    ("\U0001f4a5", "[Крит.урон]"),
    ("\U0001fa78", "[Вампиризм]"),
    ("\U0001f525", "[Бонус]"),
    ("\u2620", "[Босс]"),
    ("\u2726", "*"),
    ("\u25c6", "*"),
    ("\U0001f3af", "[Цель]"),
    ("\u26cf", "[УДАР]"),
    ("\u2022", "-")
]

for em, rep in emoji_replacements:
    text = text.replace(em, rep)

print("All emojis replaced with clean text tags!")

# -------------------------------------------------------------
# 2. INVENTORY: PREVENT SCROLLING INSIDE ITEM CARD
# -------------------------------------------------------------
old_inv_child = 'ImGui::BeginChild("InvCard", ImVec2(0.0f, 82.0f), true, ImGuiWindowFlags_NoScrollbar);'
new_inv_child = 'ImGui::BeginChild("InvCard", ImVec2(0.0f, 88.0f), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);'

if old_inv_child in text:
    text = text.replace(old_inv_child, new_inv_child)
    print("Inventory item card scrolling disabled!")

# -------------------------------------------------------------
# 3. COOKING & CRAFTING RECIPES
# -------------------------------------------------------------
# Replace cooking recipes
old_cook_recipes_pattern = r'static std::vector<SimpleRecipe> recipes = \{[\s\S]*?\};\s*std::string matchedResult = "";'
new_cook_recipes = """static std::vector<SimpleRecipe> recipes = {
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

    std::string matchedResult = "";"""

m_cook = re.search(old_cook_recipes_pattern, text)
if m_cook:
    if "\r\n" in m_cook.group(0):
        new_cook_recipes = new_cook_recipes.replace("\n", "\r\n")
    text = text[:m_cook.start()] + new_cook_recipes + text[m_cook.end():]
    print("Expanded cooking recipes added!")

# Replace crafting recipes
old_craft_pattern = r'static std::vector<CraftRecipe> craftRecipes = \{[\s\S]*?\};\s*std::string matchedResult = "";'
new_craft_recipes = """static std::vector<CraftRecipe> craftRecipes = {
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

    std::string matchedResult = "";"""

m_craft = re.search(old_craft_pattern, text)
if m_craft:
    if "\r\n" in m_craft.group(0):
        new_craft_recipes = new_craft_recipes.replace("\n", "\r\n")
    text = text[:m_craft.start()] + new_craft_recipes + text[m_craft.end():]
    print("Expanded forge/crafting recipes added!")

# -------------------------------------------------------------
# 4. QUESTS: STORY VS SIDE QUESTS WITH FILTER TABS
# -------------------------------------------------------------
old_quests_init_pattern = r'void ScreenManager::initQuests\(\)\s*\{[\s\S]*?void ScreenManager::renderQuests\(\)\s*\{[\s\S]*?ImGui::EndChild\(\);\s*\}'

new_quests_code = """void ScreenManager::initQuests() {
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
        ImGui::TextColored(ImVec4(0.30f, 1.0f, 0.45f, 1.0f), "[*] Готовы к сдаче: %d!", readyCount);
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
    float cardW = std::min(availW, 940.0f);
    float startX = (availW - cardW) * 0.5f;
    if (startX < 0.0f) startX = 0.0f;

    auto& db = GameDatabase::getInstance();

    ImGui::BeginChild("QuestsScrollList", ImVec2(0.0f, -10.0f), false);

    int displayedCount = 0;
    for (size_t i = 0; i < quests.size(); ++i) {
        auto& q = quests[i];

        // Фильтрация: 0 = Все, 1 = Сюжетные, 2 = Побочные
        if (currentQuestFilterTab == 1 && !q.isStory) continue;
        if (currentQuestFilterTab == 2 && q.isStory) continue;

        displayedCount++;
        ImGui::SetCursorPosX(startX);
        ImGui::PushID(static_cast<int>(i));

        ImVec4 bgCol = (q.status == QuestStatus::Completed)
            ? ImVec4(0.12f, 0.20f, 0.16f, 0.95f)
            : (q.status == QuestStatus::Claimed ? ImVec4(0.07f, 0.08f, 0.12f, 0.80f) : ImVec4(0.09f, 0.11f, 0.17f, 0.90f));

        ImVec4 borderCol = (q.status == QuestStatus::Completed)
            ? ImVec4(0.35f, 1.0f, 0.50f, 1.0f)
            : (q.status == QuestStatus::Claimed ? ImVec4(0.30f, 0.35f, 0.45f, 0.60f) : (q.isStory ? ImVec4(0.85f, 0.70f, 0.25f, 0.9f) : ImVec4(0.25f, 0.55f, 0.85f, 0.85f)));

        ImGui::PushStyleColor(ImGuiCol_ChildBg, bgCol);
        ImGui::PushStyleColor(ImGuiCol_Border, borderCol);
        ImGui::BeginChild("QuestCard", ImVec2(cardW, 105.0f), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        // Метка Сюжетное / Побочное
        if (q.isStory) {
            ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.30f, 1.0f), "[ СЮЖЕТНОЕ ]");
        } else {
            ImGui::TextColored(ImVec4(0.40f, 0.85f, 1.0f, 1.0f), "[ ПОБОЧНОЕ ]");
        }
        ImGui::SameLine();

        // Категория
        ImVec4 catColor = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
        if (q.category == "Охота") catColor = ImVec4(1.0f, 0.50f, 0.40f, 1.0f);
        else if (q.category == "Добыча") catColor = ImVec4(0.40f, 0.90f, 1.0f, 1.0f);
        else if (q.category == "Кузница") catColor = ImVec4(1.0f, 0.80f, 0.30f, 1.0f);
        else if (q.category == "Кулинария") catColor = ImVec4(0.50f, 0.95f, 0.55f, 1.0f);

        ImGui::TextColored(catColor, "(%s)", q.category.c_str());
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 0.95f, 0.75f, 1.0f), "%s", q.title.c_str());

        // Описание
        ImGui::TextColored(ImVec4(0.75f, 0.80f, 0.85f, 1.0f), "%s", q.description.c_str());

        // Прогресс
        float pRatio = std::clamp(static_cast<float>(q.currentProgress) / static_cast<float>(q.targetProgress), 0.0f, 1.0f);
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, (pRatio >= 1.0f) ? ImVec4(0.25f, 0.85f, 0.40f, 1.0f) : ImVec4(0.20f, 0.55f, 0.90f, 1.0f));
        ImGui::ProgressBar(pRatio, ImVec2(cardW - 320.0f, 14.0f), "");
        ImGui::PopStyleColor();
        ImGui::SameLine();
        ImGui::TextDisabled("(%d / %d) %s", std::min(q.currentProgress, q.targetProgress), q.targetProgress, q.progressLabel.c_str());

        // Награды слева внизу
        ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.25f, 1.0f), "Награда: +%d монет", q.rewardGold);
        if (q.rewardExp > 0) {
            ImGui::SameLine(0.0f, 15.0f);
            ImGui::TextColored(ImVec4(0.45f, 0.90f, 1.0f, 1.0f), "+%d опыта", q.rewardExp);
        }
        if (!q.rewardItemId.empty()) {
            ImGui::SameLine(0.0f, 15.0f);
            auto itItem = db.items.find(q.rewardItemId);
            std::string itemName = (itItem != db.items.end()) ? itItem->second->getName() : q.rewardItemId;
            ImGui::TextColored(ImVec4(0.85f, 0.65f, 1.0f, 1.0f), "+ [%s x%d]", itemName.c_str(), q.rewardItemCount);
        }

        // Кнопка сдачи справа
        ImGui::SetCursorPos(ImVec2(cardW - 200.0f, 36.0f));
        if (q.status == QuestStatus::Completed) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.20f, 0.70f, 0.35f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.28f, 0.85f, 0.45f, 1.0f));
            if (ImGui::Button("[*] ЗАБРАТЬ НАГРАДУ##Claim", ImVec2(185.0f, 34.0f))) {
                playerInventory.addFunds(q.rewardGold);
                if (!q.rewardItemId.empty()) {
                    playerInventory.addItem(q.rewardItemId, q.rewardItemCount);
                }
                q.status = QuestStatus::Claimed;
            }
            ImGui::PopStyleColor(2);
        } else if (q.status == QuestStatus::Claimed) {
            ImGui::TextColored(ImVec4(0.50f, 0.50f, 0.55f, 1.0f), "[+] Награда получена");
        } else {
            ImGui::TextColored(ImVec4(0.40f, 0.75f, 1.0f, 1.0f), "В процессе...");
        }

        ImGui::EndChild();
        ImGui::PopStyleColor(2);
        ImGui::PopID();
        ImGui::Spacing();
    }

    if (displayedCount == 0) {
        ImGui::Spacing();
        ImGui::SetCursorPosX(startX);
        ImGui::TextDisabled("В этой категории пока нет доступных заданий.");
    }

    ImGui::EndChild();
}"""

m_quests = re.search(old_quests_init_pattern, text)
if m_quests:
    if "\r\n" in m_quests.group(0):
        new_quests_code = new_quests_code.replace("\n", "\r\n")
    text = text[:m_quests.start()] + new_quests_code + text[m_quests.end():]
    print("Quests Story vs Side filters successfully added!")
else:
    print("Could not match old_quests_init_pattern!")

# -------------------------------------------------------------
# 5. OVERHAUL CHARACTER EDITOR LAYOUT (FIX OVERLAPPING)
# -------------------------------------------------------------
old_ce_lower_pattern = r'float availW = ImGui::GetContentRegionAvail\(\)\.x;\s*// Правая панель характеристик[\s\S]*?renderSlotPickerModal\(currentChar\);\s*\}'

new_ce_lower_code = """    float availW = ImGui::GetContentRegionAvail().x;

    // Правая панель характеристик: ширина 410px
    float rightPanelW = std::clamp(availW * 0.30f, 390.0f, 440.0f);
    float leftAreaW = availW - rightPanelW - 20.0f;

    // ГЛАВНЫЙ РАЗДЕЛИТЕЛЬ НА 2 СТОЛБЦА (БЕЗ НАЛОЖЕНИЙ)
    ImGui::Columns(2, "CharacterEditorTwoMainColumns", false);
    ImGui::SetColumnWidth(0, leftAreaW);
    ImGui::SetColumnWidth(1, rightPanelW);

    auto& db = GameDatabase::getInstance();

    auto renderSlotCard = [&](const char* slotTitle, ArtifactType slotType, const std::string& currentItemId, const char* iconKey) {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.09f, 0.11f, 0.17f, 0.85f));
        ImGui::PushStyleColor(ImGuiCol_Border, currentItemId.empty() ? ImVec4(0.35f, 0.40f, 0.50f, 0.6f) : ImVec4(0.85f, 0.70f, 0.25f, 0.9f));
        ImGui::BeginChild((std::string("SlotBox_") + slotTitle).c_str(), ImVec2(0, 100.0f), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.35f, 1.0f), "%s", slotTitle);
        ImGui::Separator();

        // Иконка слота
        auto itSlotTex = slotIconTextures.find(iconKey);
        if (itSlotTex != slotIconTextures.end() && itSlotTex->second.getNativeHandle() != 0) {
            ImTextureID slotTexId = (ImTextureID)(uintptr_t)itSlotTex->second.getNativeHandle();
            ImGui::Image(slotTexId, ImVec2(48.0f, 48.0f));
            ImGui::SameLine(0.0f, 10.0f);
        }

        ImGui::BeginGroup();
        if (currentItemId.empty()) {
            ImGui::TextDisabled("[ Пустой слот ]");
            if (ImGui::Button("+ Надеть##SlotBtn", ImVec2(100.0f, 22.0f))) {
                activeSlotPicker = slotType;
                requestOpenSlotPicker = true;
            }
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

            if (ImGui::Button("Сменить##Chg", ImVec2(75.0f, 22.0f))) {
                activeSlotPicker = slotType;
                requestOpenSlotPicker = true;
            }
            ImGui::SameLine(0.0f, 6.0f);
            if (ImGui::Button("Снять##Uniq", ImVec2(65.0f, 22.0f))) {
                if (slotType == ArtifactType::Weapon) {
                    currentChar->unequipWeapon(playerInventory);
                } else {
                    currentChar->unequipArtifact(slotType, playerInventory);
                }
            }
        }
        ImGui::EndGroup();

        ImGui::EndChild();
        ImGui::PopStyleColor(2);
        ImGui::Spacing();
    };

    // =========================================================================
    // ЛЕВЫЙ СТОЛБЕЦ: СНАРЯЖЕНИЕ И ВИЗУАЛ ГЕРОЯ (РАЗБИТ НА 3 СУБКОЛОНКИ)
    // =========================================================================
    float colW1 = 250.0f;
    float colW3 = 250.0f;
    float colW2 = std::max(280.0f, leftAreaW - colW1 - colW3 - 30.0f);

    ImGui::Columns(3, "EditorLeftSlotsModelGrid", false);
    ImGui::SetColumnWidth(0, colW1);
    ImGui::SetColumnWidth(1, colW2);
    ImGui::SetColumnWidth(2, colW3);

    // Подколонка 0: Оружие, Шлем, Костюм
    ImGui::TextColored(ImVec4(0.40f, 0.85f, 1.0f, 1.0f), "Снаряжение (Оружие / Броня):");
    ImGui::Spacing();
    renderSlotCard("[Оружие] Слот оружия", ArtifactType::Weapon, currentChar->getEquippedWeapon(), "slot_weapon");
    renderSlotCard("[Шлем] Головной убор", ArtifactType::Headgear, currentChar->getEquippedHeadgear(), "slot_headgear");
    renderSlotCard("[Доспех] Костюм / Доспех", ArtifactType::Costume, currentChar->getEquippedCostume(), "slot_costume");

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
        ImGui::TextColored(ImVec4(0.35f, 1.0f, 0.45f, 1.0f), "* В боевом отряде (Слот %d)", static_cast<int>(activeSquadSlot + 1));
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

    // Способности героя
    auto charTmplIt = db.characters.find(currentChar->getId());
    if (charTmplIt != db.characters.end()) {
        ImGui::TextColored(ImVec4(0.40f, 0.85f, 1.0f, 1.0f), "Способности героя:");
        const auto& skills = charTmplIt->second->getSkills();
        for (size_t sk = 0; sk < skills.size(); ++sk) {
            std::string skTag = (sk == 0) ? "[Базовая] " : (sk == 1) ? "[Навык] " : "[Ульта] ";
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
    ImGui::TextColored(ImVec4(0.40f, 0.85f, 1.0f, 1.0f), "Бижутерия и Комплекты:");
    ImGui::Spacing();
    renderSlotCard("[Амулет] Талисман", ArtifactType::Amulet, currentChar->getEquippedAmulet(), "slot_amulet");
    renderSlotCard("[Кольцо] Перстень", ArtifactType::Ring, currentChar->getEquippedRing(), "slot_ring");

    // Панель комплектов артефактов
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.08f, 0.10f, 0.15f, 0.85f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.40f, 0.70f, 0.50f, 0.80f));
    ImGui::BeginChild("ArtifactSetsPanel", ImVec2(0, 275.0f), true, ImGuiWindowFlags_NoScrollbar);

    ImGui::TextColored(ImVec4(0.45f, 0.95f, 0.55f, 1.0f), "[*] Бонусы комплектов:");
    ImGui::Separator();
    ImGui::Spacing();

    auto setCounts = currentChar->getActiveSetCounts();
    if (setCounts.empty()) {
        ImGui::TextDisabled("Сеты артефактов не активированы.");
        ImGui::Spacing();
        ImGui::TextDisabled("Экипируйте 2 или 4 предмета одного сета для получения пассивных бонусов!");
    } else {
        for (const auto& [setId, count] : setCounts) {
            auto itSet = db.artifactSets.find(setId);
            if (itSet == db.artifactSets.end()) continue;

            ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.35f, 1.0f), "«%s» (%d/4):", itSet->second.name.c_str(), count);
            if (count >= 2) {
                ImGui::TextColored(ImVec4(0.35f, 1.0f, 0.45f, 1.0f), " [2/4] [+] %s", itSet->second.twoPieceDesc.c_str());
            } else {
                ImGui::TextDisabled(" [2/4] [x] %s (+%d шт.)", itSet->second.twoPieceDesc.c_str(), 2 - count);
            }
            if (count >= 4) {
                ImGui::TextColored(ImVec4(0.35f, 1.0f, 0.45f, 1.0f), " [4/4] [+] %s", itSet->second.fourPieceDesc.c_str());
            } else {
                ImGui::TextDisabled(" [4/4] [x] %s (+%d шт.)", itSet->second.fourPieceDesc.c_str(), 4 - count);
            }
            ImGui::Spacing();
        }
    }

    ImGui::EndChild();
    ImGui::PopStyleColor(2);

    ImGui::Columns(1); // Завершаем подколонки слотов

    // =========================================================================
    // ПРАВЫЙ СТОЛБЕЦ ГЛАВНОГО РАЗДЕЛИТЕЛЯ: ИТОГОВЫЕ ХАРАКТЕРИСТИКИ
    // =========================================================================
    ImGui::NextColumn();

    StatArray totalStats = currentChar->getTotalStats();

    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.08f, 0.10f, 0.16f, 0.95f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.35f, 0.70f, 0.95f, 0.95f));
    ImGui::BeginChild("CharacterTotalStatsPanelLarge", ImVec2(0, 520.0f), true, ImGuiWindowFlags_NoScrollbar);

    ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.30f, 1.0f), "=== ИТОГОВЫЕ ХАРАКТЕРИСТИКИ ===");
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::TextColored(ImVec4(0.95f, 0.90f, 0.60f, 1.0f), "Герой: %s", currentChar->getName().c_str());
    ImGui::TextColored(ImVec4(0.40f, 0.85f, 1.0f, 1.0f), "Уровень героя: %d", currentChar->getLevel());
    ImGui::Spacing();

    // Здоровье и Мана
    ImGui::TextColored(ImVec4(0.35f, 1.0f, 0.45f, 1.0f), "Базовое здоровье (HP): %.0f", totalStats[StatIndex::HP]);
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

    // Боевые параметры
    ImGui::TextColored(ImVec4(1.0f, 0.65f, 0.30f, 1.0f), "[Атака] Сила атаки: %.1f", totalStats[StatIndex::Attack]);

    double mitigation = (totalStats[StatIndex::Defense] / (totalStats[StatIndex::Defense] + 100.0)) * 100.0;
    ImGui::TextColored(ImVec4(0.40f, 0.85f, 1.0f, 1.0f), "[Защита] Защита: %.1f (-%.1f%% урона)", totalStats[StatIndex::Defense], mitigation);

    ImGui::Text("[Крит] Критический шанс: %.1f%%", totalStats[StatIndex::CritChance] * 100.0);
    ImGui::Text("[Крит.урон] Критический урон: %.0f%%", totalStats[StatIndex::CritDamage] * 100.0);
    ImGui::Text("[Вампиризм] Вампиризм (хил от атак): %.1f%%", totalStats[StatIndex::Vampirism] * 100.0);
    ImGui::Text("[Бонус] Бонус к урону умений: +%.1f%%", totalStats[StatIndex::DamageBonus] * 100.0);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Сводка надетых предметов
    ImGui::TextColored(ImVec4(0.40f, 0.85f, 1.0f, 1.0f), "Экипированное снаряжение:");
    auto showEquipLine = [&](const char* slotName, const std::string& itemId) {
        if (itemId.empty()) {
            ImGui::TextDisabled("  %s: [Пусто]", slotName);
        } else {
            auto itItem = db.items.find(itemId);
            std::string name = (itItem != db.items.end()) ? itItem->second->getName() : itemId;
            ImGui::TextColored(ImVec4(0.95f, 0.90f, 0.70f, 1.0f), "  %s: %s", slotName, name.c_str());
        }
    };
    showEquipLine("Оружие", currentChar->getEquippedWeapon());
    showEquipLine("Шлем", currentChar->getEquippedHeadgear());
    showEquipLine("Доспех", currentChar->getEquippedCostume());
    showEquipLine("Амулет", currentChar->getEquippedAmulet());
    showEquipLine("Кольцо", currentChar->getEquippedRing());

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
    ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.25f, 1.0f), "[*] Боевая мощь героя: %d", powerScore);

    ImGui::EndChild();
    ImGui::PopStyleColor(2);

    ImGui::Columns(1); // Завершаем главный сплит

    renderSlotPickerModal(currentChar);
}"""

m_ce_lower = re.search(old_ce_lower_pattern, text)
if m_ce_lower:
    if "\r\n" in m_ce_lower.group(0):
        new_ce_lower_code = new_ce_lower_code.replace("\n", "\r\n")
    text = text[:m_ce_lower.start()] + new_ce_lower_code + text[m_ce_lower.end():]
    print("Character Editor overhaul applied successfully!")
else:
    print("Could not match old_ce_lower_pattern!")

with open(cpp_path, "w", encoding="utf-8") as f:
    f.write(text)

print("patch_all_requirements_v3 completed!")
