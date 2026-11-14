import os
import re

cpp_path = r"src\ScreenManager.cpp"
with open(cpp_path, "r", encoding="utf-8") as f:
    content = f.read()

# 1. Update constructor
m_ctor = re.search(r'(ScreenManager::ScreenManager\(\)\s*\{[\s\S]*?logConsole\([^;]+;\s*logConsole\([^;]+;\s*)(\})', content)
assert m_ctor, "m_ctor not found"

ctor_extra = """    initAchievements();
    loadGlobalAchievements();
    initQuests();
}"""

if "\r\n" in m_ctor.group(0):
    ctor_extra = ctor_extra.replace("\n", "\r\n")

content = content[:m_ctor.start(2)] + ctor_extra

# 2. Update renderLobby targets
m_lobby = re.search(r'NavTarget targets\[\]\s*=\s*\{[\s\S]*?for \(int i = 0; i < count; \+\+i\) \{[\s\S]*?ImGui::SetCursorPos\(ImVec2\(posX, posY\)\);', content)
assert m_lobby, "m_lobby not found"

new_lobby_code = """NavTarget targets[] = {
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

if "\r\n" in m_lobby.group(0):
    new_lobby_code = new_lobby_code.replace("\n", "\r\n")

content = content[:m_lobby.start()] + new_lobby_code + content[m_lobby.end():]
print("Updated renderLobby targets!")

# 3. Replace renderAchievements stub with full Achievements & Quests implementation
m_ach = re.search(r'void ScreenManager::renderAchievements\(\)\s*\{[\s\S]*?ImGui::Separator\(\);\s*\}', content)
assert m_ach, "m_ach not found"

ach_and_quest_impl = """void ScreenManager::initAchievements() {
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
        true // Стартовое достижение
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
}"""

if "\r\n" in m_ach.group(0):
    ach_and_quest_impl = ach_and_quest_impl.replace("\n", "\r\n")

content = content[:m_ach.start()] + ach_and_quest_impl + content[m_ach.end():]
print("Achievements and Quests implementations added!")

with open(cpp_path, "w", encoding="utf-8") as f:
    f.write(content)

print("Part 2 completed!")
