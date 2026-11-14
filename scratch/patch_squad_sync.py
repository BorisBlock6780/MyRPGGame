cpp_path = r"src\ScreenManager.cpp"
with open(cpp_path, "r", encoding="utf-8") as f:
    content = f.read()

old_sq = """    // Если список пуст, берем активный отряд игрока
    if (activeCombatPlayerSquad.empty()) {
        for (const auto& slot : playerProfile.getActiveSquad()) {
            if (slot) activeCombatPlayerSquad.push_back(slot->getId());
        }
    }
    if (activeCombatPlayerSquad.empty()) {
        activeCombatPlayerSquad = { "adeir", "renwick", "garosh" };
    }"""

new_sq = """    // Всегда берем текущий актуальный активный отряд игрока
    activeCombatPlayerSquad.clear();
    for (const auto& slot : playerProfile.getActiveSquad()) {
        if (slot) activeCombatPlayerSquad.push_back(slot->getId());
    }
    if (activeCombatPlayerSquad.empty()) {
        activeCombatPlayerSquad = { "adeir", "renwick", "garosh" };
    }"""

if old_sq not in content:
    old_sq = old_sq.replace("\n", "\r\n")
    new_sq = new_sq.replace("\n", "\r\n")

assert old_sq in content
content = content.replace(old_sq, new_sq, 1)

with open(cpp_path, "w", encoding="utf-8") as f:
    f.write(content)

print("Active combat squad sync updated!")
