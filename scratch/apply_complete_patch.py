import os

cpp_path = r"src\ScreenManager.cpp"
with open(cpp_path, "r", encoding="utf-8") as f:
    text = f.read()

crlf = "\r\n" if "\r\n" in text else "\n"

# 1. Constructor update
start_ctor = text.find("ScreenManager::ScreenManager()")
end_ctor = text.find("void ScreenManager::processEvent(const sf::Event& event)", start_ctor)
assert start_ctor != -1 and end_ctor != -1, "ctor markers not found"

new_ctor = """ScreenManager::ScreenManager() {
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
"""
if crlf == "\r\n":
    new_ctor = new_ctor.replace("\n", "\r\n")

text = text[:start_ctor] + new_ctor + text[end_ctor:]

# 2. Lobby NavTargets update
start_lobby = text.find("NavTarget targets[]")
end_str = "ImGui::SetCursorPos(ImVec2(posX, posY));"
end_lobby = text.find(end_str, start_lobby) + len(end_str)
assert start_lobby != -1 and end_lobby != -1, "lobby markers not found"

new_lobby = """NavTarget targets[] = {
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

if crlf == "\r\n":
    new_lobby = new_lobby.replace("\n", "\r\n")

text = text[:start_lobby] + new_lobby + text[end_lobby:]

# 3. Achievements and Quests implementation
start_ach = text.find("void ScreenManager::renderAchievements()")
end_ach = text.find("void ScreenManager::renderSquadEditor()", start_ach)
assert start_ach != -1 and end_ach != -1, "ach markers not found"

ach_and_quest = """void ScreenManager::initAchievements() {
    achievements.clear();

    // ==========================================
    // 1. ГЛОБАЛЬНЫЕ ДОСТИЖЕНИЯ (ОБЩИЕ ДЛЯ ВСЕХ СОХРАНЕНИЙ)
    // ==========================================
    achievements.push_back({
        "global_first_step",
        "Первые шаги",
        "Начать восхождение по таинственному Шпилю в любом сохранении.",
        "★",
        true,
        true
    });
    achievements.push_back({
        "global_floor_5",
        "Покоритель высот",
        "Достичь 5-го этажа Башни (Свалка Забвения).",
        "★",
        true,
        false
    });
    achievements.push_back({
        "global_floor_10",
        "Врата Истока",
        "Добраться до 10-го этажа Башни (Престол Первозданного Истока).",
        "★",
        true,
        false
    });
    achievements.push_back({
        "global_master_blacksmith",
        "Мастер горна",
        "Выковать легендарное оружие в кузнице из собранных редких руд.",
        "★",
        true,
        false
    });
    achievements.push_back({
        "global_hero_collector",
        "Легендарный созыв",
        "Собрать 10 уникальных героев в своем резерве или отряде.",
        "★",
        true,
        false
    });
    achievements.push_back({
        "global_mining_king",
        "Золотая жила",
        "Совершить критический точный удар киркой по рудной жиле.",
        "★",
        true,
        false
    });
    achievements.push_back({
        "global_boss_slayer",
        "Гроза чудовищ",
        "Одержать сокрушительную победу над великим боссом этажа.",
        "★",
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
        "⚔",
        false,
        false
    });
    achievements.push_back({
        "profile_full_gear",
        "Во всеоружии",
        "Полностью заполнить все 5 слотов экипировки у одного персонажа.",
        "🛡",
        false,
        false
    });
    achievements.push_back({
        "profile_set_synergy",
        "Сила комплекта",
        "Активировать полный комплектный бонус из 4 предметов артефактов.",
        "⚡",
        false,
        false
    });
    achievements.push_back({
        "profile_rich",
        "Богатей",
        "Накопить более 1000 золотых монет в кошельке отряда.",
        "💰",
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
        if (!line.empty() && line.back() == '\\r') line.pop_back();
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
            file << ach.id << "\\n";
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
    ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.30f, 1.0f), "[★ Глобальные: %d/%d]  [Профиль: %d/%d]",
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
    filterBtn("★ Глобальные (Общие)", 1);
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
            ImGui::TextColored(ImVec4(0.50f, 0.50f, 0.55f, 0.8f), "[ ? ]");
        }

        // Заголовок и тип
        ImGui::SetCursorPos(ImVec2(56.0f, 10.0f));
        if (ach.isGlobal) {
            ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.30f, 1.0f), "[★ ГЛОБАЛЬНОЕ]");
            ImGui::SameLine();
        } else {
            ImGui::TextColored(ImVec4(0.40f, 0.80f, 1.0f, 1.0f), "[ПРОФИЛЬ]");
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
            ImGui::TextColored(ImVec4(0.30f, 1.0f, 0.45f, 1.0f), "✓ РАЗБЛОКИРОВАНО");
        } else {
            ImGui::TextColored(ImVec4(0.50f, 0.50f, 0.55f, 1.0f), "🔒 В ПРОЦЕССЕ");
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

    quests.push_back({
        "quest_rats_skeletons",
        "Зачистка катакомб",
        "Истребите бродячую нежить на 1 этаже Шпиля.",
        "Охота",
        0, 5, "уничтожено монстров",
        250, 150, "", 0,
        QuestStatus::InProgress
    });

    quests.push_back({
        "quest_mine_ores",
        "Кузнечные пробы",
        "Добудьте куски железной руды в рудной жиле на карте этажа.",
        "Добыча",
        0, 5, "железной руды в инвентаре",
        300, 100, "old_cleaver", 1,
        QuestStatus::InProgress
    });

    quests.push_back({
        "quest_forge_weapon",
        "Искусство горна",
        "Используйте ресурсы кузницы для ковки нового клинка или топора.",
        "Кузница",
        0, 1, "оружие сковано",
        400, 200, "fire_crystal", 2,
        QuestStatus::InProgress
    });

    quests.push_back({
        "quest_reach_floor3",
        "Вглубь Шпиля",
        "Преодолейте сопротивление монстров и откройте путь на 3 этаж (Подводные глубины).",
        "Исследование",
        0, 1, "3 этаж открыт",
        500, 250, "ruby_ring", 1,
        QuestStatus::InProgress
    });

    quests.push_back({
        "quest_slay_elite",
        "Гроза элиты",
        "Сразитесь с элитным командиром монстров и одержите верх.",
        "Охота",
        0, 1, "элитный командир повержен",
        600, 300, "guardian_helm", 1,
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

    int readyCount = 0;
    int completedCount = 0;
    for (const auto& q : quests) {
        if (q.status == QuestStatus::Completed) readyCount++;
        if (q.status == QuestStatus::Claimed) completedCount++;
    }

    ImGui::TextColored(ImVec4(0.40f, 0.85f, 1.0f, 1.0f), "Всего поручений: %zu", quests.size());
    ImGui::SameLine(220.0f);
    if (readyCount > 0) {
        ImGui::TextColored(ImVec4(0.30f, 1.0f, 0.45f, 1.0f), "★ Готовы к сдаче: %d!", readyCount);
    } else {
        ImGui::TextDisabled("Готовы к сдаче: 0");
    }
    ImGui::SameLine(420.0f);
    ImGui::TextColored(ImVec4(0.85f, 0.80f, 0.40f, 1.0f), "Выполнено: %d", completedCount);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    float availW = ImGui::GetContentRegionAvail().x;
    float cardW = std::min(availW, 940.0f);
    float startX = (availW - cardW) * 0.5f;
    if (startX < 0.0f) startX = 0.0f;

    auto& db = GameDatabase::getInstance();

    ImGui::BeginChild("QuestsScrollList", ImVec2(0.0f, -10.0f), false);

    for (size_t i = 0; i < quests.size(); ++i) {
        auto& q = quests[i];
        ImGui::SetCursorPosX(startX);
        ImGui::PushID(static_cast<int>(i));

        ImVec4 bgCol = (q.status == QuestStatus::Completed)
            ? ImVec4(0.12f, 0.20f, 0.16f, 0.95f)
            : (q.status == QuestStatus::Claimed ? ImVec4(0.07f, 0.08f, 0.12f, 0.80f) : ImVec4(0.09f, 0.11f, 0.17f, 0.90f));

        ImVec4 borderCol = (q.status == QuestStatus::Completed)
            ? ImVec4(0.35f, 1.0f, 0.50f, 1.0f)
            : (q.status == QuestStatus::Claimed ? ImVec4(0.30f, 0.35f, 0.45f, 0.60f) : ImVec4(0.25f, 0.55f, 0.85f, 0.85f));

        ImGui::PushStyleColor(ImGuiCol_ChildBg, bgCol);
        ImGui::PushStyleColor(ImGuiCol_Border, borderCol);
        ImGui::BeginChild("QuestCard", ImVec2(cardW, 105.0f), true, ImGuiWindowFlags_NoScrollbar);

        // Категория
        ImVec4 catColor = ImVec4(0.4f, 0.85f, 1.0f, 1.0f);
        if (q.category == "Охота") catColor = ImVec4(1.0f, 0.50f, 0.40f, 1.0f);
        else if (q.category == "Добыча") catColor = ImVec4(0.40f, 0.90f, 1.0f, 1.0f);
        else if (q.category == "Кузница") catColor = ImVec4(1.0f, 0.80f, 0.30f, 1.0f);

        ImGui::TextColored(catColor, "[ %s ]", q.category.c_str());
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
            if (ImGui::Button("★ ЗАБРАТЬ НАГРАДУ##Claim", ImVec2(185.0f, 34.0f))) {
                playerInventory.addFunds(q.rewardGold);
                if (!q.rewardItemId.empty()) {
                    playerInventory.addItem(q.rewardItemId, q.rewardItemCount);
                }
                q.status = QuestStatus::Claimed;
            }
            ImGui::PopStyleColor(2);
        } else if (q.status == QuestStatus::Claimed) {
            ImGui::TextColored(ImVec4(0.50f, 0.50f, 0.55f, 1.0f), "✓ Награда получена");
        } else {
            ImGui::TextColored(ImVec4(0.40f, 0.75f, 1.0f, 1.0f), "В процессе...");
        }

        ImGui::EndChild();
        ImGui::PopStyleColor(2);
        ImGui::PopID();
        ImGui::Spacing();
    }

    ImGui::EndChild();
}

// =============================================================================
// РЕДАКТОР ОТРЯДА
// =============================================================================
"""

if crlf == "\r\n":
    ach_and_quest = ach_and_quest.replace("\n", "\r\n")

text = text[:start_ach] + ach_and_quest + text[end_ach:]

# 4. Character Editor: Enlarged Stats Window on the Right
start_lower = text.find("float colW1 = 330.0f;")
end_lower = text.find("renderSlotPickerModal(currentChar);", start_lower)
assert start_lower != -1 and end_lower != -1, "lower ce markers not found"

new_char_editor_lower = """    float availW = ImGui::GetContentRegionAvail().x;

    // Правая панель характеристик: ширина 400px
    float rightPanelW = std::clamp(availW * 0.29f, 380.0f, 440.0f);
    float leftAreaW = availW - rightPanelW - 24.0f;

    // Делим левую область на 3 колонки:
    // 1. Слоты слева (260px): Оружие, Головной убор, Костюм
    // 2. Витрина героя в центре (340px): анимированная модель, отряд, навыки
    // 3. Слоты справа (260px): Амулет, Кольцо, и Сеты артефактов
    float colW1 = 260.0f;
    float colW2 = std::max(290.0f, leftAreaW - 530.0f);
    float colW3 = 260.0f;

    auto& db = GameDatabase::getInstance();

    auto renderSlotCard = [&](const char* slotTitle, ArtifactType slotType, const std::string& currentItemId, const char* iconKey) {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.09f, 0.11f, 0.17f, 0.85f));
        ImGui::PushStyleColor(ImGuiCol_Border, currentItemId.empty() ? ImVec4(0.35f, 0.40f, 0.50f, 0.6f) : ImVec4(0.85f, 0.70f, 0.25f, 0.9f));
        ImGui::BeginChild((std::string("SlotBox_") + slotTitle).c_str(), ImVec2(0, 104.0f), true, ImGuiWindowFlags_NoScrollbar);

        ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.35f, 1.0f), "%s", slotTitle);
        ImGui::Separator();

        // Иконка слота
        auto itSlotTex = slotIconTextures.find(iconKey);
        if (itSlotTex != slotIconTextures.end() && itSlotTex->second.getNativeHandle() != 0) {
            ImTextureID slotTexId = (ImTextureID)(uintptr_t)itSlotTex->second.getNativeHandle();
            ImGui::Image(slotTexId, ImVec2(50.0f, 50.0f));
            ImGui::SameLine(0.0f, 10.0f);
        }

        ImGui::BeginGroup();
        if (currentItemId.empty()) {
            ImGui::TextDisabled("[ Пустой слот ]");
            if (ImGui::Button("+ Надеть##SlotBtn", ImVec2(100.0f, 24.0f))) {
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

    // --- ЛЕВАЯ И ЦЕНТРАЛЬНАЯ ГРУППА (СНАРЯЖЕНИЕ И МОДЕЛЬ) ---
    ImGui::BeginGroup();
    ImGui::Columns(3, "EditorLeftThreeColumns", false);
    ImGui::SetColumnWidth(0, colW1);
    ImGui::SetColumnWidth(1, colW2);
    ImGui::SetColumnWidth(2, colW3);

    // Колонка 1: Оружие, Шлем, Костюм
    ImGui::TextColored(ImVec4(0.40f, 0.85f, 1.0f, 1.0f), "Снаряжение (Левая рука / Торс):");
    ImGui::Spacing();
    renderSlotCard("⚔ Слот оружия", ArtifactType::Weapon, currentChar->getEquippedWeapon(), "slot_weapon");
    renderSlotCard("👑 Головной убор (Шлем)", ArtifactType::Headgear, currentChar->getEquippedHeadgear(), "slot_headgear");
    renderSlotCard("🥋 Костюм / Доспех", ArtifactType::Costume, currentChar->getEquippedCostume(), "slot_costume");

    ImGui::NextColumn();

    // Колонка 2: Витрина героя с анимированной моделью и навыками
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.08f, 0.09f, 0.14f, 0.85f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.85f, 0.70f, 0.25f, 0.85f));
    ImGui::BeginChild("CenterCharacterShowcase", ImVec2(0, 520.0f), true);

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
        ImGui::TextColored(ImVec4(0.35f, 1.0f, 0.45f, 1.0f), "● В боевом отряде (Слот %d)", static_cast<int>(activeSquadSlot + 1));
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

    // Боевые навыки героя
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

    // Колонка 3: Амулет, Кольцо, и Сеты артефактов
    ImGui::TextColored(ImVec4(0.40f, 0.85f, 1.0f, 1.0f), "Бижутерия и Комплекты:");
    ImGui::Spacing();
    renderSlotCard("📿 Амулет (Талисман)", ArtifactType::Amulet, currentChar->getEquippedAmulet(), "slot_amulet");
    renderSlotCard("💍 Кольцо (Перстень)", ArtifactType::Ring, currentChar->getEquippedRing(), "slot_ring");

    // Панель комплектов артефактов
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.08f, 0.10f, 0.15f, 0.85f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.40f, 0.70f, 0.50f, 0.80f));
    ImGui::BeginChild("ArtifactSetsPanel", ImVec2(0, 275.0f), true);

    ImGui::TextColored(ImVec4(0.45f, 0.95f, 0.55f, 1.0f), "★ Бонусы комплектов:");
    ImGui::Separator();
    ImGui::Spacing();

    auto setCounts = currentChar->getActiveSetCounts();
    if (setCounts.empty()) {
        ImGui::TextDisabled("Сеты артефактов не активированы.");
        ImGui::Spacing();
        ImGui::TextDisabled("Экипируйте 2 или 4 предмета одного сета для получения сильных пассивных бонусов!");
    } else {
        for (const auto& [setId, count] : setCounts) {
            auto itSet = db.artifactSets.find(setId);
            if (itSet == db.artifactSets.end()) continue;

            ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.35f, 1.0f), "«%s» (%d/4):", itSet->second.name.c_str(), count);
            if (count >= 2) {
                ImGui::TextColored(ImVec4(0.35f, 1.0f, 0.45f, 1.0f), " [2/4] ✓ %s", itSet->second.twoPieceDesc.c_str());
            } else {
                ImGui::TextDisabled(" [2/4] 🔒 %s (+%d шт.)", itSet->second.twoPieceDesc.c_str(), 2 - count);
            }
            if (count >= 4) {
                ImGui::TextColored(ImVec4(0.35f, 1.0f, 0.45f, 1.0f), " [4/4] ✓ %s", itSet->second.fourPieceDesc.c_str());
            } else {
                ImGui::TextDisabled(" [4/4] 🔒 %s (+%d шт.)", itSet->second.fourPieceDesc.c_str(), 4 - count);
            }
            ImGui::Spacing();
        }
    }

    ImGui::EndChild();
    ImGui::PopStyleColor(2);

    ImGui::Columns(1);
    ImGui::EndGroup();

    // =========================================================================
    // БОЛЬШОЕ ОКНО ИТОГОВЫХ ХАРАКТЕРИСТИК В ПРАВОЙ ЧАСТИ ЭКРАНА
    // =========================================================================
    ImGui::SameLine(0.0f, 16.0f);

    StatArray totalStats = currentChar->getTotalStats();

    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.08f, 0.10f, 0.16f, 0.95f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.35f, 0.70f, 0.95f, 0.95f));
    ImGui::BeginChild("CharacterTotalStatsPanelLarge", ImVec2(rightPanelW, 560.0f), true);

    ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.30f, 1.0f), "=== ИТОГОВЫЕ ХАРАКТЕРИСТИКИ ===");
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::TextColored(ImVec4(0.95f, 0.90f, 0.60f, 1.0f), "Герой: %s", currentChar->getName().c_str());
    ImGui::TextColored(ImVec4(0.40f, 0.85f, 1.0f, 1.0f), "Уровень: %d  |  Опыт: %d / %d",
        currentChar->getLevel(), currentChar->getExp(), currentChar->getExpForNextLevel());
    ImGui::Spacing();

    // Полосы здоровья и маны
    float hpRatio = static_cast<float>(currentChar->getCurrentHp() / std::max(1.0, totalStats[StatIndex::HP]));
    ImGui::TextColored(ImVec4(0.35f, 1.0f, 0.45f, 1.0f), "Здоровье (HP): %.0f / %.0f",
        currentChar->getCurrentHp(), totalStats[StatIndex::HP]);
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.25f, 0.85f, 0.40f, 1.0f));
    ImGui::ProgressBar(std::clamp(hpRatio, 0.0f, 1.0f), ImVec2(-1.0f, 14.0f), "");
    ImGui::PopStyleColor();

    float manaRatio = static_cast<float>(currentChar->getCurrentMana() / std::max(1.0, totalStats[StatIndex::Mana]));
    ImGui::TextColored(ImVec4(0.35f, 0.80f, 1.0f, 1.0f), "Мана: %.0f / %.0f",
        currentChar->getCurrentMana(), totalStats[StatIndex::Mana]);
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.20f, 0.55f, 0.95f, 1.0f));
    ImGui::ProgressBar(std::clamp(manaRatio, 0.0f, 1.0f), ImVec2(-1.0f, 14.0f), "");
    ImGui::PopStyleColor();

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Боевые параметры
    ImGui::TextColored(ImVec4(1.0f, 0.65f, 0.30f, 1.0f), "⚔  Сила атаки: %.1f", totalStats[StatIndex::Attack]);

    double mitigation = (totalStats[StatIndex::Defense] / (totalStats[StatIndex::Defense] + 100.0)) * 100.0;
    ImGui::TextColored(ImVec4(0.40f, 0.85f, 1.0f, 1.0f), "🛡  Защита: %.1f  (-%.1f%% урона)", totalStats[StatIndex::Defense], mitigation);

    ImGui::Text("⚡  Критический шанс: %.1f%%", totalStats[StatIndex::CritChance] * 100.0);
    ImGui::Text("💥  Критический урон: %.0f%%", totalStats[StatIndex::CritDamage] * 100.0);
    ImGui::Text("🩸  Вампиризм (хил от атак): %.1f%%", totalStats[StatIndex::Vampirism] * 100.0);
    ImGui::Text("🔥  Бонус к урону умений: +%.1f%%", totalStats[StatIndex::DamageBonus] * 100.0);

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
    ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.25f, 1.0f), "★  Боевая мощь героя: %d", powerScore);

    ImGui::EndChild();
    ImGui::PopStyleColor(2);

    """

if crlf == "\r\n":
    new_char_editor_lower = new_char_editor_lower.replace("\n", "\r\n")

text = text[:start_lower] + new_char_editor_lower + text[end_lower:]

with open(cpp_path, "w", encoding="utf-8") as f:
    f.write(text)

print("apply_complete_patch successfully executed!")
