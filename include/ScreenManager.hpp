#ifndef SCREEN_MANAGER_HPP
#define SCREEN_MANAGER_HPP

#include <imgui.h>
#include <SFML/Graphics.hpp>
#include <string>
#include <unordered_map>
#include <imgui-SFML.h>
#include "PlayerData.hpp"
#include "MapNodes.hpp"
#include "DialogueSystem.hpp"


enum class ScreenState {
    MainMenu,
    Settings,
    Saves,
    Lobby,
    Shop,
    Inventory,
    Cooking,
    Crafting,
    Map,
    Achievements,
    SquadEditor,
    CharacterEditor,
    Combat
};

enum class WindowMode {
    Fullscreen = 0,
    BorderlessWindowed = 1
};

enum class GameLanguage {
    Russian,
    English
};

enum class ItemTab {
    Weapons,
    Artifacts,
    Food,
    Materials,
    Jewelry,
    QuestItems
};

enum class CombatAction {
    SelectHero1 = 0,
    SelectHero2 = 1,
    SelectHero3 = 2,
    Ultimate    = 3, // Q
    Skill       = 4, // W
    BasicAttack = 5  // E
};

struct CombatKeybinds {
    sf::Keyboard::Key selectHero1 = sf::Keyboard::Num1;
    sf::Keyboard::Key selectHero2 = sf::Keyboard::Num2;
    sf::Keyboard::Key selectHero3 = sf::Keyboard::Num3;
    sf::Keyboard::Key ultimate    = sf::Keyboard::Q;
    sf::Keyboard::Key skill       = sf::Keyboard::W;
    sf::Keyboard::Key basicAttack = sf::Keyboard::E;

    void resetToDefaults() {
        selectHero1 = sf::Keyboard::Num1;
        selectHero2 = sf::Keyboard::Num2;
        selectHero3 = sf::Keyboard::Num3;
        ultimate    = sf::Keyboard::Q;
        skill       = sf::Keyboard::W;
        basicAttack = sf::Keyboard::E;
    }

    sf::Keyboard::Key getKey(CombatAction action) const {
        switch (action) {
        case CombatAction::SelectHero1: return selectHero1;
        case CombatAction::SelectHero2: return selectHero2;
        case CombatAction::SelectHero3: return selectHero3;
        case CombatAction::Ultimate:    return ultimate;
        case CombatAction::Skill:       return skill;
        case CombatAction::BasicAttack: return basicAttack;
        default: return sf::Keyboard::Unknown;
        }
    }

    void setKey(CombatAction action, sf::Keyboard::Key key) {
        switch (action) {
        case CombatAction::SelectHero1: selectHero1 = key; break;
        case CombatAction::SelectHero2: selectHero2 = key; break;
        case CombatAction::SelectHero3: selectHero3 = key; break;
        case CombatAction::Ultimate:    ultimate = key; break;
        case CombatAction::Skill:       skill = key; break;
        case CombatAction::BasicAttack: basicAttack = key; break;
        default: break;
        }
    }
};

class ScreenManager {
public:
    ScreenManager();
    ScreenState currentScreen = ScreenState::MainMenu;
    bool shouldQuit = false;

    void openShop(const std::string& merchantName, PlayerInventory* merchantInventory) {
        currentMerchantName = merchantName;
        activeMerchantInventory = merchantInventory;
        currentShopTab = ItemTab::Weapons;
        currentScreen = ScreenState::Shop;
    }
    int currentFloor = 1; // Текущий выбранный этаж (1..10)
    std::vector<MapPointOfInterest> mapPoints;
    int selectedMapPointIndex = -1; // -1 = всплывающее окно закрыто
    std::unordered_map<std::string, sf::Texture> nodeIconTextures; // Иконки для точек карты

    // --- ДАННЫЕ АКТИВНОГО БОЯ ---
    enum class CombatPhase {
        PlayerTurn,
        EnemyTurn,
        Victory,
        Defeat
    };

