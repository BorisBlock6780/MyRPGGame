hpp_path = r"include\ScreenManager.hpp"
with open(hpp_path, "r", encoding="utf-8") as f:
    content = f.read()

# 1. Update ScreenState enum
old_enum = """    Combat,
    Mining
};"""

new_enum = """    Combat,
    Mining,
    Quests
};"""

if "\r\n" in content:
    old_enum = old_enum.replace("\n", "\r\n")
    new_enum = new_enum.replace("\n", "\r\n")

assert old_enum in content, "old_enum not found"
content = content.replace(old_enum, new_enum, 1)

# 2. Add methods & structs
old_methods = """    void renderCrafting();
    void renderMining();
    void advanceDialogue(int nextIndex);"""

new_methods = """    void renderCrafting();
    void renderMining();
    void renderQuests();
    void advanceDialogue(int nextIndex);"""

if "\r\n" in content:
    old_methods = old_methods.replace("\n", "\r\n")
    new_methods = new_methods.replace("\n", "\r\n")

assert old_methods in content, "old_methods not found"
content = content.replace(old_methods, new_methods, 1)

old_dialogue = """    void renderDialogue();

    // ---"""

new_dialogue = """    void renderDialogue();

    // --- СИСТЕМА ДОСТИЖЕНИЙ (ГЛОБАЛЬНЫЕ И ПРОФИЛЬНЫЕ) ---
    struct GameAchievement {
        std::string id;
        std::string title;
        std::string description;
        std::string icon;
        bool isGlobal = false;
        bool isUnlocked = false;
        std::string unlockedDate = "";
    };
    std::vector<GameAchievement> achievements;
    void initAchievements();
    void loadGlobalAchievements();
    void saveGlobalAchievements();
    bool unlockAchievement(const std::string& idOrTitle);
    bool lockAchievement(const std::string& idOrTitle);
    int currentAchievementTab = 0;

    // --- СИСТЕМА КВЕСТОВ И ЗАДАНИЙ ---
    enum class QuestStatus {
        InProgress,
        Completed,
        Claimed
    };
    struct QuestData {
        std::string id;
        std::string title;
        std::string description;
        std::string category;
        int currentProgress = 0;
        int targetProgress = 1;
        std::string progressLabel;
        int rewardGold = 0;
        int rewardExp = 0;
        std::string rewardItemId = "";
        int rewardItemCount = 0;
        QuestStatus status = QuestStatus::InProgress;
    };
    std::vector<QuestData> quests;
    void initQuests();
    void updateQuestsProgress();

    // ---"""

if "\r\n" in content:
    old_dialogue = old_dialogue.replace("\n", "\r\n")
    new_dialogue = new_dialogue.replace("\n", "\r\n")

assert old_dialogue in content, "old_dialogue not found"
content = content.replace(old_dialogue, new_dialogue, 1)

with open(hpp_path, "w", encoding="utf-8") as f:
    f.write(content)

print("ScreenManager.hpp successfully patched!")
