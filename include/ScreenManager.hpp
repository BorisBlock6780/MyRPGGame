#ifndef SCREEN_MANAGER_HPP
#define SCREEN_MANAGER_HPP

#include <imgui.h>
#include <SFML/Graphics.hpp>
#include <string>
#include <unordered_map>

enum class ScreenState {
    MainMenu,
    Settings,
    Saves,
    Lobby,
    Shop,
    Inventory,
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
    ScreenState currentScreen = ScreenState::MainMenu;
    bool shouldQuit = false;

    // Консоль
    bool isConsoleOpen = false; // Состояние отображения консоли
    char consoleInputBuf[256] = ""; // Буфер для ввода текста с клавиатуры

    bool loadResources();
    void processEvent(const sf::Event& event); // Перехват нажатия клавиши ~
    void update(sf::RenderWindow& window);
    void renderCurrentScreen();

private:
    ItemTab currentInventoryTab = ItemTab::Weapons;
    ItemTab currentShopTab = ItemTab::Weapons;

    std::unordered_map<ItemTab, sf::Texture> tabTextures;

    void renderBottomNavigation();
    void renderConsole(); // Метод рендера консоли поверх остальных окон

    // Блоки экранов
    void renderMainMenu();
    void renderSettings();
    void renderSaves();
    void renderLobby();
    void renderShop();
    void renderInventory();
    void renderMap();
    void renderAchievements();
    void renderSquadEditor();
    void renderCharacterEditor();
    void renderCombat();
    void renderEnvironmentInteraction();
};

#endif // SCREEN_MANAGER_HPP