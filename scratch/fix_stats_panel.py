cpp_path = r"src\ScreenManager.cpp"
with open(cpp_path, "r", encoding="utf-8") as f:
    text = f.read()

old_block = """    ImGui::TextColored(ImVec4(0.95f, 0.90f, 0.60f, 1.0f), "Герой: %s", currentChar->getName().c_str());
    ImGui::TextColored(ImVec4(0.40f, 0.85f, 1.0f, 1.0f), "Уровень: %d  |  Опыт: %d / %d",
        currentChar->getLevel(), currentChar->getExp(), currentChar->getExpForNextLevel());
    ImGui::Spacing();

    // Полосы здоровья и маны
    float hpRatio = static_cast<float>(currentChar->getCurrentHp() / std::max(1.0, totalStats[StatIndex::HP]));
    ImGui::TextColored(ImVec4(0.35f, 1.0f, 0.45f, 1.0f), "Здоровье (HP): %.0f / %.0f",
        currentChar->getCurrentHp(), totalStats[StatIndex::HP]);
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.25f, 0.85f, 0.40f, 1.0f));
    ImGui::ProgressBar(std::clamp(hpRatio, 0.0f, 1.0f), ImVec2(-1.0f, 14.0f), "");
    ImGui::PopStyleColor();

    float manaRatio = static_cast<float>(currentChar->getCurrentMana() / std::max(1.0, totalStats[StatIndex::Mana]));
    ImGui::TextColored(ImVec4(0.35f, 0.80f, 1.0f, 1.0f), "Мана: %.0f / %.0f",
        currentChar->getCurrentMana(), totalStats[StatIndex::Mana]);
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.20f, 0.55f, 0.95f, 1.0f));
    ImGui::ProgressBar(std::clamp(manaRatio, 0.0f, 1.0f), ImVec2(-1.0f, 14.0f), "");
    ImGui::PopStyleColor();"""

new_block = """    ImGui::TextColored(ImVec4(0.95f, 0.90f, 0.60f, 1.0f), "Герой: %s", currentChar->getName().c_str());
    ImGui::TextColored(ImVec4(0.40f, 0.85f, 1.0f, 1.0f), "Уровень героя: %d", currentChar->getLevel());
    ImGui::Spacing();

    // Полосы здоровья и маны
    ImGui::TextColored(ImVec4(0.35f, 1.0f, 0.45f, 1.0f), "Базовое здоровье (HP): %.0f", totalStats[StatIndex::HP]);
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.25f, 0.85f, 0.40f, 1.0f));
    ImGui::ProgressBar(1.0f, ImVec2(-1.0f, 14.0f), "");
    ImGui::PopStyleColor();

    ImGui::TextColored(ImVec4(0.35f, 0.80f, 1.0f, 1.0f), "Запас маны: %.0f", totalStats[StatIndex::Mana]);
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.20f, 0.55f, 0.95f, 1.0f));
    ImGui::ProgressBar(1.0f, ImVec2(-1.0f, 14.0f), "");
    ImGui::PopStyleColor();"""

if "\r\n" in text:
    old_block = old_block.replace("\n", "\r\n")
    new_block = new_block.replace("\n", "\r\n")

assert old_block in text, "old_block not found"
text = text.replace(old_block, new_block, 1)

with open(cpp_path, "w", encoding="utf-8") as f:
    f.write(text)

print("Stats panel calls fixed!")
