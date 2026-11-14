import re

cpp_path = r"src\ScreenManager.cpp"
with open(cpp_path, "r", encoding="utf-8") as f:
    text = f.read()

# -----------------------------------------------------------------------------
# 1. LOBBY CLEAN LABELS
# -----------------------------------------------------------------------------
old_lobby_targets = """    NavTarget targets[] = {
        {"[ [Карта] ] Карта этажей",           "Исследование 10 этажей, битвы, торговцы и святилища", ScreenState::Map,             ImVec4(0.20f, 0.50f, 0.80f, 1.0f)},
        {"[ [Отряд] ] Редактор отряда",       "Формирование ударной группы из 3 боевых героев отряда",  ScreenState::SquadEditor,     ImVec4(0.25f, 0.65f, 0.35f, 1.0f)},
        {"[ [Герои] ] Герои и слоты",        "Смена экипировки, изучение артефактов и сетов", ScreenState::CharacterEditor, ImVec4(0.65f, 0.40f, 0.80f, 1.0f)},
        {"[ [Сумка] ] Инвентарь и припасы",  "Снаряжение, драгоценные металлы, минералы и еда", ScreenState::Inventory,       ImVec4(0.85f, 0.60f, 0.20f, 1.0f)},
        {"[ [Награды] ] Зал достижений",        "Список наград, подвигов и бонусов за испытания",     ScreenState::Achievements,    ImVec4(0.90f, 0.75f, 0.25f, 1.0f)},
        {"[ [Задания] ] Задания и квесты",      "Журнал поручений, охота на монстров и награды",       ScreenState::Quests,          ImVec4(0.30f, 0.70f, 0.85f, 1.0f)}
    };"""

new_lobby_targets = """    NavTarget targets[] = {
        {"Карта этажей",           "Исследование 10 этажей, битвы, торговцы и святилища", ScreenState::Map,             ImVec4(0.20f, 0.50f, 0.80f, 1.0f)},
        {"Редактор отряда",       "Формирование ударной группы из 3 боевых героев отряда",  ScreenState::SquadEditor,     ImVec4(0.25f, 0.65f, 0.35f, 1.0f)},
        {"Герои и экипировка",    "Смена экипировки, изучение артефактов и сетов", ScreenState::CharacterEditor, ImVec4(0.65f, 0.40f, 0.80f, 1.0f)},
        {"Инвентарь и припасы",  "Снаряжение, драгоценные металлы, минералы и еда", ScreenState::Inventory,       ImVec4(0.85f, 0.60f, 0.20f, 1.0f)},
        {"Зал достижений",        "Список наград, подвигов и бонусов за испытания",     ScreenState::Achievements,    ImVec4(0.90f, 0.75f, 0.25f, 1.0f)},
        {"Задания и квесты",      "Журнал поручений, охота на монстров и награды",       ScreenState::Quests,          ImVec4(0.30f, 0.70f, 0.85f, 1.0f)}
    };"""

if old_lobby_targets in text:
    text = text.replace(old_lobby_targets, new_lobby_targets)
    print("1. Lobby clean labels applied!")
else:
    print("1. Old lobby targets not found directly, trying normalized...")
    # Normalize CRLF
    old_norm = old_lobby_targets.replace("\n", "\r\n")
    new_norm = new_lobby_targets.replace("\n", "\r\n")
    if old_norm in text:
        text = text.replace(old_norm, new_norm)
        print("1. Lobby clean labels applied (CRLF)!")
    else:
        print("1. Warning: lobby targets replacement failed!")

# -----------------------------------------------------------------------------
# 2. QUESTS MASTER-DETAIL VIEW
# -----------------------------------------------------------------------------
old_render_quests_pattern = r'void ScreenManager::renderQuests\(\)\s*\{[\s\S]*?ImGui::EndChild\(\);\s*\}'

new_render_quests_code = """void ScreenManager::renderQuests() {
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
            if (itItem != db.items.end() && !itItem->second->getDescription().empty()) {
                ImGui::TextDisabled("%s", itItem->second->getDescription().c_str());
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
}"""

m_quests = re.search(old_render_quests_pattern, text)
if m_quests:
    if "\r\n" in m_quests.group(0):
        new_render_quests_code = new_render_quests_code.replace("\n", "\r\n")
    text = text[:m_quests.start()] + new_render_quests_code + text[m_quests.end():]
    print("2. Quests master-detail overhaul applied!")
