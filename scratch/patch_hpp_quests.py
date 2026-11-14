hpp_path = r"include\ScreenManager.hpp"
with open(hpp_path, "r", encoding="utf-8") as f:
    content = f.read()

old_block = """    struct QuestData {
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
    void updateQuestsProgress();"""

new_block = """    struct QuestData {
        std::string id;
        std::string title;
        std::string description;
        std::string category;
        bool isStory = false; // true = Сюжетный, false = Побочный
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
    int currentQuestFilterTab = 0; // 0 = Все, 1 = Сюжетные, 2 = Побочные"""

if "\r\n" in content:
    old_block = old_block.replace("\n", "\r\n")
    new_block = new_block.replace("\n", "\r\n")

assert old_block in content, "old_block not found in ScreenManager.hpp"
content = content.replace(old_block, new_block, 1)

with open(hpp_path, "w", encoding="utf-8") as f:
    f.write(content)

print("ScreenManager.hpp patched with isStory and currentQuestFilterTab!")
