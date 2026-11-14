import os

cpp_path = r"src\ScreenManager.cpp"
with open(cpp_path, "r", encoding="utf-8") as f:
    content = f.read()

# 1. Texture Loading in ScreenManager::ScreenManager()
old_tex_load = """    nodeIconTextures["shop"].loadFromFile("assets/icons/jewelry.png");
    nodeIconTextures["combat"].loadFromFile("assets/icons/weapon.png");
    nodeIconTextures["resource"].loadFromFile("assets/icons/materials.png");
    nodeIconTextures["cooking"].loadFromFile("assets/icons/food.png");
    nodeIconTextures["crafting"].loadFromFile("assets/icons/artifact.png");
    return true;"""

if old_tex_load not in content:
    old_tex_load = old_tex_load.replace("\n", "\r\n")

new_tex_load = """    nodeIconTextures["shop"].loadFromFile("assets/icons/jewelry.png");
    nodeIconTextures["combat"].loadFromFile("assets/icons/combat.png");
    nodeIconTextures["combat_elite"].loadFromFile("assets/icons/combat_elite.png");
    nodeIconTextures["combat_boss"].loadFromFile("assets/icons/combat_boss.png");
    nodeIconTextures["resource"].loadFromFile("assets/icons/materials.png");
    nodeIconTextures["cooking"].loadFromFile("assets/icons/food.png");
    nodeIconTextures["crafting"].loadFromFile("assets/icons/artifact.png");

    slotIconTextures["slot_weapon"].loadFromFile("assets/icons/slot_weapon.png");
    slotIconTextures["slot_headgear"].loadFromFile("assets/icons/slot_headgear.png");
    slotIconTextures["slot_costume"].loadFromFile("assets/icons/slot_costume.png");
    slotIconTextures["slot_amulet"].loadFromFile("assets/icons/slot_amulet.png");
    slotIconTextures["slot_ring"].loadFromFile("assets/icons/slot_ring.png");

    for (const auto& [charId, _] : GameDatabase::getInstance().characters) {
        characterPortraits[charId].loadFromFile("assets/portraits/" + charId + ".png");
        characterPortraits[charId].setSmooth(true);
    }

    return true;"""

assert old_tex_load in content, "Could not find old_tex_load"
content = content.replace(old_tex_load, new_tex_load, 1)

# 2. In initMapPoints(): set distinctive iconKey for elite and boss combat
old_add_combat = """        pt.type = MapNodeType::Combat;
        pt.iconKey = "combat";
        pt.isTemporary = isTemp;"""

if old_add_combat not in content:
    old_add_combat = old_add_combat.replace("\n", "\r\n")

new_add_combat = """        pt.type = MapNodeType::Combat;
        if (id.find("boss") != std::string::npos) {
            pt.iconKey = "combat_boss";
        } else if (id.find("elite") != std::string::npos) {
            pt.iconKey = "combat_elite";
        } else {
            pt.iconKey = "combat";
        }
        pt.isTemporary = isTemp;"""

assert old_add_combat in content, "Could not find old_add_combat"
content = content.replace(old_add_combat, new_add_combat, 1)

# 3. In renderMap(): remove connecting lines and add elite/boss visual distinction
old_map_lines = """    // Отрисовка декоративных связующих дорожек между точками
    if (floorNodeCenters.size() >= 2) {
        for (size_t a = 0; a < floorNodeCenters.size() - 1; ++a) {
            drawList->AddLine(floorNodeCenters[a], floorNodeCenters[a + 1],
                              IM_COL32(210, 175, 80, 110), 3.0f);
        }
    }"""

if old_map_lines not in content:
    old_map_lines = old_map_lines.replace("\n", "\r\n")

new_map_lines = """    // Линии связующих дорожек удалены по требованию пользователя"""

assert old_map_lines in content, "Could not find old_map_lines"
content = content.replace(old_map_lines, new_map_lines, 1)

# 4. In renderMap(): update combat circle colors & pulsating glows for elite & boss
old_map_point_render = """        switch (point.type) {
        case MapNodeType::Shop:        fillColor = IM_COL32(45, 110, 75, 240); break;
        case MapNodeType::Combat:      fillColor = IM_COL32(150, 45, 45, 240); break;
        case MapNodeType::Cooking:     fillColor = IM_COL32(155, 95, 35, 240); break;
        case MapNodeType::Crafting:    fillColor = IM_COL32(105, 70, 145, 240); break;
        }

        // Внешнее свечение при наведении
        if (isHovered) {
            drawList->AddCircleFilled(center, r + 6.0f, IM_COL32(255, 220, 90, 70), 32);
        }

        drawList->AddCircleFilled(center, r, fillColor, 32);
        drawList->AddCircle(center, r, borderColor, 32, isHovered ? 4.0f : 2.0f);"""

if old_map_point_render not in content:
    old_map_point_render = old_map_point_render.replace("\n", "\r\n")

new_map_point_render = """        switch (point.type) {
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
        drawList->AddCircle(center, r, borderColor, 32, (isHovered || point.iconKey == "combat_boss") ? 3.5f : 2.0f);"""

assert old_map_point_render in content, "Could not find old_map_point_render"
content = content.replace(old_map_point_render, new_map_point_render, 1)

with open(cpp_path, "w", encoding="utf-8") as f:
    f.write(content)

print("ScreenManager.cpp (part 1) successfully patched!")
