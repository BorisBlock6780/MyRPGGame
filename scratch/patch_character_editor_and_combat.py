import os

cpp_path = r"src\ScreenManager.cpp"
with open(cpp_path, "r", encoding="utf-8") as f:
    content = f.read()

# Replace the stub renderCharacterEditor with full implementation + renderSlotPickerModal
old_editor_stub = """// =============================================================================
// РЕДАКТОР ПЕРСОНАЖА
// =============================================================================
void ScreenManager::renderCharacterEditor() {
    ImGui::Text("=== РЕДАКТОР ПЕРСОНАЖА ===");
    ImGui::SameLine(ImGui::GetWindowWidth() - 200.0f);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.45f, 0.85f, 1.0f));
    if (ImGui::Button("<- В Лобби##CharBack", ImVec2(170.0f, 0.0f))) {
        currentScreen = ScreenState::Lobby;
    }
    ImGui::PopStyleColor();
    ImGui::Separator(); 
}"""

if old_editor_stub not in content:
    old_editor_stub = old_editor_stub.replace("\n", "\r\n")

new_editor_impl = """// =============================================================================
// РЕДАКТОР ПЕРСОНАЖА И СИСТЕМА ЭКИПИРОВКИ
// =============================================================================

void ScreenManager::renderSlotPickerModal(std::shared_ptr<PlayerCharacter> character) {
    if (!character || activeSlotPicker == ArtifactType::None) return;

    if (ImGui::BeginPopupModal("##EquipItemModal", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
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

        auto& db = GameDatabase::getInstance();
        bool anyFound = false;

        ImGui::BeginChild("EligibleItemsScroll", ImVec2(520.0f, 300.0f), true);

        if (activeSlotPicker == ArtifactType::Weapon) {
            for (const auto& [wId, wTmpl] : db.weapons) {
                int count = playerInventory.getItemCount(wId);
                if (count > 0) {
                    anyFound = true;
                    ImGui::PushID(wId.c_str());
                    ImGui::TextColored(ImVec4(0.35f, 0.85f, 1.0f, 1.0f), "⚔ %s (В наличии: %d шт.)",
                        wTmpl->getName().c_str(), count);
                    ImGui::TextDisabled("  Урон на ур.%d: +%.0f | %s",
                        character->getLevel(), wTmpl->getDamageAtLevel(character->getLevel()),
                        wTmpl->getEffectDesc().c_str());
                    if (ImGui::Button("Надеть в слот оружия##PickWeapon", ImVec2(180.0f, 26.0f))) {
                        character->equipWeapon(wId, playerInventory);
                        activeSlotPicker = ArtifactType::None;
                        ImGui::CloseCurrentPopup();
                    }
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
                        ImGui::TextColored(ImVec4(0.95f, 0.80f, 0.35f, 1.0f), "✦ %s (В наличии: %d шт.)",
                            artTmpl->getName().c_str(), count);
                        if (!artTmpl->getSetId().empty()) {
                            auto itSet = db.artifactSets.find(artTmpl->getSetId());
                            if (itSet != db.artifactSets.end()) {
                                ImGui::TextColored(ImVec4(0.45f, 0.95f, 0.55f, 1.0f), "  Комплект: «%s»",
                                    itSet->second.name.c_str());
                            }
                        }
                        ImGui::TextDisabled("  Бонусы: %s", artTmpl->getEffectDesc().c_str());
                        if (ImGui::Button("Надеть в слот артефакта##PickArtifact", ImVec2(180.0f, 26.0f))) {
                            character->equipArtifact(activeSlotPicker, artId, playerInventory);
                            activeSlotPicker = ArtifactType::None;
                            ImGui::CloseCurrentPopup();
                        }
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
            ImGui::TextDisabled("Команда 'give all' в консоли выдаст все комплекты артефактов и оружие.");
        }

        ImGui::EndChild();
        ImGui::Spacing();

        if (ImGui::Button("Закрыть окно##ClosePicker", ImVec2(140.0f, 28.0f))) {
            activeSlotPicker = ArtifactType::None;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
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
    // 1. ГАЛЕРЕЯ ПОРТРЕТОВ ПЕРСОНАЖЕЙ (ПОРТРЕТЫ С АВАТАРАМИ ВМЕСТО ПРОСТОГО СПИСКА)
    // =========================================================================
    ImGui::TextColored(ImVec4(0.40f, 0.85f, 1.0f, 1.0f), "ГЕРОИ ОТРЯДА (Выберите портрет для редактирования экипировки):");
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

        // Портрет героя
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
            ImGui::TextColored(ImVec4(0.35f, 1.0f, 0.45f, 1.0f), "● В отряде");
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
    // 2. РАБОЧАЯ ЗОНА: 3 КОЛОНКИ СЛОТОВ И ВЫБРАННЫЙ ПЕРСОНАЖ В ЦЕНТРЕ
    // =========================================================================
    float totalW = ImGui::GetContentRegionAvail().x;
    float colW1 = 340.0f;
    float colW3 = 340.0f;
    float colW2 = totalW - colW1 - colW3 - 24.0f;

    ImGui::Columns(3, "EditorThreeColumns", false);
    ImGui::SetColumnWidth(0, colW1);
    ImGui::SetColumnWidth(1, colW2);
    ImGui::SetColumnWidth(2, colW3);

    auto& db = GameDatabase::getInstance();

    auto renderSlotCard = [&](const char* slotTitle, ArtifactType slotType, const std::string& currentItemId, const char* iconKey) {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.09f, 0.11f, 0.17f, 0.85f));
        ImGui::PushStyleColor(ImGuiCol_Border, currentItemId.empty() ? ImVec4(0.35f, 0.40f, 0.50f, 0.6f) : ImVec4(0.85f, 0.70f, 0.25f, 0.9f));
        ImGui::BeginChild((std::string("SlotBox_") + slotTitle).c_str(), ImVec2(0, 104.0f), true, ImGuiWindowFlags_NoScrollbar);

        ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.35f, 1.0f), "%s", slotTitle);
        ImGui::Separator();

        // Slot Icon
        auto itSlotTex = slotIconTextures.find(iconKey);
        if (itSlotTex != slotIconTextures.end() && itSlotTex->second.getNativeHandle() != 0) {
            ImTextureID slotTexId = (ImTextureID)(uintptr_t)itSlotTex->second.getNativeHandle();
            ImGui::Image(slotTexId, ImVec2(52.0f, 52.0f));
            ImGui::SameLine(0.0f, 10.0f);
        }

        ImGui::BeginGroup();
        if (currentItemId.empty()) {
            ImGui::TextDisabled("[ Слот пуст ]");
            if (ImGui::Button("+ Надеть##SlotBtn", ImVec2(100.0f, 24.0f))) {
                activeSlotPicker = slotType;
                ImGui::OpenPopup("##EquipItemModal");
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
                ImGui::OpenPopup("##EquipItemModal");
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
    // КОЛОНКА 1 (СЛЕВА): ОРУЖИЕ, КОСТЮМ И СЕТ-БОНУСЫ
    // =========================================================================
    ImGui::TextColored(ImVec4(0.40f, 0.85f, 1.0f, 1.0f), "СНАРЯЖЕНИЕ (СЛЕВА):");
    ImGui::Spacing();

    // 1. Слот Оружия
    renderSlotCard("⚔ СЛОТ ОРУЖИЯ (1 Оружие)", ArtifactType::Weapon, currentChar->getEquippedWeapon(), "slot_weapon");

    // 2. Слот Костюма / Доспеха
    renderSlotCard("🛡 СЛОТ КОСТЮМА (Костюм/Доспех)", ArtifactType::Costume, currentChar->getEquippedCostume(), "slot_costume");

    // 3. Блок Бонусов за Комплекты
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.08f, 0.10f, 0.15f, 0.85f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.40f, 0.70f, 0.50f, 0.80f));
    ImGui::BeginChild("ArtifactSetsPanel", ImVec2(0, 190.0f), true);

    ImGui::TextColored(ImVec4(0.45f, 0.95f, 0.55f, 1.0f), "✦ БОНУСЫ КОМПЛЕКТОВ АРТЕФАКТОВ:");
    ImGui::Separator();
    ImGui::Spacing();

    auto setCounts = currentChar->getActiveSetCounts();
    if (setCounts.empty()) {
        ImGui::TextDisabled("Нет активных предметов из комплектов.");
        ImGui::TextDisabled("Экипируйте 2 или 4 предмета одного сета для получения мощных бонусов!");
    } else {
        for (const auto& [setId, count] : setCounts) {
            auto itSet = db.artifactSets.find(setId);
            if (itSet == db.artifactSets.end()) continue;

            ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.35f, 1.0f), "Комплект «%s» (%d / 4):",
                itSet->second.name.c_str(), count);

            // Бонус за 2 предмета
            if (count >= 2) {
                ImGui::TextColored(ImVec4(0.35f, 1.0f, 0.45f, 1.0f), " ✔ %s", itSet->second.twoPieceDesc.c_str());
            } else {
                ImGui::TextDisabled(" ○ %s (не хватает %d предм.)", itSet->second.twoPieceDesc.c_str(), 2 - count);
            }

            // Бонус за 4 предмета
            if (count >= 4) {
                ImGui::TextColored(ImVec4(0.35f, 1.0f, 0.45f, 1.0f), " ✔ %s", itSet->second.fourPieceDesc.c_str());
            } else {
                ImGui::TextDisabled(" ○ %s (не хватает %d предм.)", itSet->second.fourPieceDesc.c_str(), 4 - count);
            }
            ImGui::Spacing();
        }
    }

    ImGui::EndChild();
    ImGui::PopStyleColor(2);

    ImGui::NextColumn();

    // =========================================================================
    // КОЛОНКА 2 (ЦЕНТР): ПЕРСОНАЖ, АНИМАЦИЯ, УМЕНИЯ И УПРАВЛЕНИЕ ОТРЯДОМ
    // =========================================================================
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.08f, 0.09f, 0.14f, 0.85f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.85f, 0.70f, 0.25f, 0.85f));
    ImGui::BeginChild("CenterCharacterShowcase", ImVec2(0, 430.0f), true);

    ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.25f, 1.0f), "ВЫБРАН ДЛЯ РЕДАКТИРОВАНИЯ: %s", currentChar->getName().c_str());
    ImGui::TextDisabled("Уровень: %d  |  ID: %s", currentChar->getLevel(), currentChar->getId().c_str());
    ImGui::Separator();
    ImGui::Spacing();

    // Анимированная модель героя на постаменте
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 previewCenter = ImGui::GetCursorScreenPos();
    previewCenter.x += (colW2 * 0.5f) - 10.0f;
    previewCenter.y += 65.0f;

    float animTime = static_cast<float>(ImGui::GetTime());
    renderAnimatedHeroModel(drawList, previewCenter, currentChar->getId(), true, animTime, 1.35f);

    ImGui::Dummy(ImVec2(0, 130.0f));

    // Управление позицией в отряде
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
        ImGui::TextColored(ImVec4(0.35f, 1.0f, 0.45f, 1.0f), "★ Персонаж находится в боевом отряде (Слот %d)", static_cast<int>(activeSquadSlot + 1));
        if (ImGui::Button("Убрать из активного отряда##RemoveSq", ImVec2(240.0f, 26.0f))) {
            playerProfile.removeFromSquad(activeSquadSlot);
        }
    } else {
        ImGui::TextDisabled("Персонаж в резерве. Можно поставить в активный отряд:");
        if (ImGui::Button("Поставить в активный отряд##AddSq", ImVec2(240.0f, 26.0f))) {
            // Ищем свободный слот
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

    // Список навыков выбранного героя
    auto charTmplIt = db.characters.find(currentChar->getId());
    if (charTmplIt != db.characters.end()) {
        ImGui::TextColored(ImVec4(0.40f, 0.85f, 1.0f, 1.0f), "Способности персонажа:");
        const auto& skills = charTmplIt->second->getSkills();
        for (size_t sk = 0; sk < skills.size(); ++sk) {
            std::string skTag = (sk == 0) ? "[Обычная атака] " : (sk == 1) ? "[Навык] " : "[УЛЬТИМЕЙТ] ";
            ImGui::TextColored(ImVec4(0.95f, 0.90f, 0.70f, 1.0f), "%s%s", skTag.c_str(), skills[sk]->getName().c_str());
            if (!skills[sk]->getDescription().empty()) {
                ImGui::TextDisabled("  %s", skills[sk]->getDescription().c_str());
            }
        }
    }

    ImGui::EndChild();
    ImGui::PopStyleColor(2);

    ImGui::NextColumn();

    // =========================================================================
    // КОЛОНКА 3 (СПРАВА): ГОЛОВНОЙ УБОР, АМУЛЕТ, КОЛЬЦО И ИТОГОВЫЕ СТАТЫ
    // =========================================================================
    ImGui::TextColored(ImVec4(0.40f, 0.85f, 1.0f, 1.0f), "АРТЕФАКТЫ (СПРАВА):");
    ImGui::Spacing();

    // 1. Слот Головного убора
    renderSlotCard("🪖 ГОЛОВНОЙ УБОР (Шлем/Капюшон)", ArtifactType::Headgear, currentChar->getEquippedHeadgear(), "slot_headgear");

    // 2. Слот Амулета
    renderSlotCard("📿 АМУЛЕТ (Талисман/Кулон)", ArtifactType::Amulet, currentChar->getEquippedAmulet(), "slot_amulet");

    // 3. Слот Кольца
    renderSlotCard("💍 КОЛЬЦО (Перстень/Печатка)", ArtifactType::Ring, currentChar->getEquippedRing(), "slot_ring");

    // 4. Сводка Итоговых Характеристик с учетом экипировки и бонусов
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.08f, 0.10f, 0.15f, 0.85f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.35f, 0.65f, 0.85f, 0.80f));
    ImGui::BeginChild("CharacterTotalStatsPanel", ImVec2(0, 190.0f), true);

    StatArray totalStats = currentChar->getTotalStats();

    ImGui::TextColored(ImVec4(0.40f, 0.85f, 1.0f, 1.0f), "ИТОГОВЫЕ ХАРАКТЕРИСТИКИ:");
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("Здоровье (HP): %.0f", totalStats[StatIndex::HP]);
    ImGui::Text("Мана: %.0f", totalStats[StatIndex::Mana]);
    ImGui::TextColored(ImVec4(0.35f, 1.0f, 0.45f, 1.0f), "Атака: %.1f", totalStats[StatIndex::Attack]);
    ImGui::TextColored(ImVec4(0.35f, 0.85f, 1.0f, 1.0f), "Защита: %.1f", totalStats[StatIndex::Defense]);
    ImGui::Text("Крит. шанс: %.1f%%", totalStats[StatIndex::CritChance] * 100.0);
    ImGui::Text("Крит. урон: %.0f%%", totalStats[StatIndex::CritDamage] * 100.0);
    ImGui::Text("Вампиризм: %.1f%%", totalStats[StatIndex::Vampirism] * 100.0);
    ImGui::Text("Бонус урона: +%.1f%%", totalStats[StatIndex::DamageBonus] * 100.0);

    ImGui::EndChild();
    ImGui::PopStyleColor(2);

    ImGui::Columns(1);

    // Модальное окно выбора предметов
    renderSlotPickerModal(currentChar);
}"""

assert old_editor_stub in content, "Could not find old_editor_stub"
content = content.replace(old_editor_stub, new_editor_impl, 1)

with open(cpp_path, "w", encoding="utf-8") as f:
    f.write(content)

print("Character Editor successfully implemented in ScreenManager.cpp!")
