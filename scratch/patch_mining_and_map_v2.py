import os
import re

cpp_path = r"src\ScreenManager.cpp"
with open(cpp_path, "r", encoding="utf-8") as f:
    content = f.read()

# 1. Update craftRecipes in renderCrafting()
old_craft_recipes = """    static std::vector<CraftRecipe> craftRecipes = {
        {"old_cleaver", {"rusty_blade", "iron_ore", "iron_ore"}},
        {"berserker_axe", {"old_cleaver", "iron_ore", "iron_ore"}}
    };"""

new_craft_recipes = """    static std::vector<CraftRecipe> craftRecipes = {
        {"old_cleaver", {"rusty_blade", "iron_ore", "iron_ore"}},
        {"berserker_axe", {"old_cleaver", "iron_ore", "iron_ore"}},
        {"bastion_blade", {"iron_ore", "mithril_ore", "scrap_metal"}},
        {"vampire_blade", {"old_cleaver", "fire_crystal", "mithril_ore"}},
        {"shadow_dagger", {"scrap_metal", "void_shard", "mithril_ore"}},
        {"hunter_bow", {"scrap_metal", "scrap_metal", "mithril_ore"}},
        {"apprentice_staff", {"fire_crystal", "void_shard", "celestial_ingot"}}
    };"""

if "\r\n" in content:
    old_craft_recipes = old_craft_recipes.replace("\n", "\r\n")
    new_craft_recipes = new_craft_recipes.replace("\n", "\r\n")

if old_craft_recipes in content:
    content = content.replace(old_craft_recipes, new_craft_recipes, 1)
    print("Updated craftRecipes!")
else:
    print("craftRecipes already updated or not found")

# 2. Update renderMapPopup() for ResourceMine
if "[ Добыча полезных ископаемых ]" not in content:
    old_popup_desc = """    else if (point.type == MapNodeType::Combat) {"""
    new_popup_desc = """    else if (point.type == MapNodeType::ResourceMine) {
        ImGui::TextColored(ImVec4(0.40f, 0.90f, 1.0f, 1.0f), "[ Добыча полезных ископаемых ]");
        ImGui::Spacing();
        ImGui::TextColored(ImVec4(0.95f, 0.85f, 0.35f, 1.0f), "Богатая жила минералов и ценных самородков.");
        ImGui::TextDisabled("Добывайте руду для ковки редкого оружия и снаряжения в кузнице!");
    }
    else if (point.type == MapNodeType::Combat) {"""

    if "\r\n" in content:
        old_popup_desc = old_popup_desc.replace("\n", "\r\n")
        new_popup_desc = new_popup_desc.replace("\n", "\r\n")

    assert old_popup_desc in content, "old_popup_desc not found"
    content = content.replace(old_popup_desc, new_popup_desc, 1)

    old_popup_switch = """        case MapNodeType::Crafting:
            currentScreen = ScreenState::Crafting;
            break;
        }"""

    new_popup_switch = """        case MapNodeType::Crafting:
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
        }"""

    if "\r\n" in content:
        old_popup_switch = old_popup_switch.replace("\n", "\r\n")
        new_popup_switch = new_popup_switch.replace("\n", "\r\n")

    assert old_popup_switch in content, "old_popup_switch not found"
    content = content.replace(old_popup_switch, new_popup_switch, 1)
    print("Updated renderMapPopup!")

# 3. Add addMining lambda to initMapPoints()
if "auto addMining =" not in content:
    old_init_utility = """    auto addUtility = [&](const std::string& id, const std::string& title, const std::string& desc,
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
    };"""

    new_init_utility = """    auto addUtility = [&](const std::string& id, const std::string& title, const std::string& desc,
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
    };"""

    if "\r\n" in content:
        old_init_utility = old_init_utility.replace("\n", "\r\n")
        new_init_utility = new_init_utility.replace("\n", "\r\n")

    assert old_init_utility in content, "old_init_utility not found"
    content = content.replace(old_init_utility, new_init_utility, 1)
    print("Added addMining lambda!")