    struct CombatUnit {
        std::string id;
        std::string name;
        int level = 1;
        double currentHp = 100.0;
        double maxHp = 100.0;
        double currentMana = 50.0;
        double maxMana = 50.0;
        double shield = 0.0;
        int skillStacks = 0;
        std::array<int, 3> skillCooldowns = { 0, 0, 0 };
        StatArray stats = { 100, 50, 20, 10, 0.05, 1.5, 0, 0 };
        bool isAlive() const { return currentHp > 0.001; }
    };

    struct CombatEnemy {
        std::string enemyId;
        std::string name;
        int level = 1;
        EnemyRank rank = EnemyRank::Normal;
        double currentHp = 100.0;
        double maxHp = 100.0;
        double attack = 15.0;
        double defense = 5.0;
        double shield = 0.0;
        std::vector<EnemyAttack> attacks;
        std::vector<int> attackCooldowns;
        bool isAlive() const { return currentHp > 0.001; }
    };

    struct CombatLogMessage {
        std::string message;
        ImVec4 color = ImVec4(0.9f, 0.9f, 0.9f, 1.0f);
    };

    std::vector<std::string> activeCombatPlayerSquad; // ID персонажей текущего отряда игрока
    std::vector<EnemySpawnInfo> activeCombatEnemies;  // Состав противников на уровне
    bool isCombatInitialized = false;
    CombatPhase combatPhase = CombatPhase::PlayerTurn;
    int combatTurnNumber = 1;
    size_t activeCharacterIndex = 0;
    int selectedEnemyTarget = 0;
    int combatRewardGold = 0;
    std::vector<CombatUnit> combatSquad;
    std::vector<CombatEnemy> combatEnemies;
    std::vector<CombatLogMessage> combatLog;
    bool scrollToBottomCombatLog = false;

    void logCombat(const std::string& msg, const ImVec4& color = ImVec4(0.9f, 0.9f, 0.9f, 1.0f));
    void initCombatState();
    void processPlayerSkill(size_t skillIndex);
    void executeEnemyTurn();
    void endCombatTurn();

    // Метод запуска боя с передачей отряда игрока и врагов уровня
    void startCombat(const std::vector<std::string>& playerSquadIds, const std::vector<EnemySpawnInfo>& enemies) {
        activeCombatPlayerSquad = playerSquadIds;
        activeCombatEnemies = enemies;
        isCombatInitialized = false;
        currentScreen = ScreenState::Combat;
    }
    // --- СИСТЕМА ДИАЛОГОВ ---
    bool isDialogueOpen = false;
    std::vector<DialogueNode> currentDialogue;
    int currentDialogueIndex = 0;

    // Запуск любого диалога одной функцией
    void startDialogue(const std::vector<DialogueNode>& nodes) {
        if (nodes.empty()) return;
        currentDialogue = nodes;
        currentDialogueIndex = 0;
        isDialogueOpen = true;
    }
    ImFont* mainMenuFont = nullptr;
    // Консоль
    bool isConsoleOpen = false; // Состояние отображения консоли
    char consoleInputBuf[256] = ""; // Буфер для ввода текста с клавиатуры
    struct ConsoleLogEntry {
        std::string text;
        ImVec4 color;
    };
    std::vector<ConsoleLogEntry> consoleHistory;
    bool scrollToBottomConsole = false;
    void logConsole(const std::string& message, ImVec4 color = ImVec4(0.9f, 0.9f, 0.9f, 1.0f));
    void executeConsoleCommand(const std::string& rawLine);

    bool loadResources();
    void renderBackground(sf::RenderWindow& window);

    void processEvent(const sf::Event& event); // Перехват нажатия клавиши ~
    void update(sf::RenderWindow& window);

    // --- НАСТРОЙКИ ИГРЫ ---
    WindowMode currentWindowMode = WindowMode::Fullscreen;
    WindowMode pendingWindowMode = WindowMode::Fullscreen;
    bool windowModeChanged = false;

    float musicVolume = 100.0f; // 0.0f .. 100.0f
    float sfxVolume = 100.0f;   // 0.0f .. 100.0f

    GameLanguage currentLanguage = GameLanguage::Russian;

    void setWindowMode(WindowMode mode);
    WindowMode getWindowMode() const { return currentWindowMode; }
    void applyWindowMode(sf::RenderWindow& window, WindowMode mode);

