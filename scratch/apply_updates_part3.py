import os

cpp_path = r"src\ScreenManager.cpp"
with open(cpp_path, "r", encoding="utf-8") as f:
    content = f.read()

# Replace renderAnimatedHeroModel, renderAnimatedEnemyModel, renderCombatBattlefield
start_mark = "void ScreenManager::renderAnimatedHeroModel(ImDrawList* drawList, const ImVec2& center, const std::string& charId, bool isActive, float animTime, float scale) {"
end_mark = "void ScreenManager::renderCombat() {"

assert start_mark in content, "Could not find start_mark for models"
assert end_mark in content, "Could not find end_mark for renderCombat"

models_and_combat_code = """void ScreenManager::renderAnimatedHeroModel(ImDrawList* drawList, const ImVec2& center, const std::string& charId, bool isActive, float animTime, float scale) {
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
    drawList->AddEllipseFilled(ImVec2(pos.x + 4.0f * scale, pos.y - 27.0f * scale), 3.0f * scale, 2.0f * scale, eyeCol);
    drawList->AddEllipseFilled(ImVec2(pos.x + 9.0f * scale, pos.y - 27.0f * scale), 3.0f * scale, 2.0f * scale, eyeCol);

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
        drawList->AddEllipse(ImVec2(pos.x, pos.y - 42.0f * scale), 12.0f * scale, 4.0f * scale, IM_COL32(255, 235, 100, 255), 16, 2.0f);
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
        drawList->AddArc(ImVec2(pos.x + 22.0f * scale, pos.y + 8.0f * scale), 10.0f * scale, 3.14f, 6.28f, IM_COL32(115, 240, 190, 255), 12, 3.0f * scale);
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
    drawList->AddEllipseFilled(ImVec2(pos.x - 5.0f * scale, pos.y - 29.0f * scale), 3.0f * scale, 2.0f * scale, eyeCol);
    drawList->AddEllipseFilled(ImVec2(pos.x - 10.0f * scale, pos.y - 29.0f * scale), 3.0f * scale, 2.0f * scale, eyeCol);
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
        std::string dmgText = (combatAnim.isCrit ? "★ КРИТ! -" : "-") + std::to_string(static_cast<int>(combatAnim.damage)) + " HP";
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
    drawList->AddText(ImVec2(canvasPos.x + 30.0f, canvasPos.y + 8.0f), IM_COL32(100, 220, 140, 220), "⚔ ОТРЯД ГЕРОЕВ");
    drawList->AddText(ImVec2(canvasPos.x + width - 190.0f, canvasPos.y + 8.0f), IM_COL32(255, 90, 90, 220), "☠ ВРАЖЕСКИЙ ОТРЯД");

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
"""

content = content[:content.find(start_mark)] + models_and_combat_code + "\n" + content[content.find(end_mark):]

# Now update processPlayerSkill to start combatAnim
old_skill_proc = """    if (isShield) {
        hero.shield += rawValue;
        logCombat(hero.name + " активирует '" + skillName + "' и накладывает щит: +" + std::to_string(static_cast<int>(rawValue)) + "!", ImVec4(0.3f, 0.8f, 1.0f, 1.0f));
    } else if (isHeal) {
        double healed = std::min(hero.maxHp - hero.currentHp, rawValue);
        hero.currentHp = std::min(hero.maxHp, hero.currentHp + rawValue);
        logCombat(hero.name + " применяет '" + skillName + "' и восстанавливает +" + std::to_string(static_cast<int>(healed)) + " HP!", ImVec4(0.3f, 1.0f, 0.4f, 1.0f));
    } else {
        auto& target = combatEnemies[selectedEnemyTarget];

        bool isCrit = ((rand() % 1000) / 1000.0) < hero.stats[StatIndex::CritChance];
        double critMult = isCrit ? hero.stats[StatIndex::CritDamage] : 1.0;
        double dmgBonus = 1.0 + hero.stats[StatIndex::DamageBonus];
        double totalDmg = rawValue * critMult * dmgBonus;

        double netDmg = std::max(1.0, totalDmg - (target.defense * 0.4));
        if (target.shield > 0.0) {
            double absorbed = std::min(target.shield, netDmg);
            target.shield -= absorbed;
            netDmg -= absorbed;
        }
        target.currentHp = std::max(0.0, target.currentHp - netDmg);

        if (hero.stats[StatIndex::Vampirism] > 0.0 && netDmg > 0.0) {
            double vampHeal = netDmg * hero.stats[StatIndex::Vampirism];
            hero.currentHp = std::min(hero.maxHp, hero.currentHp + vampHeal);
        }

        std::string critPrefix = isCrit ? "[КРИТ!] " : "";
        ImVec4 logColor = isCrit ? ImVec4(1.0f, 0.45f, 0.1f, 1.0f) : ImVec4(0.95f, 0.95f, 0.35f, 1.0f);
        logCombat(critPrefix + hero.name + " наносит '" + skillName + "' по " + target.name + " (-" + std::to_string(static_cast<int>(netDmg)) + " HP)!", logColor);

        if (!target.isAlive()) {
            logCombat("[ПОБЕЖДЕН] " + target.name + " повержен!", ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
        }
    }

    if (skill->getConsumesAllStacks()) {
        hero.skillStacks = 0;
    } else if (skill->getStacksGranted() > 0) {
        hero.skillStacks += skill->getStacksGranted();
    }

    hero.skillCooldowns[skillIndex] = skill->getCooldown();

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

    // Если цель погибла, переключаем на следующего живого
    if (selectedEnemyTarget >= 0 && !combatEnemies[selectedEnemyTarget].isAlive()) {
        for (size_t i = 0; i < combatEnemies.size(); ++i) {
            if (combatEnemies[i].isAlive()) {
                selectedEnemyTarget = static_cast<int>(i);
                break;
            }
        }
    }

    executeEnemyTurn();"""

if old_skill_proc not in content:
    old_skill_proc = old_skill_proc.replace("\n", "\r\n")

new_skill_proc = """    // Управление стаками и КД
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
    }"""

if "\r\n" in old_skill_proc:
    new_skill_proc = new_skill_proc.replace("\n", "\r\n")

assert old_skill_proc in content, "Could not find old_skill_proc"
content = content.replace(old_skill_proc, new_skill_proc, 1)

with open(cpp_path, "w", encoding="utf-8") as f:
    f.write(content)

print("Part 3: 20 Unique Hero Models & Attack Animations updated!")