# 4. Insert new combat and mining points for each floor right after the boss camp of that floor
extra_points = {
    1: """
    addMining("f1_mining_iron", "Железная жила", "Богатая жила железной руды и обломков металла в катакомбах.",
              1, ImVec2(500.0f, 170.0f));
    addCombat("f1_combat_patrol", "Дозор нежити", "Отряд бродячих скелетов патрулирует склепы.",
              1, ImVec2(170.0f, 260.0f), {{"f1_skeleton_scout", "Скелет-разведчик", 2, 3}});
    addCombat("f1_combat_ghouls", "Логово упырей", "Несколько ненасытных упырей рыщут в поисках свежей плоти.",
              1, ImVec2(720.0f, 510.0f), {{"f1_decayed_zombie", "Разложившийся зомби", 2, 2}, {"f1_bone_hound", "Костяная гончая", 2, 2}});""",
    2: """
    addMining("f2_mining_vein", "Заросшая жила", "Кварцевая жила с прожилками мифрила среди корней древних деревьев.",
              2, ImVec2(240.0f, 210.0f));
    addCombat("f2_combat_spiders", "Паучья засада", "Гигантские ядовитые пауки спустились с кроны.",
              2, ImVec2(560.0f, 490.0f), {{"f2_thorn_stalker", "Шипастый ловец", 5, 3}});
    addCombat("f2_combat_vipers", "Змеиный выводок", "Гнездо болотных гадюк у водопоя.",
              2, ImVec2(730.0f, 320.0f), {{"f2_oasis_viper", "Болотная гадюка", 5, 3}, {"f2_lake_lurker", "Озерный выползень", 5, 2}});""",
    3: """
    addMining("f3_mining_crystals", "Подводная жила", "Океанические кристаллы и минералы на шельфе.",
              3, ImVec2(480.0f, 210.0f));
    addCombat("f3_combat_raiders", "Налетчики глубин", "Группа тритонов с трезубцами перекрыла путь.",
              3, ImVec2(180.0f, 270.0f), {{"f3_triton_spearman", "Тритон-копейщик", 9, 3}});
    addCombat("f3_combat_chasm", "Твари расщелины", "Глубоководные угри выплыли на свет факелов.",
              3, ImVec2(720.0f, 530.0f), {{"f3_chasm_eel", "Угорь бездны", 10, 2}, {"f3_abyssal_jellyfish", "Глубинная медуза", 9, 2}});""",
    4: """
    addMining("f4_mining_magma", "Магматическая жила", "Раскаленная жила с огненными кристаллами и рудой.",
              4, ImVec2(490.0f, 190.0f));
    addCombat("f4_combat_flame", "Огненная стража", "Духи пламени кружат вокруг жерла.",
              4, ImVec2(210.0f, 260.0f), {{"f4_magma_imp", "Магматический бес", 13, 3}, {"f4_flame_hound", "Пламенная гончая", 14, 2}});
    addCombat("f4_combat_duergar", "Аванпост дуэргаров", "Патруль темных кузнецов охраняет подходы к плавильням.",
              4, ImVec2(710.0f, 490.0f), {{"f4_duergar_berserker", "Дуэргар-берсерк", 14, 3}});""",
    5: """
    addMining("f5_mining_scrap", "Свалка сплавов", "Залежи редких металлических сплавов и мифрила среди обломков механизмов.",
              5, ImVec2(470.0f, 200.0f));
    addCombat("f5_combat_drones", "Рой дронов", "Автономные хирургические дроны перешли в режим охоты.",
              5, ImVec2(200.0f, 260.0f), {{"f5_surgical_drone", "Хирургический дрон", 17, 3}, {"f5_toxic_crawler", "Токсичный ползун", 17, 2}});
    addCombat("f5_combat_hounds", "Кибер-гончие", "Усиленные псы рыщут по коридорам комплекса.",
              5, ImVec2(710.0f, 520.0f), {{"f5_cyber_hound", "Кибер-гончая", 18, 3}});""",
    6: """
    addMining("f6_mining_jungle", "Алмазная жила", "Ископаемые кристаллы среди корней древних исполинов.",
              6, ImVec2(510.0f, 200.0f));
    addCombat("f6_combat_raptors", "Стая хищников", "Быстрые джунглевые рапторы окружили тропу.",
              6, ImVec2(190.0f, 270.0f), {{"f6_jungle_raptor", "Джунглевый раптор", 22, 3}});
    addCombat("f6_combat_hornets", "Гнездо шершней", "Рой ядовитых гигантских шершней в ветвях.",
              6, ImVec2(720.0f, 500.0f), {{"f6_toxic_hornet", "Токсичный шершень", 21, 4}});""",
    7: """
    addMining("f7_mining_ice", "Ледяная жила", "Мифриловые самородки, вмороженные в вековой лед.",
              7, ImVec2(490.0f, 210.0f));
    addCombat("f7_combat_wolves", "Стая метели", "Свирепые снежные волки преграждают перевал.",
              7, ImVec2(190.0f, 280.0f), {{"f7_blizzard_wolf", "Снежный волк", 26, 3}});
    addCombat("f7_combat_golems", "Кристальные големы", "Стражи ледника восстают из толщи снега.",
              7, ImVec2(710.0f, 510.0f), {{"f7_crystal_golem", "Кристальный голем", 27, 2}, {"f7_frozen_remnant", "Ледяной остаток", 26, 2}});""",
    8: """
    addMining("f8_mining_shadow", "Теневая жила", "Жила с осколками пустоты и редкими металлами в переулках мегаполиса.",
              8, ImVec2(490.0f, 220.0f));
    addCombat("f8_combat_androids", "Патруль синдиката", "Тяжеловооруженные андроиды зачищают сектор.",
              8, ImVec2(190.0f, 270.0f), {{"f8_enforcer_android", "Андроид-каратель", 31, 3}});
    addCombat("f8_combat_glitch", "Глитч-фантомы", "Искаженные цифровые фантомы мерцают в неоновом тумане.",
              8, ImVec2(720.0f, 510.0f), {{"f8_holo_phantom", "Голо-фантом", 31, 2}, {"f8_glitch_hound", "Глитч-гончая", 31, 2}});""",
    9: """
    addMining("f9_mining_void", "Жила разлома", "Чистейшие осколки пустоты и небесные металлы у Престола Света.",
              9, ImVec2(500.0f, 210.0f));
    addCombat("f9_combat_seraphs", "Карающий авангард", "Серафимы-часовые преграждают путь грешникам.",
              9, ImVec2(200.0f, 270.0f), {{"f9_seraph_sentinel", "Серафим-часовой", 36, 3}});
    addCombat("f9_combat_zealots", "Круг фанатиков", "Ревностные последователи света отражают вторжение.",
              9, ImVec2(720.0f, 500.0f), {{"f9_radiant_zealot", "Лучезарный фанатик", 36, 3}});""",
    10: """
    addMining("f10_mining_celestial", "Небесная жила", "Небесные слитки и самородки божественной руды в ядре мироздания.",
              10, ImVec2(490.0f, 210.0f));
    addCombat("f10_combat_wisps", "Сгустки сингулярности", "Странствующие искры первородной сингулярности искажают пространство.",
              10, ImVec2(200.0f, 270.0f), {{"f10_singularity_wisp", "Искра сингулярности", 42, 3}});
    addCombat("f10_combat_knights", "Рыцари забвения", "Элитная стража бездны не пропускает никого к истоку.",
              10, ImVec2(710.0f, 510.0f), {{"f10_oblivion_knight", "Рыцарь забвения", 43, 2}, {"f10_void_walker", "Странник пустоты", 42, 2}});"""
}

