import os

path = r"include\ScreenManager.hpp"
with open(path, "r", encoding="utf-8") as f:
    content = f.read()

marker = "    void renderDialogue();"
new_declarations = """    void renderDialogue();

    // --- РЕДАКТОР ПЕРСОНАЖА (ЭКИПИРОВКА И ПОРТРЕТЫ) ---
    size_t selectedEditorCharIndex = 0;
    ArtifactType activeSlotPicker = ArtifactType::None;
    std::unordered_map<std::string, sf::Texture> characterPortraits;
    std::unordered_map<std::string, sf::Texture> slotIconTextures;
    void renderSlotPickerModal(std::shared_ptr<PlayerCharacter> character);

    // --- АНИМИРОВАННЫЕ МОДЕЛИ В БОЮ ---
    void renderAnimatedHeroModel(ImDrawList* drawList, const ImVec2& center, const std::string& charId, bool isActive, float animTime, float scale = 1.0f);
    void renderAnimatedEnemyModel(ImDrawList* drawList, const ImVec2& center, const CombatEnemy& enemy, bool isTarget, float animTime, float scale = 1.0f);
    void renderCombatBattlefield(float width, float height);"""

if "selectedEditorCharIndex" not in content:
    assert marker in content
    content = content.replace(marker, new_declarations, 1)

with open(path, "w", encoding="utf-8") as f:
    f.write(content)

print("ScreenManager.hpp successfully patched!")
