import os

path = r"include\ScreenManager.hpp"
with open(path, "r", encoding="utf-8") as f:
    content = f.read()

# Add Mining to ScreenState
old_state = "    SquadEditor,\n    CharacterEditor,\n    Combat\n};"
if old_state not in content:
    old_state = old_state.replace("\n", "\r\n")

new_state = "    SquadEditor,\n    CharacterEditor,\n    Combat,\n    Mining\n};"
if "\r\n" in old_state:
    new_state = new_state.replace("\n", "\r\n")

assert old_state in content, "Could not find old_state in ScreenManager.hpp"
content = content.replace(old_state, new_state, 1)

# Add Mining & Animation fields to ScreenManager
old_decls = "    void renderCrafting();\n    void advanceDialogue(int nextIndex); // Переход к следующей фразе или закрытие\n    void renderDialogue();"
if old_decls not in content:
    old_decls = old_decls.replace("\n", "\r\n")

new_decls = """    void renderCrafting();
    void renderMining();
    void advanceDialogue(int nextIndex); // Переход к следующей фразе или закрытие
    void renderDialogue();

    // --- ИНТЕРАКТИВНАЯ ДОБЫЧА РУДЫ ---
    float miningTimer = 0.0f;
    int miningAttemptsLeft = 5;
    int miningTotalYield = 0;
    std::string miningFeedbackMsg;
    ImVec4 miningFeedbackColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    int currentMiningFloor = 1;

    // --- РЕДАКТОР ПЕРСОНАЖА (ЭКИПИРОВКА И ПОРТРЕТЫ) ---
    bool requestOpenSlotPicker = false;
    size_t selectedEditorCharIndex = 0;
    ArtifactType activeSlotPicker = ArtifactType::None;
    std::unordered_map<std::string, sf::Texture> characterPortraits;
    std::unordered_map<std::string, sf::Texture> slotIconTextures;
    void renderSlotPickerModal(std::shared_ptr<PlayerCharacter> character);

    // --- АНИМИРОВАННЫЕ МОДЕЛИ И ЭФФЕКТЫ АТАК В БОЮ ---
    struct CombatAnimState {
        bool isPlaying = false;
        std::string attackerId;
        bool isHero = true;
        size_t heroIndex = 0;
        size_t targetIndex = 0;
        size_t skillIndex = 0;
        float timer = 0.0f;
        float duration = 0.85f;
        double damage = 0.0;
        bool isCrit = false;
        bool damageApplied = false;
        std::string skillName;
    };
    CombatAnimState combatAnim;

    void renderAnimatedHeroModel(ImDrawList* drawList, const ImVec2& center, const std::string& charId, bool isActive, float animTime, float scale = 1.0f);
    void renderAnimatedEnemyModel(ImDrawList* drawList, const ImVec2& center, const CombatEnemy& enemy, bool isTarget, float animTime, float scale = 1.0f);
    void renderCombatBattlefield(float width, float height);
    void renderCombatVisualEffects(ImDrawList* drawList, const ImVec2& arenaPos, float arenaW, float arenaH);"""

if "\r\n" in old_decls:
    new_decls = new_decls.replace("\n", "\r\n")

# Remove previous declarations block if present
prev_marker = "    // --- РЕДАКТОР ПЕРСОНАЖА (ЭКИПИРОВКА И ПОРТРЕТЫ) ---"
if prev_marker in content:
    idx_p = content.find(prev_marker)
    idx_end = content.find("};", idx_p)
    content = content[:idx_p] + content[idx_end:]

content = content.replace(old_decls, new_decls, 1)

with open(path, "w", encoding="utf-8") as f:
    f.write(content)

print("ScreenManager.hpp successfully patched with Mining and Combat Animations!")
