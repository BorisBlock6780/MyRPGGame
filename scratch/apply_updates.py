import os

cpp_path = r"src\ScreenManager.cpp"
with open(cpp_path, "r", encoding="utf-8") as f:
    content = f.read()

# 1. Texture Loading for mining icon
old_tex = 'nodeIconTextures["crafting"].loadFromFile("assets/icons/artifact.png");'
new_tex = """nodeIconTextures["crafting"].loadFromFile("assets/icons/artifact.png");
    nodeIconTextures["mining"].loadFromFile("assets/icons/mining.png");"""

if old_tex in content and 'nodeIconTextures["mining"]' not in content:
    content = content.replace(old_tex, new_tex, 1)

# 2. Add ScreenState::Mining to renderScreen switch
old_switch = "    case ScreenState::Combat: renderCombat(); break;\n    case ScreenState::Crafting: renderCrafting(); break;\n    case ScreenState::Cooking: renderCooking(); break;"
if old_switch not in content:
    old_switch = old_switch.replace("\n", "\r\n")

new_switch = """    case ScreenState::Combat: renderCombat(); break;
    case ScreenState::Crafting: renderCrafting(); break;
    case ScreenState::Cooking: renderCooking(); break;
    case ScreenState::Mining: renderMining(); break;"""
if "\r\n" in old_switch:
    new_switch = new_switch.replace("\n", "\r\n")

if old_switch in content:
    content = content.replace(old_switch, new_switch, 1)

# 3. Add Mining handling in renderMapPopup
old_popup_switch = """    case MapNodeType::Crafting:
        currentScreen = ScreenState::Crafting;
        break;"""
if old_popup_switch not in content:
    old_popup_switch = old_popup_switch.replace("\n", "\r\n")

new_popup_switch = """    case MapNodeType::Crafting:
        currentScreen = ScreenState::Crafting;
        break;
    case MapNodeType::ResourceMine:
        currentScreen = ScreenState::Mining;
        currentMiningFloor = point.floor;
        miningAttemptsLeft = 5;
        miningFeedbackMsg = "Рудная жила обнаружена! Выберите момент и нанесите удар киркой.";
        miningFeedbackColor = ImVec4(0.4f, 0.9f, 1.0f, 1.0f);
        break;"""
if "\r\n" in old_popup_switch:
    new_popup_switch = new_popup_switch.replace("\n", "\r\n")

if old_popup_switch in content:
    content = content.replace(old_popup_switch, new_popup_switch, 1)

# 4. In renderMap(): add ResourceMine node color & icon
old_node_switch = """        case MapNodeType::Cooking:     fillColor = IM_COL32(155, 95, 35, 240); break;
        case MapNodeType::Crafting:    fillColor = IM_COL32(105, 70, 145, 240); break;"""
if old_node_switch not in content:
    old_node_switch = old_node_switch.replace("\n", "\r\n")

new_node_switch = """        case MapNodeType::Cooking:     fillColor = IM_COL32(155, 95, 35, 240); break;
        case MapNodeType::Crafting:    fillColor = IM_COL32(105, 70, 145, 240); break;
        case MapNodeType::ResourceMine: fillColor = IM_COL32(190, 130, 45, 245); break;"""
if "\r\n" in old_node_switch:
    new_node_switch = new_node_switch.replace("\n", "\r\n")

if old_node_switch in content:
    content = content.replace(old_node_switch, new_node_switch, 1)

with open(cpp_path, "w", encoding="utf-8") as f:
    f.write(content)

print("Part 1: Screen switch and map node types patched!")