else:
    print("2. Warning: old_render_quests_pattern not matched!")

# -----------------------------------------------------------------------------
# 3. SLOT PICKER MODAL (ADD UNEQUIP BUTTON)
# -----------------------------------------------------------------------------
old_modal_start = """        ImGui::Separator();
        ImGui::Spacing();

        auto& db = GameDatabase::getInstance();
        bool anyFound = false;

        ImGui::BeginChild("EligibleItemsScroll", ImVec2(520.0f, 320.0f), true);"""

new_modal_start = """        ImGui::Separator();
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

        ImGui::BeginChild("EligibleItemsScroll", ImVec2(520.0f, 300.0f), true);"""

if old_modal_start in text:
    text = text.replace(old_modal_start, new_modal_start)
    print("3. Slot picker modal unequip button added!")
else:
    old_m_norm = old_modal_start.replace("\n", "\r\n")
    new_m_norm = new_modal_start.replace("\n", "\r\n")
    if old_m_norm in text:
        text = text.replace(old_m_norm, new_m_norm)
        print("3. Slot picker modal unequip button added (CRLF)!")
    else:
        print("3. Warning: old_modal_start not found!")

# -----------------------------------------------------------------------------
# 4. CHARACTER EDITOR OVERHAUL (CENTERED, NARROW STATS, WHOLE CARD BUTTON, NO TAGS, NO EQUIP LIST)
# -----------------------------------------------------------------------------
old_ce_lower_pattern = r'float availW = ImGui::GetContentRegionAvail\(\)\.x;\s*// Правая панель характеристик[\s\S]*?renderSlotPickerModal\(currentChar\);\s*\}'

new_ce_lower_code = """    float availW = ImGui::GetContentRegionAvail().x;

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
}"""

m_ce_lower = re.search(old_ce_lower_pattern, text)
if m_ce_lower:
    if "\r\n" in m_ce_lower.group(0):
        new_ce_lower_code = new_ce_lower_code.replace("\n", "\r\n")
    text = text[:m_ce_lower.start()] + new_ce_lower_code + text[m_ce_lower.end():]
    print("4. Character editor overhaul applied!")
else:
    print("4. Warning: old_ce_lower_pattern not matched!")

# -----------------------------------------------------------------------------
# 5. REMOVE OTHER RESIDUAL BRACKET TAGS
# -----------------------------------------------------------------------------
tags_cleanups = [
    ('combatAnim.isCrit ? "[*] Крит! -" : "-"', 'combatAnim.isCrit ? "Крит! -" : "-"'),
    ('IM_COL32(100, 220, 140, 220), "[Герои] ОТРЯД ГЕРОЕВ")', 'IM_COL32(100, 220, 140, 220), "ОТРЯД ГЕРОЕВ")'),
    ('IM_COL32(255, 90, 90, 220), "[Босс] ВРАГИ")', 'IM_COL32(255, 90, 90, 220), "ВРАГИ")'),
    ('"[[Щит] +%.0f]"', '"Щит: +%.0f"'),
    ('"[Крит] %s"', '"%s"'),
    ('"[*] Стаки умения: %d (бонус урона по площади)"', '"Стаки умения: %d (бонус урона по площади)"'),
    ('"[ [Цель] Цель ]##TargetActive"', '"Текущая цель##TargetActive"'),
    ('"[Крит] Умения героя:"', '"Умения героя:"'),
    ('"[[*] Достижения: %d/%d]  [Очки: %d/%d]"', '"Достижения: %d/%d  |  Очки: %d/%d"'),
    ('filterBtn("[*] Особые (глобальные)", 1);', 'filterBtn("Особые (глобальные)", 1);'),
    ('"[[*] Глобальное]"', '"Глобальное"'),
    ('"[+] Получено"', '"Получено"'),
    ('"[x] Не получено"', '"Не получено"'),
    ('"[УДАР]  Ударить киркой по жиле!##Strike"', '"Ударить киркой по жиле!##Strike"'),
    ('"[*] Успешный удар!"', '"Успешный удар!"')
]

for old_s, new_s in tags_cleanups:
    if old_s in text:
        text = text.replace(old_s, new_s)
        print(f"Cleaned tag: {old_s[:30]}...")

with open(cpp_path, "w", encoding="utf-8") as f:
    f.write(text)

print("Redesign patch completed!")