# Regex find for each floor: addCombat\("f{floor}_boss_camp".*?\);
for fl, extra_code in extra_points.items():
    if f'"f{fl}_mining_' in content:
        print(f"Floor {fl} already has mining node, skipping")
        continue

    pat = rf'(addCombat\(\s*"f{fl}_boss_camp"[^;]+;\s*\n)'
    m = re.search(pat, content)
    assert m is not None, f"Could not find boss camp for floor {fl}"
    matched_block = m.group(1)

    replacement_extra = extra_code
    if "\r\n" in matched_block:
        replacement_extra = replacement_extra.replace("\n", "\r\n")

    new_block = matched_block + replacement_extra
    content = content.replace(matched_block, new_block, 1)
    print(f"Floor {fl} nodes added!")

# 5. Append renderMining implementation if not already present
if "void ScreenManager::renderMining()" not in content:
    mining_impl = """

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

            if (ImGui::Button("⛏  НАНЕСТИ ТОЧНЫЙ УДАР КИРКОЙ!##Strike", ImVec2(btnW, btnH))) {
                miningAttemptsLeft--;
                float dist = std::fabs(strikePos - 0.5f);

                if (dist <= 0.08f) {
                    // Критический точный удар
                    if (currentMiningFloor >= 8) {
                        playerInventory.addItem("celestial_ingot", 1);
                        playerInventory.addItem("void_shard", 2);
                        playerInventory.addItem("mithril_ore", 3);
                        miningTotalYield += 6;
                        miningFeedbackMsg = "★ КРИТИЧЕСКИЙ УДАР! Жила раскололась: получен [Небесный слиток x1], [Осколок пустоты x2], [Мифриловая руда x3]!";
                    } else if (currentMiningFloor >= 4) {
                        playerInventory.addItem("fire_crystal", 2);
                        playerInventory.addItem("mithril_ore", 2);
                        playerInventory.addItem("iron_ore", 4);
                        miningTotalYield += 8;
                        miningFeedbackMsg = "★ КРИТИЧЕСКИЙ УДАР! Жила озарилась жаром: получен [Огненный кристалл x2], [Мифриловая руда x2], [Железная руда x4]!";
                    } else {
                        playerInventory.addItem("mithril_ore", 2);
                        playerInventory.addItem("iron_ore", 4);
                        playerInventory.addItem("scrap_metal", 3);
                        miningTotalYield += 9;
                        miningFeedbackMsg = "★ КРИТИЧЕСКИЙ УДАР! Высечена богатая порода: получен [Мифриловая руда x2], [Железная руда x4], [Обломки металла x3]!";
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
"""
    if "\r\n" in content:
        mining_impl = mining_impl.replace("\n", "\r\n")
    content += mining_impl
    print("Appended renderMining!")

with open(cpp_path, "w", encoding="utf-8") as f:
    f.write(content)

print("All updates successfully applied!")
