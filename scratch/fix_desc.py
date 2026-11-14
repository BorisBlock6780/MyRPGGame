with open('src/ScreenManager.cpp', 'r', encoding='utf-8') as f:
    text = f.read()

target = "if (itItem != db.items.end() && !itItem->second->getDescription().empty()) {\n                ImGui::TextDisabled(\"%s\", itItem->second->getDescription().c_str());\n            }"
replacement = """auto itW = db.weapons.find(q.rewardItemId);
            if (itW != db.weapons.end()) {
                ImGui::TextDisabled("Оружие: %s", itW->second->getEffectDesc().c_str());
            } else {
                auto itA = db.artifacts.find(q.rewardItemId);
                if (itA != db.artifacts.end()) {
                    ImGui::TextDisabled("Артефакт: %s", itA->second->getEffectDesc().c_str());
                } else if (itItem != db.items.end()) {
                    ImGui::TextDisabled("Цена: %d монет", itItem->second->getSellPrice());
                }
            }"""

if target in text:
    text = text.replace(target, replacement)
    print("Fixed with LF")
elif target.replace('\n', '\r\n') in text:
    text = text.replace(target.replace('\n', '\r\n'), replacement.replace('\n', '\r\n'))
    print("Fixed with CRLF")
else:
    print("Target not found directly, checking substring...")
    pos = text.find("getDescription()")
    print(f"getDescription at {pos}")
    if pos != -1:
        start = text.rfind("if (itItem != db.items.end()", 0, pos)
        end = text.find("}", pos) + 1
        text = text[:start] + replacement.replace('\n', '\r\n') + text[end:]
        print("Fixed by pos!")

with open('src/ScreenManager.cpp', 'w', encoding='utf-8') as f:
    f.write(text)
