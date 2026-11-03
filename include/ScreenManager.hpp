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
    Combat,
    EnvironmentInteraction
};

enum class ItemTab {
    Weapons,
    Artifacts,
    Food,
    Materials,
    Jewelry,
    QuestItems
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

    // --- ДАННЫЕ АКТИВНОГО БОЯ (передаются с карты при переходе в бой) ---
    std::vector<std::string> activeCombatPlayerSquad; // ID персонажей текущего отряда игрока
    std::vector<EnemySpawnInfo> activeCombatEnemies;  // Состав противников на уровне

    // Метод запуска боя с передачей отряда игрока и врагов уровня
    void startCombat(const std::vector<std::string>& playerSquadIds, const std::vector<EnemySpawnInfo>& enemies) {
        activeCombatPlayerSquad = playerSquadIds;
        activeCombatEnemies = enemies;
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

    bool loadResources();
    void renderBackground(sf::RenderWindow& window);

    void processEvent(const sf::Event& event); // Перехват нажатия клавиши ~
    void update(sf::RenderWindow& window);
    void renderCurrentScreen();
    void initMapPoints(); // Заполнение точек по этажам (с ручной настройкой координат)
    

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
    int maxUnlockedFloor = 2;
    std::array<std::string, 9> craftingGrid = { "", "", "", "", "", "", "", "", "" };
    std::array<std::string, 9> cookingGrid = { "", "", "", "", "", "", "", "", "" };
    bool isSquadEditorInitialized = false;
    std::array<std::shared_ptr<PlayerCharacter>, 3> tempSquad = { nullptr, nullptr, nullptr };
    bool showCharacterSelector = false;
    int currentEditingSlot = -1;
    std::unordered_map<ItemTab, sf::Texture> tabTextures;
    sf::Texture mainMenuBgTexture;
    sf::Sprite mainMenuBgSprite;

    void clearCraftingGrid() {
        for (auto& slot : craftingGrid) {
            slot = "";
        }
    }

    void renderBottomNavigation();
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
    void renderEnvironmentInteraction();
    void renderCooking();
    void renderCrafting();
    void advanceDialogue(int nextIndex); // Переход к следующей фразе или закрытие
    void renderDialogue();
};

#endif // SCREEN_MANAGER_HPP