    float getMusicVolume() const { return musicVolume; }
    void setMusicVolume(float vol);

    float getSfxVolume() const { return sfxVolume; }
    void setSfxVolume(float vol);

    GameLanguage getLanguage() const { return currentLanguage; }
    void setLanguage(GameLanguage lang) { currentLanguage = lang; }
    const char* getLanguageCode() const;
    const char* getLanguageName() const;

    void renderCurrentScreen();
    void initMapPoints(); // Заполнение точек по этажам (с ручной настройкой координат)

    // Применение глобальной темы ImGui
    static void applyGameStyle();

    // Предупреждение при выходе из боя
    bool showCombatExitWarning = false;

    // Горячие клавиши боя и их переназначение
    CombatKeybinds combatKeybinds;
    int remappingActionIndex = -1; // -1: не переназначаем, 0..5: индекс действия
    static std::string getKeyDisplayName(sf::Keyboard::Key key);
    static std::string getFloorTitle(int floor);
    static std::string getSkillDetailedDescription(const std::shared_ptr<SkillTemplate>& skill, const CombatUnit& hero, size_t sIdx);

private:
    PlayerInventory playerInventory{ 250 }; // Инвентарь игрока (стартовый баланс: 250 монет)
    PlayerInventory* activeMerchantInventory = nullptr;
    std::string currentMerchantName = "Торговец";
    int* activeMerchantBalancePtr = nullptr;

    void renderMapPopup(); // Всплывающее окно поверх карты ("Перейти" / "Отмена")
    ItemTab currentInventoryTab = ItemTab::Weapons;
    ItemTab currentShopTab = ItemTab::Weapons;
    
    void renderInventory();

    // Вспомогательный метод перевода вкладки UI в категорию базы данных
    ItemCategory tabToCategory(ItemTab tab) const {
        switch (tab) {
        case ItemTab::Weapons:    return ItemCategory::Weapon;
        case ItemTab::Artifacts:  return ItemCategory::Artifact;
        case ItemTab::Food:       return ItemCategory::Food;
        case ItemTab::Materials:  return ItemCategory::Material;
        case ItemTab::Jewelry:    return ItemCategory::Jewelry;
        case ItemTab::QuestItems: return ItemCategory::Quest;
        }
        return ItemCategory::Material;
    }

    PlayerProfile playerProfile;
    ItemTab currentCraftingTab = ItemTab::Materials;    
    int maxUnlockedFloor = 10;
    std::array<std::string, 9> craftingGrid = { "", "", "", "", "", "", "", "", "" };
    std::array<std::string, 9> cookingGrid = { "", "", "", "", "", "", "", "", "" };
    bool isSquadEditorInitialized = false;
    std::array<std::shared_ptr<PlayerCharacter>, 3> tempSquad = { nullptr, nullptr, nullptr };
    bool showCharacterSelector = false;
    int currentEditingSlot = -1;
    std::unordered_map<ItemTab, sf::Texture> tabTextures;
    sf::Texture mainMenuBgTexture;
    sf::Sprite mainMenuBgSprite;
    sf::Texture combatBgTexture;
    sf::Sprite combatBgSprite;
    sf::Texture lobbyBgTexture;
    sf::Sprite lobbyBgSprite;
    sf::Texture floorBgTextures[11];
    sf::Sprite floorBgSprites[11];

    void clearCraftingGrid() {
        for (auto& slot : craftingGrid) {
            slot = "";
        }
    }

    void renderConsole(); // Метод рендера консоли поверх остальных окон
    void clearCookingGrid() {
        for (auto& slot : cookingGrid) {
            slot = "";
        }
    }
    // Блоки экранов
    void renderMainMenu();
    void renderSettings();
    void renderSaves();
    void renderLobby();
    void renderShop();
    void renderMap();
    void renderAchievements();
    void renderSquadEditor();
    void renderCharacterEditor();
    void renderCombat();
    void renderCooking();
    void renderCrafting();
    void advanceDialogue(int nextIndex); // Переход к следующей фразе или закрытие
    void renderDialogue();
};

#endif // SCREEN_MANAGER_HPP
