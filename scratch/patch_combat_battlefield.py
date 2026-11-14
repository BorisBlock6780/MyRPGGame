import os

cpp_path = r"src\ScreenManager.cpp"
with open(cpp_path, "r", encoding="utf-8") as f:
    content = f.read()

battlefield_methods = """
// =============================================================================
// ПРОЦЕДУРНЫЕ АНИМИРОВАННЫЕ МОДЕЛИ БОЯ (ГЕРОИ И ПРОТИВНИКИ)
// =============================================================================

void ScreenManager::renderAnimatedHeroModel(ImDrawList* drawList, const ImVec2& center, const std::string& charId, bool isActive, float animTime, float scale) {
    float bob = sinf(animTime * 3.5f + (isActive ? 0.0f : 1.2f)) * 4.5f * scale;
    float breath = sinf(animTime * 2.5f) * 1.5f * scale;
    ImVec2 pos = ImVec2(center.x, center.y + bob);

    // 1. Тень под ногами
    drawList->AddEllipseFilled(ImVec2(center.x, center.y + 40.0f * scale), 26.0f * scale, 9.0f * scale, IM_COL32(0, 0, 0, 115));

    // 2. Свечение активного героя
    if (isActive) {
        float p = sinf(animTime * 4.0f) * 0.25f + 0.75f;
        drawList->AddCircle(ImVec2(center.x, center.y + 40.0f * scale), 30.0f * scale, IM_COL32(255, 215, 60, (int)(160 * p)), 32, 2.5f);
        drawList->AddCircleFilled(ImVec2(pos.x, pos.y), 34.0f * scale, IM_COL32(255, 220, 80, (int)(35 * p)), 24);
    }

    // 3. Цветовая палитра по персонажу
    ImU32 bodyCol = IM_COL32(65, 75, 95, 255);
    ImU32 trimCol = IM_COL32(180, 195, 225, 255);
    ImU32 eyeCol = IM_COL32(255, 225, 100, 255);
    bool isMage = (charId == "adeir" || charId == "mira" || charId == "silas" || charId == "elix" || charId == "lumien");
    bool isTank = (charId == "garosh" || charId == "born" || charId == "tauros");

    if (charId == "adeir") { bodyCol = IM_COL32(140, 40, 25, 255); trimCol = IM_COL32(255, 160, 50, 255); eyeCol = IM_COL32(255, 200, 40, 255); }
    else if (charId == "renwick") { bodyCol = IM_COL32(50, 70, 110, 255); trimCol = IM_COL32(180, 210, 255, 255); eyeCol = IM_COL32(100, 220, 255, 255); }
    else if (charId == "garosh") { bodyCol = IM_COL32(50, 55, 65, 255); trimCol = IM_COL32(190, 195, 205, 255); eyeCol = IM_COL32(255, 120, 50, 255); }
    else if (charId == "vex") { bodyCol = IM_COL32(30, 80, 45, 255); trimCol = IM_COL32(110, 230, 140, 255); eyeCol = IM_COL32(140, 255, 170, 255); }

    // Корпус
    if (isMage) {
        // Robe cone
        drawList->AddTriangleFilled(
            ImVec2(pos.x, pos.y - 18.0f * scale),
            ImVec2(pos.x - (18.0f + breath) * scale, pos.y + 36.0f * scale),
            ImVec2(pos.x + (18.0f + breath) * scale, pos.y + 36.0f * scale),
            bodyCol);
        drawList->AddTriangle(
            ImVec2(pos.x, pos.y - 18.0f * scale),
            ImVec2(pos.x - (18.0f + breath) * scale, pos.y + 36.0f * scale),
            ImVec2(pos.x + (18.0f + breath) * scale, pos.y + 36.0f * scale),
            trimCol, 2.0f);
        // Посох с магической сферой
        float staffBob = sinf(animTime * 4.0f) * 3.0f * scale;
        ImVec2 staffTop = ImVec2(pos.x + 22.0f * scale, pos.y - (28.0f + staffBob) * scale);
        drawList->AddLine(staffTop, ImVec2(pos.x + 18.0f * scale, pos.y + 36.0f * scale), IM_COL32(140, 95, 55, 255), 3.0f * scale);
        drawList->AddCircleFilled(staffTop, 7.0f * scale, trimCol);
        drawList->AddCircle(staffTop, 9.0f * scale, IM_COL32(255, 255, 255, 200), 16, 1.5f);
    } else if (isTank) {
        // Spiked Heavy Armor Body
        drawList->AddRectFilled(
            ImVec2(pos.x - (22.0f + breath) * scale, pos.y - 16.0f * scale),
            ImVec2(pos.x + (22.0f + breath) * scale, pos.y + 34.0f * scale),
            bodyCol, 6.0f * scale);
        drawList->AddRect(
            ImVec2(pos.x - (22.0f + breath) * scale, pos.y - 16.0f * scale),
            ImVec2(pos.x + (22.0f + breath) * scale, pos.y + 34.0f * scale),
            trimCol, 6.0f * scale, 0, 2.5f);
        // Tower Shield
        ImVec2 sCenter = ImVec2(pos.x - 22.0f * scale, pos.y + 8.0f * scale);
        drawList->AddRectFilled(ImVec2(sCenter.x - 10.0f * scale, sCenter.y - 24.0f * scale),
                               ImVec2(sCenter.x + 10.0f * scale, sCenter.y + 24.0f * scale),
                               IM_COL32(40, 45, 55, 255), 4.0f * scale);
        drawList->AddRect(ImVec2(sCenter.x - 10.0f * scale, sCenter.y - 24.0f * scale),
                          ImVec2(sCenter.x + 10.0f * scale, sCenter.y + 24.0f * scale),
                          trimCol, 4.0f * scale, 0, 2.0f);
    } else {
        // Warrior / Rogue Form
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
        // Gleaming Sword
        ImVec2 swordTip = ImVec2(pos.x + 24.0f * scale, pos.y - 24.0f * scale);
        drawList->AddLine(ImVec2(pos.x + 16.0f * scale, pos.y + 12.0f * scale), swordTip, IM_COL32(230, 240, 255, 255), 3.0f * scale);
        drawList->AddLine(ImVec2(pos.x + 12.0f * scale, pos.y + 16.0f * scale), ImVec2(pos.x + 20.0f * scale, pos.y + 8.0f * scale), IM_COL32(210, 160, 50, 255), 3.0f * scale);
    }

    // Голова / Шлем
    drawList->AddCircleFilled(ImVec2(pos.x, pos.y - 26.0f * scale), 12.0f * scale, bodyCol);
    drawList->AddCircle(ImVec2(pos.x, pos.y - 26.0f * scale), 12.0f * scale, trimCol, 24, 2.0f);

    // Светящиеся глаза / визор
    drawList->AddEllipseFilled(ImVec2(pos.x + 4.0f * scale, pos.y - 27.0f * scale), 3.0f * scale, 2.0f * scale, eyeCol);
    drawList->AddEllipseFilled(ImVec2(pos.x + 9.0f * scale, pos.y - 27.0f * scale), 3.0f * scale, 2.0f * scale, eyeCol);
}

void ScreenManager::renderAnimatedEnemyModel(ImDrawList* drawList, const ImVec2& center, const CombatEnemy& enemy, bool isTarget, float animTime, float scale) {
    float bob = sinf(animTime * 2.8f + 1.2f) * 5.0f * scale;
    float breath = sinf(animTime * 2.0f) * 2.0f * scale;
    ImVec2 pos = ImVec2(center.x, center.y + bob);

    // 1. Тень монстра
    float rShadow = (enemy.rank == EnemyRank::Boss ? 42.0f : enemy.rank == EnemyRank::Elite ? 32.0f : 24.0f) * scale;
    drawList->AddEllipseFilled(ImVec2(center.x, center.y + 40.0f * scale), rShadow, 10.0f * scale, IM_COL32(0, 0, 0, 120));

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
    drawList->AddEllipseFilled(ImVec2(pos.x - 5.0f * scale, pos.y - 29.0f * scale), 3.0f * scale, 2.0f * scale, eyeCol);
    drawList->AddEllipseFilled(ImVec2(pos.x - 10.0f * scale, pos.y - 29.0f * scale), 3.0f * scale, 2.0f * scale, eyeCol);
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
    drawList->AddText(ImVec2(canvasPos.x + 30.0f, canvasPos.y + 8.0f), IM_COL32(100, 220, 140, 220), "⚔ ОТРЯД ГЕРОЕВ");
    drawList->AddText(ImVec2(canvasPos.x + width - 190.0f, canvasPos.y + 8.0f), IM_COL32(255, 90, 90, 220), "☠ ВРАЖЕСКИЙ ОТРЯД");

    // Отрисовка отряда героев СЛЕВА
    float heroStartX = canvasPos.x + 80.0f;
    float heroStepX = 110.0f;
    float heroBaseY = canvasPos.y + height * 0.50f;

    for (size_t i = 0; i < combatSquad.size(); ++i) {
        auto& hero = combatSquad[i];
        if (!hero.isAlive()) continue;
        bool isActive = (i == activeCharacterIndex);
        float hX = heroStartX + i * heroStepX + (isActive ? 25.0f : 0.0f);
        float hY = heroBaseY + (isActive ? 12.0f : -6.0f);
        renderAnimatedHeroModel(drawList, ImVec2(hX, hY), hero.id, isActive, animTime, isActive ? 1.25f : 1.0f);
    }

    // Отрисовка отряда врагов СПРАВА
    float enemyStartX = canvasPos.x + width - 90.0f;
    float enemyStepX = 120.0f;
    float enemyBaseY = canvasPos.y + height * 0.50f;

    for (size_t i = 0; i < combatEnemies.size(); ++i) {
        auto& enemy = combatEnemies[i];
        if (!enemy.isAlive()) continue;
        bool isTarget = (static_cast<int>(i) == selectedEnemyTarget);
        float eX = enemyStartX - i * enemyStepX - (isTarget ? 25.0f : 0.0f);
        float eY = enemyBaseY + (isTarget ? 12.0f : -6.0f);
        renderAnimatedEnemyModel(drawList, ImVec2(eX, eY), enemy, isTarget, animTime,
                                (enemy.rank == EnemyRank::Boss) ? 1.45f :
                                (enemy.rank == EnemyRank::Elite) ? 1.20f : 1.0f);
    }

    ImGui::EndChild();
    ImGui::PopStyleColor(2);
}
"""

marker_render_combat = "void ScreenManager::renderCombat() {"
assert marker_render_combat in content, "Could not find renderCombat"
content = content.replace(marker_render_combat, battlefield_methods + "\n" + marker_render_combat, 1)

# Now wire renderCombatBattlefield into renderCombat() right before ImGui::Columns(3, "CombatThreeColumns", false);
old_cols_start = """    ImGui::Separator();
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

    ImGui::Columns(3, "CombatThreeColumns", false);"""

if old_cols_start not in content:
    old_cols_start = old_cols_start.replace("\n", "\r\n")

new_cols_start = """    ImGui::Separator();
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

    ImGui::Columns(3, "CombatThreeColumns", false);"""

assert old_cols_start in content, "Could not find old_cols_start"
content = content.replace(old_cols_start, new_cols_start, 1)

with open(cpp_path, "w", encoding="utf-8") as f:
    f.write(content)

print("Battlefield models and arena successfully integrated!")
